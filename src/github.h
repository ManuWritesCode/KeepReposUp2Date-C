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
void init_string(struct string *s) {
    s->len = 0;
    s->ptr = (char *)malloc(s->len + 1);
    if (s->ptr == NULL) {
        fprintf(stderr, "malloc() failed\n");
        exit(EXIT_FAILURE);
    }
    s->ptr[0] = '\0';
}


/* Callback function to write data from Github API response */
size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s) {
    size_t new_len = s->len + size * nmemb;
    s->ptr = (char *)realloc(s->ptr, new_len + 1);
    if (s->ptr == NULL) {
        fprintf(stderr, "realloc() failed\n");
        exit(EXIT_FAILURE);
    }
    memcpy(s->ptr + s->len, ptr, size * nmemb);
    s->ptr[new_len] = '\0';
    s->len = new_len;

    return size * nmemb;
}


/* Function to parse and return repository names */
struct repo_names parse_and_get_repo_names(const char *json) {
    struct json_object *parsed_json;
    struct json_object *repos;
    struct json_object *repo;
    struct json_object *name;
    size_t n_repos;
    size_t i;

    struct repo_names result;
    result.names = NULL;
    result.count = 0;

    parsed_json = json_tokener_parse(json);
    if (parsed_json == NULL) {
        fprintf(stderr, "Failed to parse JSON\n");
        return result;
    }

    n_repos = json_object_array_length(parsed_json);
    result.names = malloc(n_repos * sizeof(char *));
    if (result.names == NULL) {
        fprintf(stderr, "malloc() failed\n");
        json_object_put(parsed_json);
        return result;
    }

    for (i = 0; i < n_repos; i++) {
        repo = json_object_array_get_idx(parsed_json, i);
        json_object_object_get_ex(repo, "name", &name);

        if (json_object_is_type(name, json_type_string)) {
            result.names[result.count] = strdup(json_object_get_string(name));
            if (result.names[result.count] == NULL) {
                fprintf(stderr, "strdup() failed\n");
                // Free previously allocated names
                for (size_t j = 0; j < result.count; j++) {
                    free(result.names[j]);
                }
                free(result.names);
                json_object_put(parsed_json);
                result.names = NULL;
                result.count = 0;
                return result;
            }
            result.count++;
        }
    }

    json_object_put(parsed_json);
    return result;
}


/* Fetch Github repositories */
struct repo_names fetch_github_repos( const char *github_token ) {

    CURL *curl;
    CURLcode res;
    struct string s;
    struct repo_names repos = { NULL, 0 };

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
            repos = parse_and_get_repo_names( s.ptr );
        }

        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }

    free(s.ptr);
    curl_global_cleanup();

    return repos;
}