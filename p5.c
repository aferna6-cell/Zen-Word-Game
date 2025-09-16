/*
 * Author: Aidan Fernadnes
 * Clemson Login Name: Aferna6
 * Course Number: ECE 2220
 * Semester: Spring 2025
 * Assignment Number: Lab 5 – Strings – Zen Word
 *
 * Purpose:
 *  This program emulates part of the Zen Word game. It reads a dictionary file
 *  (e.g., Dictionary.txt, WordleDictionary.txt, or CollinsScrabbleWords(2019).txt),
 *  randomly selects seven letters (with at least two vowels), finds words that can
 *  be constructed using those letters (each used only once per word) from the dictionary,
 *  displays a “circle” of letters (now in a diamond-like layout) along with a table of
 *  word outlines, and then interacts with the user to fill in the words.
 *
 * Assumptions:
 *  - The dictionary file is provided as a command line argument.
 *  - Each word in the dictionary is on a separate line.
 *  - The user inputs are assumed to be valid words or 'Q'/'q' to quit.
 *
 * Known Bugs:
 *  - The word grouping is implemented for words up to a maximum length defined by MAX_WORD_LEN.
 *  - The “circle” (diamond) display and four-column table formatting is a basic implementation.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAX_WORDS      100000  /* Adjust if dictionary is huge */
#define MAX_WORD_LEN   50      /* Adjust if dictionary has longer words */
#define MIN_WORD_LEN   3       /* Minimum length of word to include in puzzle */
#define LETTER_COUNT   7       /* Number of letters to pick for the puzzle */

/*
 * A simple set of vowels (ensuring at least two vowels in the final 7 letters).
 */
static const char *VOWELS = "AEIOU";

/*
 * A simple weighting: more common letters appear more frequently in a pool
 * to bias random picks (feel free to tweak if you wish).
 */
static const char LETTER_POOL[] =
    "EEEEEEEEEEEEAAAAAAAAAAIIIIIIIIII"
    "OOOOOOOOOOOONNNNNN"
    "RRRRRRTTTTTTLLLLSSSS"
    "DDDDGGGBBCCMMPPFFHHVVWWYYKJXQZ";


/* ------------------------------------------------------------------------ *
 * Struct to group words by length, and track how many have been guessed.
 * ------------------------------------------------------------------------ */
typedef struct
{
    char  **wordList;   /* dynamic array of pointers to words of this length */
    int     count;      /* how many words of this length */
    int     found;      /* how many have been guessed so far */
} WordGroup;


/* ------------------------------------------------------------------------ *
 * Helper: display_progress_message
 *   Prints a dot every 'blockSize' items processed for a progress effect.
 * ------------------------------------------------------------------------ */
void display_progress_message(int index, int blockSize)
{
    if (index % blockSize == 0)
    {
        printf(".");
        fflush(stdout);
    }
}


/* ------------------------------------------------------------------------ *
 * Function: load_dictionary
 *   Reads all words from a file into a dynamically allocated array of char*.
 *   Returns number of words read, or -1 on error.
 * ------------------------------------------------------------------------ */
int load_dictionary(const char *filename, char ***dict)
{
    FILE *fp = fopen(filename, "r");
    if (!fp)
    {
        perror("Error opening dictionary file");
        return -1;
    }

    printf("Reading dictionary");
    fflush(stdout);

    char **tempDict = (char **)malloc(MAX_WORDS * sizeof(char *));
    if (!tempDict)
    {
        fclose(fp);
        fprintf(stderr, "\nFailed to allocate dictionary pointer array.\n");
        return -1;
    }

    int count = 0;
    char buffer[MAX_WORD_LEN];
    while (fgets(buffer, sizeof(buffer), fp) != NULL && count < MAX_WORDS)
    {
        /* Remove trailing newline if present */
        buffer[strcspn(buffer, "\r\n")] = '\0';

        /* Convert to uppercase for consistency */
        for (int i = 0; buffer[i]; i++)
        {
            buffer[i] = (char)toupper((unsigned char)buffer[i]);
        }

        /* Allocate space for the word and copy it in */
        tempDict[count] = (char *)malloc(strlen(buffer) + 1);
        if (!tempDict[count])
        {
            fprintf(stderr, "\nMemory allocation failed for dictionary word.\n");
            fclose(fp);
            return -1;
        }
        strcpy(tempDict[count], buffer);
        count++;

        display_progress_message(count, 50000); /* Print dot periodically */
    }

    fclose(fp);
    printf("\nFinished reading %d words from the dictionary.\n", count);

    *dict = tempDict;
    return count;
}


/* ------------------------------------------------------------------------ *
 * Function: free_dictionary
 *   Frees all strings in the dictionary array and the array itself.
 * ------------------------------------------------------------------------ */
void free_dictionary(char **dict, int dictSize)
{
    for (int i = 0; i < dictSize; i++)
    {
        free(dict[i]);
    }
    free(dict);
}


/* ------------------------------------------------------------------------ *
 * Function: generate_letters
 *   Generates 7 distinct letters (with at least 2 vowels).
 * ------------------------------------------------------------------------ */
void generate_letters(char letters[LETTER_COUNT])
{
    int foundVowels = 0;
    int distinctCount = 0;
    srand((unsigned)time(NULL));

    while (distinctCount < LETTER_COUNT)
    {
        int index = rand() % (int)strlen(LETTER_POOL);
        char c = LETTER_POOL[index];

        /* Check for duplicate */
        int isDuplicate = 0;
        for (int i = 0; i < distinctCount; i++)
        {
            if (letters[i] == c)
            {
                isDuplicate = 1;
                break;
            }
        }

        if (!isDuplicate)
        {
            letters[distinctCount++] = c;
            if (strchr(VOWELS, c) != NULL)
            {
                foundVowels++;
            }
        }

        /* If we have 7 letters but < 2 vowels, reset and start again. */
        if (distinctCount == LETTER_COUNT && foundVowels < 2)
        {
            distinctCount = 0;
            foundVowels = 0;
        }
    }
}


/* ------------------------------------------------------------------------ *
 * Function: can_form_word
 *   Determines if 'word' can be formed from the set of letters (each used once).
 * ------------------------------------------------------------------------ */
int can_form_word(const char *word, const char letters[LETTER_COUNT])
{
    /* We'll make a copy of letters so we can mark used ones. */
    char usedSet[LETTER_COUNT];
    for (int i = 0; i < LETTER_COUNT; i++)
    {
        usedSet[i] = letters[i];
    }

    for (int i = 0; word[i] != '\0'; i++)
    {
        char w = word[i];
        int found = 0;
        for (int j = 0; j < LETTER_COUNT; j++)
        {
            if (usedSet[j] == w)
            {
                /* mark used */
                usedSet[j] = '*';
                found = 1;
                break;
            }
        }
        if (!found) return 0; /* can't form word */
    }
    return 1;
}


/* ------------------------------------------------------------------------ *
 * Function: build_word_groups
 *   Allocates and initializes an array of WordGroup for lengths 0..maxLen.
 * ------------------------------------------------------------------------ */
WordGroup *build_word_groups(int maxLen)
{
    WordGroup *groups = (WordGroup *)calloc((size_t)(maxLen + 1), sizeof(WordGroup));
    if (!groups)
    {
        fprintf(stderr, "Failed to allocate WordGroup array.\n");
        return NULL;
    }
    return groups;
}


/* ------------------------------------------------------------------------ *
 * Function: add_word_to_group
 *   Adds 'word' to the group corresponding to its length.
 * ------------------------------------------------------------------------ */
void add_word_to_group(WordGroup *groups, const char *word)
{
    int len = (int)strlen(word);
    WordGroup *wg = &groups[len];

    wg->wordList = (char **)realloc(wg->wordList, (wg->count + 1) * sizeof(char *));
    wg->wordList[wg->count] = (char *)malloc(strlen(word) + 1);
    strcpy(wg->wordList[wg->count], word);
    wg->count++;
}


/* ------------------------------------------------------------------------ *
 * Function: free_word_groups
 *   Frees all allocated memory for the word groups.
 * ------------------------------------------------------------------------ */
void free_word_groups(WordGroup *groups, int maxLen)
{
    for (int i = 0; i <= maxLen; i++)
    {
        for (int j = 0; j < groups[i].count; j++)
        {
            free(groups[i].wordList[j]);
        }
        free(groups[i].wordList);
    }
    free(groups);
}


/* ------------------------------------------------------------------------ *
 * Function: create_valid_word_lists
 *   Goes through the dictionary and stores valid words (>=3 letters) into
 *   WordGroups by length, if they can be formed with the puzzle letters.
 * ------------------------------------------------------------------------ */
WordGroup *create_valid_word_lists(char **dictionary, int dictSize,
                                   const char letters[LETTER_COUNT], int *pMaxLen)
{
    printf("Creating puzzle word lists");
    fflush(stdout);

    int maxLen = 0;
    for (int i = 0; i < dictSize; i++)
    {
        int len = (int)strlen(dictionary[i]);
        if (len > maxLen)
        {
            maxLen = len;
        }
    }

    WordGroup *groups = build_word_groups(maxLen);
    if (!groups) return NULL;

    for (int i = 0; i < dictSize; i++)
    {
        int len = (int)strlen(dictionary[i]);
        if (len < MIN_WORD_LEN) 
            continue;

        if (can_form_word(dictionary[i], letters))
        {
            add_word_to_group(groups, dictionary[i]);
        }
        display_progress_message(i, 80000);
    }
    printf("\nDone creating valid word lists.\n");

    *pMaxLen = maxLen;
    return groups;
}


/* ------------------------------------------------------------------------ *
 * UPDATED FUNCTION: display_letter_circle
 *   Displays the 7 letters in a diamond-like layout for better visual symmetry.
 * ------------------------------------------------------------------------ */
void display_letter_circle(const char letters[LETTER_COUNT])
{
    /*
       Example for letters: [A,B,C,D,E,F,G]

             A
           B   C
          D     E
           F   G
    */
    printf("\n     %c\n", letters[0]);             // near top
    printf("   %c   %c\n", letters[1], letters[2]);
    printf("  %c     %c\n", letters[3], letters[4]);
    printf("   %c   %c\n", letters[5], letters[6]); 
    printf("\n");
}


/* ------------------------------------------------------------------------ *
 * Function: display_table_of_words
 *   Displays all groups from length=3 up to maxLen, each in four columns.
 *   - If a word is "found" (letters are lowercase), show it in uppercase.
 *   - If a word is not found, show first letter + dashes.
 *   - If 'badWord' is given and matches an unfound word, print Xs.
 * ------------------------------------------------------------------------ */
void display_table_of_words(WordGroup *groups, int maxLen, const char *badWord)
{
    const int COLS = 4;  /* number of columns per row */
    for (int length = MIN_WORD_LEN; length <= maxLen; length++)
    {
        WordGroup *wg = &groups[length];
        if (wg->count == 0)
            continue;

        /* Label each group: e.g. "--- 3-letter words ---" */
        printf("\n--- %d-letter words ---\n", length);

        int column = 0;
        for (int i = 0; i < wg->count; i++)
        {
            const char *w = wg->wordList[i];

            /* Check if w is "found" by scanning if any letter is lowercase. */
            int isFound = 0;
            for (int k = 0; w[k]; k++)
            {
                if (islower((unsigned char)w[k]))
                {
                    isFound = 1;
                    break;
                }
            }

            if (isFound)
            {
                /* Word guessed -> print fully in uppercase. */
                char temp[128];
                strcpy(temp, w);
                for (int k = 0; temp[k]; k++)
                {
                    temp[k] = (char)toupper((unsigned char)temp[k]);
                }
                /* %-10s keeps columns aligned */
                printf("%-10s", temp);
            }
            else
            {
                /* Word not guessed yet */
                if (badWord && strcmp(badWord, w) == 0)
                {
                    /* Print X's for each character in 'badWord' to indicate a wrong guess. */
                    int len = (int)strlen(w);
                    for (int k = 0; k < len; k++)
                        printf("X");
                    /* Fill out spaces up to 10 chars for alignment. */
                    for (int pad = len; pad < 10; pad++)
                        printf(" ");
                }
                else
                {
                    /* "first letter + dashes" for an unguessed word */
                    int len = (int)strlen(w);
                    printf("%c", w[0]);  /* first letter */
                    for (int k = 1; k < len; k++)
                        printf("-");
                    /* Fill out spaces up to 10 chars for alignment. */
                    for (int pad = len; pad < 10; pad++)
                        printf(" ");
                }
            }

            column++;
            if (column == COLS)
            {
                /* Move to next line after 4 columns. */
                printf("\n");
                column = 0;
            }
        }

        /* If we ended mid-line, start a new line. */
        if (column != 0)
            printf("\n");
    }
}


/* ------------------------------------------------------------------------ *
 * Function: check_and_fill_word
 *   Checks if 'inputWord' is in the group for its length. If found, mark it
 *   guessed by converting stored word to lowercase.
 *   Return 1 if found, 0 if not.
 * ------------------------------------------------------------------------ */
int check_and_fill_word(WordGroup *groups, int maxLen, const char *inputWord)
{
    int length = (int)strlen(inputWord);
    if (length < MIN_WORD_LEN || length > maxLen)
        return 0;

    WordGroup *wg = &groups[length];
    for (int i = 0; i < wg->count; i++)
    {
        if (strcmp(wg->wordList[i], inputWord) == 0)
        {
            /* Mark it as found by converting to lowercase in place */
            for (int c = 0; wg->wordList[i][c]; c++)
            {
                wg->wordList[i][c] = (char)tolower((unsigned char)wg->wordList[i][c]);
            }
            wg->found++;
            return 1;
        }
    }
    return 0;
}


/* ------------------------------------------------------------------------ *
 * Function: all_words_found
 *   Checks if all words in all groups have been guessed (found).
 * ------------------------------------------------------------------------ */
int all_words_found(WordGroup *groups, int maxLen)
{
    for (int length = MIN_WORD_LEN; length <= maxLen; length++)
    {
        WordGroup *wg = &groups[length];
        if (wg->count != wg->found)
            return 0;
    }
    return 1;
}


/* ------------------------------------------------------------------------ *
 * MAIN
 * ------------------------------------------------------------------------ */
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <dictionary_filename>\n", argv[0]);
        return 1;
    }

    /* 1) Load dictionary into a dynamic array of char* */
    char **dictionary = NULL;
    int dictSize = load_dictionary(argv[1], &dictionary);
    if (dictSize < 0)
    {
        fprintf(stderr, "Failed to load dictionary.\n");
        return 1;
    }

    /* Repeatedly create new puzzle until the user chooses to exit. */
    while (1)
    {
        /* 2) Generate 7 letters with constraints (≥ 2 vowels) */
        char letters[LETTER_COUNT];
        generate_letters(letters);

        /* 3) Create valid word lists from the dictionary */
        int maxLen = 0;
        WordGroup *groups = create_valid_word_lists(dictionary, dictSize, letters, &maxLen);
        if (!groups)
        {
            fprintf(stderr, "Failed to create word groups.\n");
            free_dictionary(dictionary, dictSize);
            return 1;
        }

        /* 4) Display the letters in a diamond-like "circle" */
        display_letter_circle(letters);

        /* 5) Display the table of words in an initial state (unfound) */
        display_table_of_words(groups, maxLen, NULL);

        /* 6) Now prompt for user input until all words found or 'Q' */
        while (1)
        {
            /* Check if puzzle is complete */
            if (all_words_found(groups, maxLen))
            {
                printf("\nCongratulations! You found all the words!\n");
                break;
            }

            printf("\nEnter a word (or 'Q' to quit this puzzle): ");
            char inputBuffer[128];
            if (!fgets(inputBuffer, sizeof(inputBuffer), stdin))
            {
                /* End of file or error */
                printf("Input error. Exiting puzzle.\n");
                break;
            }

            /* Remove trailing newline */
            inputBuffer[strcspn(inputBuffer, "\r\n")] = '\0';

            /* Convert to uppercase for dictionary matching */
            for (int i = 0; inputBuffer[i]; i++)
            {
                inputBuffer[i] = (char)toupper((unsigned char)inputBuffer[i]);
            }

            if (strcmp(inputBuffer, "Q") == 0)
            {
                /* Quit this puzzle (outer loop will start a new puzzle). */
                break;
            }

            /* 7) Check if word can be formed from letters and if it's in the valid set */
            if (!can_form_word(inputBuffer, letters))
            {
                printf("\nThat word cannot be formed from these letters!\n");
                display_letter_circle(letters);
                /* Pass inputBuffer as badWord to display X's in the table */
                display_table_of_words(groups, maxLen, inputBuffer);
            }
            else
            {
                /* If it is in the valid set, fill it in; otherwise display X's */
                if (check_and_fill_word(groups, maxLen, inputBuffer))
                {
                    printf("\nGood find!\n");
                    display_letter_circle(letters);
                    display_table_of_words(groups, maxLen, NULL);
                }
                else
                {
                    printf("\nThat word is not in the list of valid words.\n");
                    display_letter_circle(letters);
                    display_table_of_words(groups, maxLen, inputBuffer);
                }
            }
        }

        /* Free the puzzle's WordGroups before starting a new puzzle. */
        free_word_groups(groups, maxLen);

        /* Ask user if they want to keep playing or really quit */
        printf("\nPlay another puzzle? (Y/N): ");
        char againBuf[10];
        if (!fgets(againBuf, sizeof(againBuf), stdin))
        {
            /* Input error -> exit */
            break;
        }
        if (toupper((unsigned char)againBuf[0]) != 'Y')
        {
            /* User doesn't want another puzzle. Exit. */
            break;
        }
    }

    /* Finally, free the dictionary memory before exiting. */
    free_dictionary(dictionary, dictSize);

    return 0;
}
