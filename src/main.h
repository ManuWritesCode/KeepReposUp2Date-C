/*************************************************************************************************************
 * 
 * KEEP REPOS UP TO DATE (KeepReposUp2Date)
 * 
 * This file is part of the KeepReposUp2Date program (https://github.com/ManuWritesCode/KeepReposUp2Date-C).
 * Copyright (c) 2025 Emmanuel BOIDIN (@ManuWritesCode).
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 *************************************************************************************************************/

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <signal.h>
#include "github.h"


/* Structure of a repository
 * REPO_URL : URL of the repository
 * LOCAL_PATH : Local path of the repository 
 */
typedef struct {
    char *repo_url;
    char *local_path;
    const char *private_key;
    const char *public_key;
} thread_args_t;

/*
 * Intercepts handle signal
 */
void handle_sigint( int sig ) {
    fprintf( stderr, "Interrupt signal received./ Exiting...\n" );
    exit( 1 );
}

/* Reads the configuration file to get :
 * DEV_PATH : the path where all projects are stored
 * GITHUB_TOKEN : your Github token to connect to your Github account
 */
void load_conf( const char *filename )
{
    FILE *file = fopen( filename, "r" );
    char line[256];


    if ( file == NULL ) {
        fprintf(stderr, "Failed to open the configuration file: %s\n", filename);
        exit( EXIT_FAILURE );
    }

    while ( fgets( line, sizeof( line ), file )) {
        // Remove newline character
        line[ strcspn( line, "\n" ) ] = 0;

        // Split the line into key and value
        char *key = strtok( line, "=" );
        char *value= strtok( NULL, "=" );

        if ( key && value ) {
            if ( setenv( key, value, 1 ) != 0 ) {
                fprintf( stderr, "Failed to set environement variable : %s\n", key );
            }
        }
    }

    fclose( file );
}


/* Check if a directory exists */
int directory_exists( const char *path ) {
    struct stat st;

    return ( stat( path, &st ) == 0 && S_ISDIR( st.st_mode ) );
}



void *thread_clone_or_pull_repo( void *arg )
{
    thread_args_t *args = (thread_args_t *)arg;

    const char *private_key = args->private_key;
    const char *public_key = args->public_key;

    if ( !private_key || !public_key ) {
        fprintf( stderr, "SSH keys not privided to thread !\n" );
        pthread_exit( NULL );
    }

    //printf("Thread using SSH_PRIVATE_KEY=%s\n", private_key);
    //printf("Thread using SSH_PUBLIC_KEY=%s\n", public_key);
    
    if ( directory_exists( args->local_path ) ) {
        fprintf( stdout, "Repository exists locally. Pulling : %s to %s\n", args->repo_url, args->local_path );

        if ( pull_repo( args->local_path ) != 0 ) {
            fprintf( stderr, "Failed to pull repository : %s\n", args->local_path );
        } 
    } else {
        fprintf( stdout, "Repository does not exist locally. Cloning : %s\n", args->repo_url );

        //if ( clone_repo( args->repo_url, args->local_path ) != 0 ) {
        //    fprintf( stderr, "Failed to clone repository : %s\n", args->repo_url );
        //}
    }

    //pthread_exit( NULL );
    return NULL;
}