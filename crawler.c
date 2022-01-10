/*
Crawler program follows a starting URL and goes to a given maximum depth of links from the page
Saves pages visited into a separate file with a unique ID and all of the html
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "hashtable.h"
#include "bag.h"
#include <dirent.h>
#include "webpage.h"
#include <unistd.h>
#include "pagedir.h"

void namedelete(void *item);
void pagefetcher(webpage_t *page);
void pagescanner(webpage_t *page, bag_t *bag, hashtable_t *hash);
void pagesaver(char *urlName, int id, int depth, char *html, char *pageDir);
void crawler(int maxDepth, hashtable_t *hash, bag_t *bag, char *pageDir);


int
main(const int argc, char *argv[]) {
    if (argc != 4) {    ///check correct # of varaibles
        fprintf(stderr, "%s\n", "Wrong number of commands. Needs exactly 3 + the command.");
        return 1;
    }
    char temp[100];
    char *ptr;
    strcpy(temp, argv[3]);
    const int maxDepth = strtod(temp, &ptr);
    if (argv[1] == NULL) {
        fprintf(stderr, "%s\n", "Error with URL");
        return 2;
    // check directory is valid and exists
    } else if (argv[2] == NULL) {
        fprintf(stderr, "%s\n", "Error with pageDirectory");
        return 3;
    } else if (argv[3] == NULL || maxDepth > 10 || maxDepth < 0) {   //check depth is valid
        fprintf(stderr, "%s\n", "Error with depth");
        return 4;
    }

        // rename variables
        char *seedURL = argv[1];
        char *pageDirectory = argv[2];
        webpage_t *webpage = webpage_new(seedURL, 0, NULL);

        if (!IsInternalURL(webpage_getURL(webpage))) { // check if internal URL
                fprintf(stderr, "%s\n", "Noninternal URL");
                return 2;
        }
        // create a bag to store webpages and a hashtable to store the data
        bag_t *bag = bag_new();
        hashtable_t *hash = hashtable_new(maxDepth*3);
        // insert the starting webpage
        bag_insert(bag, webpage);
        hashtable_insert(hash, webpage_getURL(webpage), "");
        // crawl from the given webpage
        crawler(maxDepth, hash, bag, pageDirectory);
        // clear from memory
        hashtable_delete(hash, NULL);
        bag_delete(bag, webpage_delete);
        return 0;
}


void
crawler(int maxDepth, hashtable_t *hash, bag_t *bag, char *pageDir) {
    int idTrack = 1;
    webpage_t *currPage;
    newPageDir(pageDir);
    while ((currPage = bag_extract(bag)) != NULL) {   // while more pages to crawl
        sleep(1);
        pagefetcher(currPage);     // retrieve HTML for page
        pagesaver(webpage_getURL(currPage), idTrack, webpage_getDepth(currPage), webpage_getHTML(currPage), pageDir);         // save page to pageDirectory with unique id
        if (webpage_getDepth(currPage) < maxDepth) {    // makes sure depth is correct
            pagescanner(currPage, bag, hash);        // find all URLs from the page
        }
        idTrack++;    // count up for next page being saved
    }
}


//get html and return it
void
pagefetcher(webpage_t *page){
    if (page != NULL && webpage_getHTML(page) == NULL) {
        webpage_fetch(page);
    }
}

// used to retrieve all the URLs from the webpage given from the HTML
void
pagescanner(webpage_t *page, bag_t *bag, hashtable_t *hash) {
    int pos = 0;
    char *result;
    while ((result = webpage_getNextURL(page, &pos)) != NULL) {   // check there are URLs
        webpage_t *webpage = webpage_new(result, webpage_getDepth(page) + 1, NULL); // create a new webpage data type
        if (NormalizeURL(webpage_getURL(webpage))) {  // make text lowercase and "normal"
            if (IsInternalURL(webpage_getURL(webpage))) { // check the URL is in the dartmouth servers
                if (hashtable_insert(hash, webpage_getURL(webpage), "")) {    // if data input is successful, add to the bag to crawl
                    bag_insert(bag, webpage);
                }
            }
        }
    }
}

// call helper function retrive the correct URL info (given from the professor)
void
pagesaver(char *urlName, int id, int depth, char *html, char *pageDir) {
    pageSave(id, urlName, depth, html, pageDir);
}

// clear memory when done with item
void namedelete(void *item) {
    if (item != NULL) {
      free(item);
    }
}
