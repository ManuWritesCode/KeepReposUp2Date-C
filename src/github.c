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



void init_string( struct string *s )
{
    s->len = 0;
    s->ptr = ( char * )malloc( s->len + 1 );
    if ( s->ptr == NULL ) {
        fprintf( stderr, "malloc() failed\n" );
        exit( EXIT_FAILURE );
    }
    s->ptr[0] = '\0';
}


size_t writefunc( void *ptr, size_t size, size_t nmemb, struct string *s )
{
    size_t new_len = s->len + size * nmemb;
    s->ptr = ( char * )realloc( s->ptr, new_len + 1 );
    if ( s->ptr == NULL ) {
        fprintf( stderr, "realloc() failed\n" );
        exit( EXIT_FAILURE );
    }
    memcpy( s->ptr + s->len, ptr, size * nmemb );
    s->ptr[new_len] = '\0';
    s->len = new_len;

    return size * nmemb;
}


struct repo_names parse_and_get_repo_names( const char *json )
{
    struct json_object *parsed_json;
    struct json_object *repos;
    struct json_object *repo;
    struct json_object *name;
    struct json_object *ssh_url;
    size_t n_repos;
    size_t i;

    struct repo_names result;
    result.names = NULL;
    result.urls = NULL;
    result.count = 0;

    parsed_json = json_tokener_parse( json );
    if ( parsed_json == NULL ) {
        fprintf( stderr, "Failed to parse JSON\n" );
        return result;
    }

    n_repos = json_object_array_length( parsed_json );
    result.names = malloc( n_repos * sizeof( char * ) );
    result.urls = malloc( n_repos * sizeof( char * ) );
    if ( result.names == NULL ) {
        fprintf( stderr, "malloc() failed\n" );
        json_object_put( parsed_json );
        free( result.names );
        free( result.urls );
        return result;
    }

    for ( i = 0; i < n_repos; i++ ) {
        repo = json_object_array_get_idx( parsed_json, i );
        
        // Extract the repository name
        if ( json_object_object_get_ex( repo, "name", &name ) &&
             json_object_is_type( name, json_type_string ) ) {
                result.names[result.count] = strdup( json_object_get_string( name ) );
                if ( result.names[result.count] == NULL ) {
                    fprintf( stderr, "strdup() failed for name\n" );
                    goto cleanup;
                }
        } else {
            result.names[result.count] = NULL;
        }

        // Extract the repository URL
        if ( json_object_object_get_ex( repo, "ssh_url", &ssh_url ) &&
             json_object_is_type( ssh_url, json_type_string ) ) {
                result.urls[result.count] = strdup( json_object_get_string( ssh_url ) );
                if ( result.urls[result.count] == NULL ) {
                    fprintf( stderr, "strdup() failed for URL\n" );
                    goto cleanup;
                }
        } else {
            result.urls[result.count] = NULL;
        }

        result.count++;
    }

    json_object_put(parsed_json);
    return result;

cleanup:
    for ( size_t j = 0; j < result.count; j++ ) {
        free( result.names[j] );
        free( result.urls[j] );
    }
    free( result.names );
    free( result.urls );
    json_object_put( parsed_json );
    result.names = NULL;
    result.urls = NULL;
    result.count = 0;

    return result;
}


void free_repo_names( struct repo_names *repos )
{
    if ( repos->names ) {
        for ( size_t i = 0; i < repos->count; i++ ) {
            free( repos->names[i] );
            free( repos->urls[i] );
        }
        free( repos->names );
        free( repos->urls );
    }

    repos->names = NULL;
    repos->urls = NULL;
    repos->count = 0;
}


struct repo_names fetch_github_repos( const char *github_token ) 
{
    CURL *curl;
    CURLcode res;
    struct string s;
    struct repo_names repos = { NULL, 0 };

    init_string( &s );

    curl_global_init( CURL_GLOBAL_DEFAULT );
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

int credentials_callback( git_cred **cred, const char *url, const char *username_from_url,
    unsigned int allowed_types, void *payload )
{
    kru2d_conf *conf = ( kru2d_conf * )payload;

    return git_cred_ssh_key_new( cred, username_from_url ? username_from_url : "git",
           conf->ssh_public_key, conf->ssh_private_key, conf->ssh_passphrase );
}

int clone_repo( const char *repo_url, const char *local_path, const kru2d_conf *conf )
{   
    const char *url = repo_url;
    const char *local = local_path;
    git_repository *git_repo = NULL;
    git_clone_options clone_opts = GIT_CLONE_OPTIONS_INIT;
    git_remote_callbacks callbacks = GIT_REMOTE_CALLBACKS_INIT;


    // Configures the SSH authentication callback
    callbacks.credentials = credentials_callback;
    callbacks.payload = ( kru2d_conf * )conf;
    clone_opts.fetch_opts.callbacks = callbacks;

    if ( git_libgit2_init() < 0 ) {
        fprintf( stderr, "Failed to initialize libgit2\n" );
        return -1;
    }

    if ( git_clone( &git_repo, repo_url, local, &clone_opts ) != 0 ) {
        const git_error *e = git_error_last();
        fprintf( stderr, "\tError while cloning %s: %s\n\n", url, 
                e && e->message ? e->message : "Unknown error" );
        return -1;
    }

    if ( git_repo ) {
        git_repository_free( git_repo );
    }
    
    git_libgit2_shutdown();

    return 0;
}


int pull_is_needed( const char *local_path, const kru2d_conf *conf )
{
    int result = 1; // By default, a pull is needed

    git_repository *repo = NULL;
    git_remote *remote = NULL;
    git_oid local_head_oid, remote_head_oid;
    const git_remote_head **remote_heads;
    size_t remote_heads_count = 0;
    git_remote_callbacks callbacks = GIT_REMOTE_CALLBACKS_INIT;

    // Configures the SSH authentication callback
    callbacks.credentials = credentials_callback;
    callbacks.payload = ( kru2d_conf * )conf;

    const git_error *e;


    // Open the local repository
    if ( git_repository_open( &repo, local_path ) != 0 ) {
        e = git_error_last();
        fprintf( stderr, "Error opening local repository %s : %s\n", local_path, e && e->message ? e->message : "Unknown error" );
        return -1;
    }

    // Get local HEAD OID
    if ( git_reference_name_to_id( &local_head_oid, repo, "HEAD" ) != 0 ) {
        e = git_error_last();
        fprintf( stderr, "Failed to get local HEAD OID for %s : %s\n", local_path, e && e->message ? e->message : "Unknown error" );
        return -1;
    }

    // Get remote 'origin'
    if ( git_remote_lookup( &remote, repo, "origin" ) != 0 ) {
        e = git_error_last();
        fprintf( stderr, "Failed to find remote 'origin' for %s : %s\n", local_path, e && e->message ? e->message : "Unknown error" );
        return -1;
    }

    // Connect to remote and get references
    if ( git_remote_connect( remote, GIT_DIRECTION_FETCH, &callbacks, NULL, NULL ) != 0 ) {
        e = git_error_last();
        fprintf( stderr, "Failed to connect to remote 'origin' : %s\n", e && e->message ? e->message : "Unknown error" );
        return -1;
    }

    if ( git_remote_ls( &remote_heads, &remote_heads_count, remote ) != 0 ) {
        e = git_error_last();
        fprintf( stderr, "Failed to list references from %s : %s\n\t%s\n", git_remote_name( remote ), git_remote_url( remote ), e && e->message ? e->message : "Unknown error" );
        return -1;
    }
    
    // Get remote HEAD OID ( refs/heads/main or refs/heads/master )
    for ( size_t i = 0; i < remote_heads_count; i++ ) {
        if ( strcmp( remote_heads[i]->name, "refs/heads/main" ) || strcmp( remote_heads[i]->name, "refs/heads/master" ) == 0 ) {
            git_oid_cpy( &remote_head_oid, &remote_heads[i]->oid );
            break;
        }
    } 

    // Compare OIDs of local and remote HEAD
    if ( git_oid_cmp( &local_head_oid, &remote_head_oid ) == 0 ) {
        return 0;
    }

    git_remote_free( remote );
    git_repository_free( repo );

    return result;
}


int pull_repo( const char *local_path, const kru2d_conf *conf )
{
    git_repository *repo = NULL;
    git_remote *remote = NULL;
    git_reference *fetch_head = NULL;
    git_annotated_commit *fetch_head_commit = NULL;

    git_merge_options merge_opts = GIT_MERGE_OPTIONS_INIT;
    merge_opts.flags = GIT_MERGE_FIND_RENAMES;

    git_checkout_options checkout_opts = GIT_CHECKOUT_OPTIONS_INIT;
    checkout_opts.checkout_strategy = GIT_CHECKOUT_SAFE;

    git_remote_callbacks callbacks = GIT_REMOTE_CALLBACKS_INIT;
    callbacks.credentials = credentials_callback;
    callbacks.payload = ( void * )conf;

    git_fetch_options fetch_opts = GIT_FETCH_OPTIONS_INIT;
    fetch_opts.callbacks = callbacks;

    const git_error *e;

    int result = -1; // By defaut, failed

    // Open local repository
    if ( git_repository_open( &repo, local_path ) != 0 ) {
        e = git_error_last();
        fprintf( stderr, "Failed to open local repository %s : %s\n", local_path, e && e->message ? e->message : "Unknown error" );
        goto cleanup;
    }

    // Get remote 'origin'
    if ( git_remote_lookup( &remote, repo, "origin" ) != 0 ) {
        e = git_error_last();
        fprintf( stderr, "Failed to find remote 'origin' of %s : %s\n" , local_path, e && e->message ? e->message : "Unknown error" );
        goto cleanup;
    }

    // Fetch modifications from remote
    if ( git_remote_fetch( remote, NULL, &fetch_opts, NULL ) != 0 ) {
        e = git_error_last();
        fprintf( stderr, "Failed to fetch from %s remote 'origin' : %s\n", local_path, e && e->message ? e->message : "Unknown error" );
        goto cleanup;
    }

    // Get FETCH_HEAD reference
    if ( git_reference_lookup( &fetch_head, repo, "FETCH_HEAD" ) != 0 ) {
        e = git_error_last();
        fprintf( stderr, "Failed to lookup FETCH_HEAD of %s : %s\n", local_path, e && e->message ? e->message : "Unknown error" );
        goto cleanup;
    }

    // Create a annotated commit from FETCH_HEAD
    if ( git_annotated_commit_from_ref( &fetch_head_commit, repo, fetch_head ) != 0 ) {
        e = git_error_last();
        fprintf( stderr, "Failed to create annotated commit from FETCH_HEAD of %s : %s\n", local_path, e && e->message ? e->message : "Unknown error" );
        goto cleanup;
    }

    // Merge FETCH_HEAD in the local branch
    if ( git_merge( repo, ( const git_annotated_commit ** )&fetch_head_commit, 1, &merge_opts, &checkout_opts ) != 0 ) {
        e = git_error_last();
        fprintf( stderr, "Failed to merge FETCH_HEAD of %s into the current branch : %s\n", local_path, e && e->message ? e->message : "Unknown error" );
        goto cleanup;
    }

    // Verify if a commit is needed to end merging
    if ( git_repository_state( repo ) == GIT_REPOSITORY_STATE_MERGE ) {
        fprintf( stdout, "\tFinalizing merge with a commit...\n" );

        git_index *index = NULL;
        if ( git_repository_index( &index, repo ) != 0 ) {
            e = git_error_last();
            fprintf( stderr, "Failed to get repository index : %s\n", e && e->message ? e->message : "Unknown error" );
            goto cleanup;
        }

        // Check conflicts
        if ( git_index_has_conflicts( index ) ) {
            fprintf( stderr, "Merge conflicts detected. Resolve them before committing.\n" );
            git_index_free( index );
            goto cleanup;
        }

        // Create a merge commit
        git_signature *signature = NULL;
        if ( git_signature_default( &signature, repo ) != 0 ) {
            e = git_error_last();
            fprintf( stderr, " Failed to create default signature : %s\n", e && e->message ? e->message : "Unknown error" );
            git_index_free( index );
            goto cleanup;
        }

        git_oid tree_oid, commit_oid;
        git_tree *tree = NULL;

        if ( git_index_write_tree( &tree_oid, index ) != 0 ) {
            e = git_error_last();
            fprintf( stderr, "Failed to write tree : %s\n", e && e->message ? e->message : "Unknown error" );
            git_signature_free( signature );
            git_index_free( index );
            goto cleanup;
        }

        if ( git_tree_lookup( &tree, repo, &tree_oid ) != 0 ) {
            e = git_error_last();
            fprintf( stderr, "Failed to lookup tree : %s\n", e && e->message ? e->message : "Unknown error" );
            git_signature_free( signature );
            git_index_free( index );
            goto cleanup;
        }

        git_reference *head_ref = NULL;
        if ( git_repository_head( &head_ref, repo ) != 0 ) {
            e = git_error_last();
            fprintf( stderr, "Failed to get HEAD reference : %s\n", e && e->message ? e->message : "Unknown error" );
            git_tree_free( tree );
            git_signature_free( signature );
            git_index_free( index );
            goto cleanup;
        }

        git_oid head_oid;
        git_commit *head_commit = NULL;

        // Get the current branch's HEAD commit
        if ( git_reference_name_to_id( &head_oid, repo, "HEAD" ) != 0 ) {
            e = git_error_last();
            fprintf( stderr, "Failed to get HEAD OID : %s\n", e && e->message ? e->message : "Unknown error" );
            git_tree_free( tree );
            git_signature_free( signature );
            git_index_free( index );
            goto cleanup;
        }

        if ( git_commit_lookup( &head_commit, repo, &head_oid ) != 0 ) {
            e = git_error_last();
            fprintf( stderr, "Failed to lookup HEAD commit : %s\n", e && e->message ? e->message : "Unknown error" );
            git_tree_free( tree );
            git_signature_free( signature );
            git_index_free( index );
            goto cleanup;
        }

        git_commit *parent_commit = NULL;
        if ( git_commit_lookup( &parent_commit, repo, git_annotated_commit_id( fetch_head_commit ) ) != 0 ) {
            e = git_error_last();
            fprintf( stderr, "Failed to lookup FETCH_HEAD commit : %s\n", e && e->message ? e->message : "Unknown error" );
            git_commit_free( head_commit );
            git_tree_free( tree );
            git_signature_free( signature );
            git_index_free( index );
            goto cleanup;
        }

        const git_commit *parent_commits[] = { head_commit, parent_commit };
        if ( git_commit_create( &commit_oid, repo, "HEAD", signature, signature, NULL,
                                    "Merge commit", tree, 1, parent_commits ) != 0 ) {
            e = git_error_last();
            fprintf( stderr, "Failed to create merge commit : %s\n", e && e->message ? e->message : "Unknon error" );
            git_reference_free( head_ref );
            git_tree_free( tree );
            git_signature_free( signature );
            git_index_free( index );
            git_commit_free( parent_commit );
            goto cleanup;
        }

        if ( git_repository_state_cleanup( repo ) != 0 ) {
            e = git_error_last();
            fprintf( stderr, "Failed to clean up repository state : %s\n", e && e->message ? e->message : "Unknown error" );
            goto cleanup;
        }

        
        git_commit_free ( parent_commit );
        git_reference_free( head_ref );
        git_tree_free( tree );
        git_signature_free( signature );
        git_index_free( index );
    } else {
        fprintf( stdout, "NOT in MERGE state. No commit willbe created.\n" );
        goto cleanup;
    }

    result = 0;

cleanup:
    if (fetch_head_commit) git_annotated_commit_free(fetch_head_commit);
    if (fetch_head) git_reference_free(fetch_head);
    if (remote) git_remote_free(remote);
    if (repo) git_repository_free(repo);

    return result;
}