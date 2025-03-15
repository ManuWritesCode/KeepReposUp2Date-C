#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <json-c/json.h>

struct string {
    char *ptr;
    size_t len;
};

void init_string(struct string *s) {
    s->len = 0;
    s->ptr = malloc(s->len + 1);
    if (s->ptr == NULL) {
        fprintf(stderr, "malloc() failed\n");
        exit(EXIT_FAILURE);
    }
    s->ptr[0] = '\0';
}

size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s) {
    size_t new_len = s->len + size * nmemb;
    s->ptr = realloc(s->ptr, new_len + 1);
    if (s->ptr == NULL) {
        fprintf(stderr, "realloc() failed\n");
        exit(EXIT_FAILURE);
    }
    memcpy(s->ptr + s->len, ptr, size * nmemb);
    s->ptr[new_len] = '\0';
    s->len = new_len;

    return size * nmemb;
}

void parse_and_print_repos(const char *json) {
    struct json_object *parsed_json;
    struct json_object *repos;
    struct json_object *repo;
    struct json_object *name;
    struct json_object *html_url;
    size_t n_repos;
    size_t i;

    parsed_json = json_tokener_parse(json);
    if (parsed_json == NULL) {
        fprintf(stderr, "Failed to parse JSON\n");
        return;
    }

    n_repos = json_object_array_length(parsed_json);
    for (i = 0; i < n_repos; i++) {
        repo = json_object_array_get_idx(parsed_json, i);
        json_object_object_get_ex(repo, "name", &name);
        json_object_object_get_ex(repo, "html_url", &html_url);

        printf("Repo: %s, URL: %s\n", json_object_get_string(name), json_object_get_string(html_url));
    }

    json_object_put(parsed_json);
}

void load_env(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Failed to open .env file");
        exit(EXIT_FAILURE);
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        // Remove newline character
        line[strcspn(line, "\n")] = 0;

        // Split the line into key and value
        char *key = strtok(line, "=");
        char *value = strtok(NULL, "=");

        if (key && value) {
            setenv(key, value, 1);
        }
    }

    fclose(file);
}

int main(void) {
    load_env(".env");

    const char *hostname_pc1 = getenv("HOSTNAME_PC1");
    const char *projects_dir_pc1 = getenv("PROJECTS_DIR_PC1");

    if (hostname_pc1 && projects_dir_pc1) {
        printf("Hostname PC1: %s\n", hostname_pc1);
        printf("Projects Directory PC1: %s\n", projects_dir_pc1);
    } else {
        fprintf(stderr, "Failed to load environment variables\n");
    }

    CURL *curl;
    CURLcode res;
    struct string s;
    char *token = "YOUR_GITHUB_TOKEN"; // Remplacez par votre token GitHub

    init_string(&s);

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "User-Agent: KeepReposUp2Date");
        headers = curl_slist_append(headers, "Accept: application/vnd.github.v3+json");

        char auth_header[256];
        snprintf(auth_header, sizeof(auth_header), "Authorization: token %s", token);
        headers = curl_slist_append(headers, auth_header);

        curl_easy_setopt(curl, CURLOPT_URL, "https://api.github.com/user/repos");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            parse_and_print_repos(s.ptr);
        }

        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }

    free(s.ptr);
    curl_global_cleanup();

    return 0;
}