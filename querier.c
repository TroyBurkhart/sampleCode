/*
Takes indexed and crawler outputs and search term which returns a list of the files
that contain the instance of the search terms from most to least.
Input a line like this:
hi world          //equivalent to hi and world//
hi or world
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ctype.h"
#include "hashtable.h"
#include "counters.h"


void readFile(char *file1, hashtable_t *hash, char *write);
void NormalizeWord(char *word);
int readQuery(char **words, char *line);
int verifyQuery(char **words, int wordIndex);
void searchFile(char **words, hashtable_t *hash, int maxid, char *pagedirec, int wordIndex);
int maxID(char *path);

int main(int argc, char const *argv[]) {
    if (argc != 3) {  //check # of vars
        fprintf(stderr, "%s\n", "Wrong number of arguments. Needs two plus the executable");
        return 1;
    }

    char dir[200];
    strcpy(dir, argv[1]);
    char *check = ".crawled";
    if (argv[1] == NULL) {  //check if nonnull pagedir
        fprintf(stderr, "%s\n", "Invalid pageDir");
        return 2;
    } else if (strstr(dir, check) == NULL) {
        fprintf(stderr, "%s\n", "The directory is not crawled");
        return 2;
    }

    FILE *file;
    char temp[200];
    strcpy(temp, argv[2]);
    file = fopen(temp, "r");
    if (file == NULL) {  //check if nonnull filename
        fprintf(stderr, "%s\n", "Invalid filename");
        return 3;
    }

    fclose(file);
    hashtable_t *hash = hashtable_new(1000);
    readFile(temp, hash, NULL);   //index into the hash from the index file from common dir
    int bigid = maxID(temp);
    //query loop
    while (true) {
        //allocate mem for word array
        char **words;
        words = malloc(1000*sizeof(char *));
        for (int i = 0; i < sizeof(words); i++) {
            words[i] = NULL;
        }

        char line[1000];
        int wordIndex;
        printf("%s\n", "enter a query. ! to quit");
        //get query
        wordIndex = readQuery(words, line);
        if (wordIndex == -1) {
            break;
        }

        //verify query is correct format
        int verify = verifyQuery(words, wordIndex);
        if (verify == 0) {
            //do the search
            searchFile(words, hash, bigid, dir, wordIndex);
        }
        else {
            return verify;
        }

        free(words);
  }
  return 0;
}

//make array of pointers of query separated by spaces
int readQuery(char **words, char *line) {
    scanf("%[^\n]%*c", line);
    int wordIndex = 0;
    if (strcmp(line, "!") == 0) {
        return(-1);
    }

    //check if break if user want to stop query
    NormalizeWord(line);
    int trackInWord = 0;
    int size = strlen(line);
    //go through every character in the string array
    for (int i = 0; i < size; i++) {
        //if character is not alpha and not space and not null
        if (!isalpha(line[i]) && !isspace(line[i]) && &line[i] != NULL) {
            fprintf(stderr, "%s\n", "Nonalpha input");
            return(-1);
        }

        //store pointer to word, enter if first letter
        if (isalpha(line[i]) && trackInWord == 0) {
            trackInWord = 1;
            words[wordIndex] = &line[i];
            wordIndex++;
        }

        //insert null into the space if after a letter
        if (isspace(line[i]) && trackInWord == 1) {
            line[i] = '\0';
            trackInWord = 0;
        }
    }
    return wordIndex;
}

//correct position of words and operators and not nulls in the search
int verifyQuery(char **words, int wordIndex) {
    int trackDoubleOps = 0;
    //if query starts with an operator
    if (strcmp(words[0], "and") == 0 || strcmp(words[0], "or") == 0) {
        fprintf(stderr, "%s\n", "query starts with operator");
        return(5);
    }

    //if query ends with an operator
    if (strcmp(words[wordIndex - 1], "and") == 0 || strcmp(words[wordIndex - 1], "or") == 0) {
        fprintf(stderr, "%s\n", "query ends with operator");
        return(6);
    }

    //check if two operators in a row
    for (int i = 0; i < wordIndex; i++) {
        if (strcmp(words[i], "and") == 0 || strcmp(words[i], "or") == 0) {
            trackDoubleOps++;
        } else {
            trackDoubleOps = 0;
        }
        if (trackDoubleOps == 2) {
            fprintf(stderr, "%s\n", "two adjacent operators");
            return(7);
        }
    }
    return(0);
}

//find word from input in hashtable
//iterate in counter found to get ID that has the word the most
//declare array score, store in slot
void searchFile(char **words, hashtable_t *hash, int maxid, char *pagedirec, int wordIndex) {
    int maxDocID = maxid;
    int foundDocs[maxid + 1];
    // reset pointers
    for (int i = 1; i <= maxDocID; i++) {
        foundDocs[i] = 0;
    }
    // iterate every doc
    for (int j = 1; j <= maxDocID; j++) {
        int score = -1;
        // check the search command (and/or/search term)
        char *track = "and";
        for(int i = 0; i < wordIndex; i++) {
            int currscore = 0;
            if (strcmp(words[i], "or") == 0) {
                track = "or";
            } else if (strcmp(words[i], "and") == 0){
                track = "and";
            } else {  // if the word is not a keyword then it finds the score
                currscore = counters_get(hashtable_find(hash, words[i]), j);
                if (score == -1) {  //no previous score, currscore is score
                    score = currscore;
                } else if (strcmp(track, "and") == 0 && currscore < score) {  // if and, highest score is kept
                    score = currscore;
                } else if (strcmp(track, "or") == 0 && currscore > 0){  // if or, and previous and current score together
                    score = score + currscore;
                }
                // default to "and" keyword
                track = "and";
            }
       }
       //record the score in array for docid
       foundDocs[j] = score;
    }


    //sort output from greatest to least by swapping scores in score array and the doc IDs in doc arrays
    int docKeys[maxid + 1];
    //initialize the array
    for (int i = 1; i <= maxDocID; i++) {
        docKeys[i] = i;
    }
    for (int i = 1; i <= maxDocID; i++) {
        for (int j = i; j <= maxDocID; j++) {
            if (foundDocs[i] < foundDocs[j]) {
                //swap scores
                int temp = foundDocs[j];
                foundDocs[j] = foundDocs[i];
                foundDocs[i] = temp;
                //swap docids
                int temp2 = docKeys[j];
                docKeys[j] = i;
                docKeys[i] = temp2;
            }
        }

    }
    //get URLS and print the information
    FILE *file;
    for (int j = 1; j <= maxDocID; j++) {
        char path[1000];
        char str[10];
        char line[1000];
        strcpy(path, pagedirec);
        sprintf(str, "%d", j);
        strcat(path, "/");
        strcat(path, str);
        file = fopen(path, "r");
        fscanf(file, "%s", line);
        printf("doc id %d = score of %d. URL = %s\n", docKeys[j], foundDocs[j], line);
        fclose(file);
    }
}

//find biggest DocID in index to stay in bounds when searching
int maxID(char *path) {
    int bigID = 0;
    FILE *file;
    file = fopen(path, "rt");
    int c;
    while ((c = fgetc(file)) != EOF) {
        if (isdigit(c)) {
            if (c > bigID) {
                bigID = c;
            }
        }
    }
    //convert from ascii (can improve, docs can only be single digit numbers because of this)
    bigID = bigID - 48;
    return bigID;
}
