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


/* Reads the configuration file to get :
 * DEV_PATH : the path where all projects are stored
 * GITHUB_TOKEN : your Github token to connect to your Github account
 */
void load_conf( const char *filename )
{
    FILE *file = fopen( filename, "r" );
    char line[256];


    if ( file == NULL ) {
        perror( "Failed to open the configuration file" );
        exit( EXIT_FAILURE );
    }

    while ( fgets( line, sizeof( line ), file )) {
        // Remove newline character
        line[ strcspn( line, "\n" ) ] = 0;

        // Split the line into key and value
        char *key = strtok( line, "=" );
        char *value= strtok( NULL, "=" );

        if ( key && value ) {
            setenv( key, value, 1 );
        }
    }

    fclose( file );
}


void *thread_clone_or_pull_repo( void *arg )
{
    long num = (long) arg;
    
    fprintf(stderr, "Thread #%ld\n", num);
}