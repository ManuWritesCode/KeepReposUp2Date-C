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

#include "github.h"

void handle_sigint( int sig ) 
{
    fprintf( stderr, "Interrupt signal received./ Exiting...\n" );
    exit( 1 );
}


void load_conf()
{
    const char *home;
    FILE *file;
    char line[256];
 
    home = getenv( "HOME" );
    if ( home == NULL ) {
        fprintf( stderr, "Failed to get the HOME environment variable.\n" );
        exit( EXIT_FAILURE );
    }

    // Constructs the complete path to the configuration file
    size_t path_length = strlen( home ) + strlen( "/.config/KeepReposUp2Date/kru2d.conf" ) + 1;
    char *config_path = malloc( path_length );
    if ( config_path == NULL ) {
        fprintf( stderr, "Failed to allocate memory for config_path.\n" );
        exit( EXIT_FAILURE );
    }
    snprintf( config_path, path_length, "%s/.config/KeepReposUp2Date/kru2d.conf", home );
 
    file = fopen( config_path, "r" );
    if ( file == NULL ) {
        fprintf(stderr, "Failed to open the configuration file: %s\n", config_path);
        exit( EXIT_FAILURE );
    }
 
    while ( fgets( line, sizeof( line ), file )) {
         // Remove newline character
        line[ strcspn( line, "\n" ) ] = 0;
 
        // Split the line into key and value
        char *key = strtok( line, "=" );
        char *value= strtok( NULL, "=" );
 
        // Set program environment
        if ( key && value ) {
            if ( setenv( key, value, 1 ) != 0 ) {
                fprintf( stderr, "Failed to set environement variable : %s\n", key );
            }
        }
    }

    free( config_path );
    fclose( file );
}


int directory_exists( const char *path ) {
    struct stat st;

    return ( stat( path, &st ) == 0 && S_ISDIR( st.st_mode ) );
}


void *thread_clone_or_pull_repo( void *arg )
{
    thread_args_t *args = ( thread_args_t * )arg;

    kru2d_conf *conf = args->conf;

    struct repo_names *repos = args->repos;
    size_t index = args->repo_index;

    char local_path[512];
    snprintf( local_path, sizeof( local_path ), "%s/%s", conf->dev_path, repos->names[index] );

    

    // If a local directory exists, pulling it, else cloning from Github
    if ( directory_exists( local_path ) ) {
        fprintf( stdout, "Pulling into: %s from %s...\n", local_path, repos->urls[index] );
        
        if ( pull_repo( local_path, conf ) != 0 ) {
            fprintf( stderr, "\tError while pulling %s from %s\n\n", repos->names[index], repos->urls[index] );
            pthread_exit( NULL );
        }else {
            fprintf( stdout, "\tRepository %s pulled successfully from %s\n\n", repos->names[index], repos->urls[index] );
        }
    } else {
        fprintf( stdout, "Cloning into: %s from %s...\n", local_path, repos->urls[index] );

        if ( clone_repo( repos->urls[index], local_path, conf ) != 0 ) {
            fprintf( stderr, "\tError while cloning %s from %s\n\n", repos->names[index], repos->urls[index] );
            pthread_exit( NULL );
        } else {
            fprintf( stdout, "\tRepository %s cloned successfully from %s\n\n", repos->names[index], repos->urls[index] );
        }
        
    }

   return NULL;
}