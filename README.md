# Troy Burkhart
## CS50 Winter 2021, Tiny Search Engine

##Implementation
I implemented a crawler, indexer, and querier.

Crawler output a text file containing each webpage's URL, HTML, and given unique ID.

Indexer took the crawler input and created another file containing every word over three characters and what files they appeared in.

Querier allows a user to enter a search term or phrase with "and" or "or" in between each search term. It prints in the terminal a list of website URLs from
the website that best matched the search input to the worst.
Matching is determined by counting the times the word appeared in a file and if the word was with an "and" then the score for the doc is the maximum of the tracked score and current word score for the document, if it ia an "or" key word, the scores are added together.

Querier was able to run crawler and indexer when the user input a search command. 
I just chose to breakdown the problem by first creating crawler, then indexer, and finally querier because this proccess was 
the overview we were explained in class about the basics of how bigger search engines function.
