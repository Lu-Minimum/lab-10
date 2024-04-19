#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXLEN 200

//These define statements are to convert the previous into strings
#define STR_(X) #X
#define STR(X) STR_(X)

// Trie structure
struct Trie
{
    int count;
    struct Trie* next[26];
};

// deallocate the trie structure
struct Trie *deallocateTrie(struct Trie *pTrie)
{
    //Check for null
    if (pTrie==NULL) {
        return NULL;
    }
    
    //Deallocate child nodes
    for (int i = 0; i<26; i++) {
        if (pTrie->next[i] != NULL) {
            pTrie->next[i] = deallocateTrie(pTrie->next[i]);
            if (pTrie->next[i] != NULL) {
                //This can't run unless someone changes this function
                printf("Couldn't deallocate\n");
                return pTrie;
            }
        }
    }

    return NULL;
}

// Initializes a trie structure
//And allocates
struct Trie *createTrie()
{
    //Allocate
    struct Trie* newTrie = (struct Trie*)malloc(sizeof(struct Trie));
    if (newTrie == NULL) {
        printf("Couldn't allocate memory\n");
        return NULL;
    }

    //Initialize to zero and null pointers
    newTrie->count = 0;
    for (int i = 0; i<26; i++) {
        newTrie->next[i] = NULL;
    }

    return newTrie;
}

//Called by insert function
void rInsert(struct Trie *pTrie, char *word, int index, int maxDepth) {
    //Check for null or negative numbers
    if (pTrie==NULL || word==NULL || index<0 || maxDepth<0) {
        return;
    }

    //Increment current node
    if (index>=maxDepth) {
        pTrie->count++;
        return;
    }

    //Get character difference
    short cDif = word[index]-'a';
    if (cDif<0||cDif>25) {
        return;
    }

    //Create node if it doesn't exist
    if (pTrie->next[cDif]==NULL) {
        pTrie->next[cDif] = createTrie();
    }

    //Recursively go to next node
    rInsert(pTrie->next[cDif], word, index+1, maxDepth);
}

// Inserts the word to the trie structure
//Calls recursive insert
void insert(struct Trie *pTrie, char *word)
{
    //Check for null
    if (pTrie == NULL || word == NULL) {
        return;
    }

    //Get length of word
    short n = (short)strnlen(word, MAXLEN+1);
    if (n>MAXLEN || n<0) {
        return;
    }

    //call recursive insert
    rInsert(pTrie, word, 0, n);
}

// computes the number of occurances of the word
int numberOfOccurances(struct Trie *pTrie, char *word)
{
    //Check for null
    if (pTrie==NULL||word==NULL) {
        return 0;
    }

    //Get length of word
    short n = (short)strnlen(word, MAXLEN+1);
    if (n>MAXLEN || n<0) {
        return 0;
    }

    //Define variables
    struct Trie* curr = pTrie;
    short cDif;

    //Loop to find correct node
    for (int i = 0; i<n; i++) {
        cDif = word[i]-'a';
        if (cDif<0||cDif>25||curr->next[cDif]==NULL) {
            return 0;
        }
        curr = curr->next[cDif];
    }
    return curr->count;
}

// this function will return number of words in the dictionary,
// and read all the words in the dictionary to the structure words
//Assumes destination array has enough space for all words
int readDictionary(char *filename, char **pInWords)
{
    //Check for null
    if (filename==NULL||pInWords==NULL) {
        return 0;
    }

    //Open the input file if it exists
    FILE* fIn = fopen(filename,"r");
    if (fIn==NULL) {
        printf("Dictionary file couldn't be found.\n");
        return 0;
    }

    //Get number of words to read
    int numLines;
    fscanf(fIn, "%d%*c", &numLines);
    if (numLines<=0) {
        return 0;
    }

    //Create string to hold words
    char inputWord[MAXLEN+1];
    for (int i = 0; i<MAXLEN+1; i++) {
        inputWord[i] = '\0';
    }

    //Declare variables
    short wordLen;
    //Counts how many words were actually assigned
    int countWords = 0;

    for (int i = 0; i<numLines; i++) {
        //Scan line and check that it scanned properly
        int temp = fscanf(fIn, "%" STR(MAXLEN) "[^\n ]%*c", inputWord);
        if (temp < 0) {
            printf("Failed to read input from file\n");
            break;
        }
        if (temp == 0) {
            inputWord[0] = '\0';
            fscanf(fIn, "%*c");
        }

        //Confirm word length
        wordLen = strnlen(inputWord, MAXLEN);
        inputWord[wordLen] = '\0';
        
        //Allocate memory in array
        pInWords[i] = (char*)malloc((wordLen+1)*sizeof(char));
        if (pInWords[i] == NULL) {
            printf("Couldn't allocate memory\n");
            break;
        }

        //Copy to array
        memcpy(pInWords[i], inputWord, wordLen*sizeof(char));
        pInWords[i][wordLen] = '\0';
        countWords++;
    }

    //Close file and return
    fclose(fIn);
    return countWords;
}

int main(void)
{
	char *inWords[256];
	
	//read the number of the words in the dictionary
	int numWords = readDictionary("dictionary.txt", inWords);
	for (int i=0;i<numWords;++i)
	{
		printf("%s\n",inWords[i]);
	}
	
	struct Trie *pTrie = createTrie();
	for (int i=0;i<numWords;i++)
	{
		insert(pTrie, inWords[i]);
	}
    //free inWords
    for (int i=0; i<numWords; i++) {
        free(inWords[i]);
    }
	// parse line by line, and insert each word to the trie data structure
	char *pWords[] = {"notaword", "ucf", "no", "note", "corg"};
	for (int i=0;i<5;i++)
	{
		printf("\t%s : %d\n", pWords[i], numberOfOccurances(pTrie, pWords[i]));
	}
	pTrie = deallocateTrie(pTrie);
	if (pTrie != NULL)
		printf("There is an error in this program\n");
	return 0;
}