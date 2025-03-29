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
#include <curl/curl.h>
#include <json-c/json.h>
#include <git2.h>
#include "kru2d.h"

 
 /* 
  * Structure containing the Github API response 
  */
struct string {
    char *ptr;
    size_t len;
};

/* 
 * Structure to hold the repository names 
 */
struct repo_names {
    char **names;
    char **urls;
    size_t count;
};


/* 
 * Initialize the structure containing the Github API response 
 */
void init_string(struct string *s);

/* 
 * Callback function to write data from Github API response 
 */
size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s);

/* 
 * Function to parse and return repository names 
 */
struct repo_names parse_and_get_repo_names(const char *json);

/* 
 * Free memory for repos_name structure 
 */
void free_repo_names( struct repo_names *repos );

/* 
 * Fetch Github repositories 
 */
struct repo_names fetch_github_repos( const char *github_token );

/*
 * Callback for SSH authentication
 */
int credentials_callback( git_cred **cred, const char *url, const char *username_from_url,
    unsigned int allowed_types, void *payload );

/* 
 * Clone a repository 
 */
int clone_repo( const char *repo_url, const char *local_path, const kru2d_conf *conf );

/*
 * Verify if a pull is needed
 */
int pull_is_needed( const char *local_path, const kru2d_conf *conf );

/*
 * Pull a repository
 */
int pull_repo( const char *local_path, const kru2d_conf *conf );