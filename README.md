# Zen Word Game

A C implementation of the popular Zen Word puzzle game where players form words using a set of randomly generated letters. Features dynamic dictionary loading, intelligent word validation, and an interactive command-line interface.

## Overview

Players are given 7 unique letters (with at least 2 vowels) arranged in a diamond pattern and must find all possible words that can be formed using these letters. Each letter can only be used once per word, and words must be at least 3 characters long.

## Key Features

### Dynamic Dictionary Support
- Loads dictionaries from text files (supports large dictionaries up to 100,000 words)
- Compatible with multiple dictionary formats:
  - `Dictionary.txt`
  - `WordleDictionary.txt` 
  - `CollinsScrabbleWords(2019).txt`
- Progress indicators during dictionary loading
- Automatic uppercase conversion for consistency

### Intelligent Letter Generation
- Generates 7 unique letters with guaranteed minimum of 2 vowels
- Uses weighted letter pool favoring common English letters
- Ensures diverse and solvable puzzles
- Automatic regeneration if vowel constraint not met

### Advanced Word Validation
- Multi-stage validation process:
  1. Can letters form the word? (each letter used only once)
  2. Is word in dictionary?
  3. Meets minimum length requirement (3+ characters)
- Real-time feedback for invalid attempts
- Visual indicators for incorrect guesses (X marks)

### Interactive Game Interface
- Diamond-shaped letter display for visual appeal
- Organized word table by length (3-letter, 4-letter, etc.)
- Four-column layout for efficient space usage
- Progress tracking (found vs. total words)
- Support for multiple puzzle sessions

## Technical Implementation

### Memory Management
- Dynamic allocation for dictionary storage
- Efficient reallocation for word groupings
- Proper cleanup to prevent memory leaks
- Handles large dictionaries without performance loss

### Data Structures
- **WordGroup**: Groups words by length with progress tracking
- **Dynamic arrays**: Flexible storage for variable dictionary sizes
- **String processing**: Efficient character manipulation and comparison

### Algorithm Highlights
- **Letter constraint checking**: O(n×m) validation where n=word length, m=available letters
- **Dictionary search**: Linear search with early termination
- **Word grouping**: Automatic categorization by word length
- **Progress tracking**: Real-time game state management

## Compilation

```bash
gcc -Wall -g -std=c99 p5.c -o zen_word
```

## Usage

```bash
./zen_word <dictionary_file>
```

### Example Session
```bash
./zen_word Dictionary.txt

     E
   T   R
  A     I
   N   S

--- 3-letter words ---
A--       E--       N--       T--
R--       S--       

--- 4-letter words ---
A---      E---      N---      R---
S---      T---

Enter a word (or 'Q' to quit this puzzle): ARTS
Good find!

     E
   T   R  
  A     I
   N   S

--- 4-letter words ---
ARTS      E---      N---      R---
```

### Commands
- **Word input**: Type any word to attempt
- **Q**: Quit current puzzle and start new one
- **Y/N**: Continue or exit after completing puzzle

## Game Mechanics

### Letter Pool Weighting
Common letters appear more frequently in the generation pool:
- **E**: Most frequent (12 instances)
- **A, I**: Very common (9 instances each)
- **T, N, R**: Common (6 instances each)
- **Rare letters** (J, X, Q, Z): Single instances

### Word Discovery Process
1. **Input validation**: Check if word uses only available letters
2. **Dictionary lookup**: Verify word exists in loaded dictionary
3. **Length grouping**: Organize by word length for better visualization
4. **Progress tracking**: Monitor completion status

### Visual Feedback
- **Found words**: Display in full uppercase
- **Unfound words**: Show first letter + dashes (e.g., "A---")
- **Invalid attempts**: Mark with X's across entire word
- **Diamond layout**: Aesthetic letter arrangement

## Technical Specifications

### Performance Characteristics
- **Dictionary loading**: O(n) where n = number of words
- **Word validation**: O(k×7) where k = word length
- **Memory usage**: Linear with dictionary size
- **Search complexity**: O(m) where m = words of target length

### Constraints
- Maximum 100,000 dictionary words (configurable)
- Maximum 50 characters per word (configurable)
- Minimum 3-character words
- Exactly 7 letters per puzzle
- At least 2 vowels required

### Supported Input
- Case-insensitive word entry
- Automatic whitespace trimming
- Graceful handling of invalid input
- Support for various dictionary formats

## Algorithm Details

### Letter Generation Algorithm
```
1. Initialize empty letter set
2. While set size < 7:
   a. Select random letter from weighted pool
   b. Check for duplicates
   c. Add if unique
3. Count vowels in final set
4. If vowels < 2, restart process
```

### Word Formation Validation
```
1. Create copy of available letters
2. For each character in input word:
   a. Search for character in available set
   b. If found, mark as used
   c. If not found, return invalid
3. Return valid if all characters matched
```

## Author
Aidan Fernandes  
ECE 2220 - Spring 2025  
Clemson University

## Educational Objectives
This project demonstrates:
- Dynamic memory management in C
- String processing and manipulation
- File I/O operations
- Algorithm design and optimization
- Interactive program design
- Data structure implementation
- Memory leak prevention
