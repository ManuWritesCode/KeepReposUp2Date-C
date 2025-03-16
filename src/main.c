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

#include "main.h"
#include "github.h"


int main ( void )
{    
    // Loads configuration file
    load_conf( "/Users/manu/Documents/02_DEV/KeepReposUp2Date-C/kru2d.conf" );

    // Gets the main development path
    const char *dev_path = getenv( "DEV_PATH" );

    // Gets the Github token
    const char *github_token = getenv( "GITHUB_TOKEN" );

   
    if ( !dev_path && !github_token ) {
        fprintf( stderr, "Please, fill the DEV_PATH or/and GITHUB_TOKEN to your kru2d.conf configuration file !\n" );
        exit( EXIT_FAILURE );
    }

    // Gets all Github repositories
    struct repo_names repos = fetch_github_repos( github_token );

    pthread_t thr[repos.count];

    for ( size_t i = 0; i < repos.count; i++ ) {
        // Creates a thread per Github repository
        if (pthread_create(&thr[i], NULL, thread_clone_or_pull_repo, ( void * )i) != 0) {
            fprintf(stderr, "Error during pthread_create()\n");
            exit(EXIT_FAILURE);
        }
        
        //printf( "Repo : %s\n", repos.names[i] );
        free( repos.names[i] );
    }
    
    free( repos.names );
    
    pthread_exit(NULL);
}