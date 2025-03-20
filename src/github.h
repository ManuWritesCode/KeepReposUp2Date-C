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
#include <curl/curl.h>
#include <json-c/json.h>


/* Structure containing the Github API response */
struct string {
    char *ptr;
    size_t len;
};

/* Structure to hold the repository names */
struct repo_names {
    char **names;
    size_t count;
};


/* Initialize the structure containing the Github API response */
void init_string(struct string *s);


/* Callback function to write data from Github API response */
size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s);


/* Function to parse and return repository names */
struct repo_names parse_and_get_repo_names(const char *json);

/* Fetch Github repositories */
struct repo_names fetch_github_repos( const char *github_token );


/* Clone a repository */
int clone_repo( const char *repo_url, const char *local_path );


/* Pull changes from a repository */
int pull_repo( const char *local_path );