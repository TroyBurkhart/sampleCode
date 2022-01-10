/*
Takes crawled pages and counts the words over 3 letters. Uses a hash table of sets of counters to count the words in each file
Outputs how many times the word appears with the unique page IDs
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <hashtable.h>
#include <counters.h>
#include <webpage.h>
#include <word.h>

void index(char *dirName, char *filePath, hashtable_t *hash);
void parseWords(webpage_t *page, int id, hashtable_t *hash);
void NormalizeWord(char *word);
void indexSave(char *filePath, hashtable_t *hash);
static void hcount(void *arg, const char *key, void *item);
static void countercount(void *arg, const int key, const int count);

int main(int argc, char const *argv[]) {
    if (argc != 3) {    //validate number of arguments is correct
        fprintf(stderr, "%s\n", "Needs exactly 2 arguments (and the command)");
        exit(1);
    }

    char dirName[100];
    char *check = ".crawled";
    if (argv[1] != NULL) { // check directory exists
        strcpy(dirName, argv[1]);
    } else {
        fprintf(stderr, "%s\n", "Directory does not exist");
        exit(2);
    }

    //checks if directory exists and is produced by the crawler
    if (argv[1] == NULL || strstr(dirName, check) == NULL) {
        fprintf(stderr, "%s\n", "Directory does not exist or is not produced by crawler");
        exit(2);
    }

    char filePath[100];
    strcpy(filePath, argv[2]);
    //checks for the file and if it exists, makes sure it is writable
    if (argv[2] != NULL) {
        if (access(filePath, W_OK) != 0) {
        } else {
            fprintf(stderr, "%s\n", "File is unwrittable");
            exit(3);
        }
    }

    // create hash to store info before writing to a new file
    hashtable_t *hash = hashtable_new(400);
    // index the info from crawler
    index(dirName, filePath, hash);
    return 0;
}


void index(char *dirName, char *filePath, hashtable_t *hash) {
    //vars for file scan
    FILE *file;
    char *url;
    char *depth;
    int realDepth;
    char line[256];
    int tracker = 0;
    //make path to each file
    int id = 1; //file number
    // create valid name using str functions
    char num[10];
    sprintf(num, "%d", id);
    char path[100];
    strcpy(path, dirName);
    strcat(path, "/");
    strcat(path, num);

    while(access(path, F_OK) == 0){   //index the files
        //reset variables to fix pointer errors
        char html[1000];
        strcpy(html, "");
        tracker = 0;
        file = fopen(path, "r");

        //recreate webpage line by line, eventually cat the lines from html section
        while (fgets(line, sizeof(line), file)) {
            tracker++;
            if (tracker == 1) {
                url = line;
            }
            if (tracker == 2) {
                depth = line;
            }
            if (tracker >= 3) {
                strcat(html, line);
            }
        }
        // get the depth of the file
        realDepth = atoi(depth);
        webpage_t *webpage = webpage_new(url, realDepth, html);
        // puts words into the hashtable and counter for each word for each file
        parseWords(webpage, id, hash);
        fclose(file);

        id++;    //change to next file and update vars
        sprintf(num, "%d", id);
        char nextpath[100];
        strcpy(nextpath, dirName);
        strcat(nextpath, "/");
        strcat(nextpath, num);
        strcpy(path, nextpath);
    }
    // create the output file
    indexSave(filePath, hash);
}

// sort words into the hash
void parseWords(webpage_t *page, int id, hashtable_t *hash) {
    int pos = 0;
    char *result;
    while ((result = webpage_getNextWord(page, &pos)) != NULL) {
        NormalizeWord(result);
        if (strlen(result) >= 3) {    //check word size
            if(hashtable_find(hash, result) == NULL){   //check if word is hash, if not insert, then increment either way
                hashtable_insert(hash, result, counters_new());   //change counter
            }
            counters_add(hashtable_find(hash, result), id);
        }
        free(result);
    }
}

// write output to a file from iterating the hashtable
void indexSave(char *filePath, hashtable_t *hash) {
    FILE *file;
    file = fopen(filePath, "w");
    hashtable_iterate(hash, file, hcount);
    fclose(file);
}

// iterate the word in the hashtable from crawler data
static void hcount(void *arg, const char *key, void *item) {
    counters_iterate(item, arg, countercount);
}

// iterate the page ids in the hashtable from crawler data
static void countercount(void *arg, const int key, const int count) {
    int *nitems = arg;
}
