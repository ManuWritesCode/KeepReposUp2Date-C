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

#pragma once
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <signal.h>




/*
 * Structure containing all the variables in the configuration file
 */
typedef struct {
    const char *dev_path;
    const char *github_token;
    const char *github_username;
    const char *ssh_private_key;
    const char *ssh_public_key;
    const char *ssh_passphrase;
} kru2d_conf;

/*
 * Structures to pass arguments to threads
 */
typedef struct {
    kru2d_conf *conf;
    struct repo_names *repos;
    size_t repo_index;
} thread_args_t;



/*
 * Intercepts Ctrl-C signal 
 */
void handle_sigint( int sig );

/* 
 * Reads the configuration file in the HOME user path
 */
void load_conf();

/*
 * Check if a directory exists 
 */
int directory_exists( const char *path );

/* 
 * Thread to clone or pull a repository 
 */
void *thread_clone_or_pull_repo( void *arg );