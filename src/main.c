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

#include "kru2d.h"
#include "github.h"

int main ( void )
{
    kru2d_conf conf;

    signal( SIGINT, handle_sigint );
 
    // Loads configuration file
    load_conf();

    // Stores all environment variables in the kru2d_conf structure
    conf.dev_path = getenv( "DEV_PATH" );
    conf.github_token = getenv( "GITHUB_TOKEN" );
    conf.github_username = getenv( "GITHUB_USERNAME" );
    conf.ssh_private_key = getenv( "SSH_PRIVATE_KEY" );
    conf.ssh_public_key = getenv( "SSH_PUBLIC_KEY" );
    conf.ssh_passphrase = getenv( "SSH_PASSPHRASE" );
    
    if ( conf.dev_path == NULL || conf.github_token == NULL || conf.github_username == NULL ||
         conf.ssh_private_key == NULL || conf.ssh_public_key == NULL || conf.ssh_passphrase == NULL ) {
        fprintf( stderr, "One or more environment variables not defined. Please, modify your configuration file !\n" );
        exit( EXIT_FAILURE );
    }

    // Gets all Github repositories
    struct repo_names repos = fetch_github_repos( conf.github_token );
    
    // Threads to parallaize clone or pull repos
    pthread_t thr[repos.count];
    thread_args_t args[repos.count];

    // For each repository, creates a thread to clone or pull the repository
    for ( size_t i = 0; i < repos.count; i++ ) {
        args[i].conf = &conf;
        args[i].repos = &repos;
        args[i].repo_index = i;

        if ( pthread_create( &thr[i], NULL, thread_clone_or_pull_repo, &args[i] ) != 0 ) {
            fprintf( stderr, "Error while creating thread for repository %s\n", repos.names[i] );
        }
        pthread_join( thr[i], NULL );
    }

    free_repo_names( &repos );

    return 0;
}