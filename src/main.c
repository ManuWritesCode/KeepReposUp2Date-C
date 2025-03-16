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

    CURL *curl;
    CURLcode res;
    struct string s;


    if ( dev_path && github_token ) {
        //fprintf( stdout, "DEV_PATH=%s\n", dev_path );
        //fprintf( stdout, "GITHUB_TOKEN=%s\n", github_token );
    }

    init_string(&s);

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "User-Agent: KeepReposUp2Date");
        headers = curl_slist_append(headers, "Accept: application/vnd.github.v3+json");

        char auth_header[256];
        snprintf(auth_header, sizeof(auth_header), "Authorization: token %s", github_token);
        headers = curl_slist_append(headers, auth_header);

        curl_easy_setopt(curl, CURLOPT_URL, "https://api.github.com/user/repos");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            struct repo_names repos = parse_and_get_repo_names(s.ptr);
            for (size_t i = 0; i < repos.count; i++) {
                printf("Repo: %s\n", repos.names[i]);
                free(repos.names[i]);
            }
            free(repos.names);
        }

        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }

    free(s.ptr);
    curl_global_cleanup();

    return 0;
}