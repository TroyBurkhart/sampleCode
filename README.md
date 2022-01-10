# Troy Burkhart
## CS50 Winter 2021, Tiny Search Engine

## Implementation
I implemented a crawler, indexer, and querier.

### Crawler
Crawler output a folder with text files containing each webpage's URL, HTML, and given unique ID.

This code utilized a bag to store all the URLs found.
when crawling each webpage and is iterated through until no webpages are left in the bag. The hashtable is used to track what pages have beem visited already to make sure the crawler does not go over the same page twice. Each webpage gets written to a separate file.

### Indexer
Indexer takes the crawler input and created another file containing every word over three characters and what files they appeared in.

This code recreates the webpage and builds a hashtable with the word as the key and the data as a counter that tracks the number of times the file ID contains that word. The code then iterates through the hashtable to output another text file as seen in the sample.

### Querier
Querier allows a user to enter a search term or phrase with "and" or "or" in between each search term. It prints in the terminal a list of website URLs from
the website that best matched the search input to the worst.
Matching is determined by counting the times the word appeared in a file and if the word was with an "and" then the score for the doc is the maximum of the tracked score and current word score for the document, if it ia an "or" key word, the scores are added together.

This code is a continuous loop that waits for a search term. When entered, the search prase is verified. Then the words are searched up by recreating the hashtable that the indexer file recorded. Scoring is determined as described above and the list is output into the terminal of best to worst match.


I chose to breakdown the problem by first creating crawler, then indexer, and finally querier because this proccess was 
the overview we were explained in class about the basics of how bigger search engines function.
