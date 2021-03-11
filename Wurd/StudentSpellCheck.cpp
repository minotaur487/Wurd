#include "StudentSpellCheck.h"
#include <string>
#include <vector>

#include <fstream>
#include <iostream>
using namespace std;


/*---------------------------------------------------------------
*		create StudentSpellCheck object
---------------------------------------------------------------*/
SpellCheck* createSpellCheck()
{
	return new StudentSpellCheck;
}


/*---------------------------------------------------------------
*		Properly destructs the StudentSpellCheck object,
*		like freeing the trie adt
---------------------------------------------------------------*/
StudentSpellCheck::~StudentSpellCheck() {
	cleanUp(root);
}


/*---------------------------------------------------------------
*		Frees the trie adt.
---------------------------------------------------------------*/
void StudentSpellCheck::cleanUp(trieNode*& cur)
{
	if (cur == nullptr)
		return;

	// for each letter in the alphabet, end of word marker, and apostrophe
	for (int i = 0; i < 28; i++)
		cleanUp(cur->variation[i]);
	delete cur;
	cur = nullptr;
}


/*---------------------------------------------------------------
*		Loads words from the specified dictionary data file
---------------------------------------------------------------*/
bool StudentSpellCheck::load(std::string dictionaryFile) {
	ifstream infile(dictionaryFile);
	if (!infile)
	{
		cerr << "Bad state, dictionary can't load" << endl;
		return false;
	}
	else
	{
		// clears previous dictionary
		cleanUp(root);
		root = new trieNode;

		string line;
		trieNode* ptr = root;
		// load dictionary line by line
		while (getline(infile, line))
		{
			ptr = root;
			// for each char in the current line
			for (int i = 0; i < line.length(); i++)
			{
				if (isalpha(line[i]))
				{
					char ch = tolower(line[i]);
					int pos = ch - 'a';
					// if there isn't a precedent for current letter
					if (ptr->variation[pos] == nullptr)
					{
						// create a new node for the current letter and move down trie
						ptr->variation[pos] = new trieNode;
						ptr = ptr->variation[pos];
					}
					else
					{
						// else move down trie
						ptr = ptr->variation[pos];
						continue;
					}
				}
				else if (line[i] == '\'')
				{
					// if there isn't a precedent for an apostrophe 
					if (ptr->variation[26] == nullptr)
					{
						// create a new node for it
						ptr->variation[26] = new trieNode;
					}
					// move down trie
					ptr = ptr->variation[26];
				}
			}
			// indicate end of word if this hasn't been done before
			if (ptr->variation[27] == nullptr)
				ptr->variation[27] = new trieNode;
		}
		return true;	// indicate dictionary is successfully loaded
	}
}


/*---------------------------------------------------------------
*		Determines whether a given word is in the dictionary,
*		and if not, returns 0 or more spelling suggestions
---------------------------------------------------------------*/
bool StudentSpellCheck::spellCheck(std::string word, int max_suggestions, std::vector<std::string>& suggestions) {
	// if the word is valid, return true
	if (isValidWord(word))
		return true;

	// clear any previous suggestions
	suggestions.clear();

	// transform each letter to lowercase
	string transformedWord;
	for (auto x : word)
	{
		transformedWord += tolower(x);
	}

	// for each character in the word
	char letter, oldLetter;
	for (int i = 0; i < transformedWord.length(); i++)
	{
		// swap the current letter with each letter in the alphabet and apostrophe
		for (int j = 0; j < 27; j++)
		{
			// if there are enough suggestions, indicate that the word passed in is not valid
			if (suggestions.size() == max_suggestions)
				return false;

			// get proper character
			if (j == 26)
				letter = '\'';
			else
				letter = 'a' + j;

			oldLetter = transformedWord[i];
			transformedWord[i] = letter;
			// save suggestion if valid
			if (isValidWord(transformedWord))
				suggestions.push_back(transformedWord);
			transformedWord[i] = oldLetter;
		}
	}
	return false;	// indicate that the word passed in is not valid
}


/*---------------------------------------------------------------
*		Determines whether a given word is in the dictionary
---------------------------------------------------------------*/
bool StudentSpellCheck::isValidWord(const string& word) const
{
	if (word.length() <= 0)
		return false;

	trieNode* ptr = root;
	bool isValid = true;

	// for each character
	for (auto ch : word)
	{
		// if character is a apostrophe, check if it's supposed to be there
		if (ch == '\'' && ptr->variation[26] != nullptr)
		{
			ptr = ptr->variation[26];
			continue;
		}
		else if (ch == '\'' && ptr->variation[26] == nullptr)
		{
			isValid = false;
			break;
		}

		// if character is a letter, check if it's supposed to be there
		char cur = tolower(ch);
		int pos = cur - 'a';
		if (ptr->variation[pos] != nullptr)
		{
			ptr = ptr->variation[pos];
		}
		else
		{
			isValid = false;
			break;
		}
	}
	// if the result was a word in the dictionary
	if (isValid && ptr->variation[27] != nullptr)
		return true;
	return false;
}


/*---------------------------------------------------------------
*		Determines the starting and ending positions of all
*		misspelled words in a given line of text
---------------------------------------------------------------*/
void StudentSpellCheck::spellCheckLine(const std::string& line, std::vector<SpellCheck::Position>& problems) {
	// clear previous problems
	problems.clear();

	int start, end;
	start = end = 0;
	int numOfWords = 0;
	vector<SpellCheck::Position> words;
	// for each character in the line
	for (auto ch : line)
	{
		// if the character is not a letter or apostrophe
		if ((!isalpha(ch) && ch != '\''))
		{
			if (end != start)
			{
				// save start and end position of word
				SpellCheck::Position potentialProblem;
				potentialProblem.start = start;
				potentialProblem.end = end;
				words.push_back(potentialProblem);
				numOfWords++;
			}
			end++;
			start = end;
			continue;
		}
		end++;
	}

	// if the last position equals the end of the line (case for single word lines)
	if (end == line.length())
	{
		// save start and end position of word
		SpellCheck::Position potentialProblem;
		potentialProblem.start = start;
		potentialProblem.end = end;
		words.push_back(potentialProblem);
		numOfWords++;
	}

	int counter = 0;
	string curWord;
	// for each word in the line
	while (counter < numOfWords)
	{
		// see if word is valid
		int length = words[counter].end - words[counter].start;
		curWord = line.substr(words[counter].start, length);
		bool isValid = isValidWord(curWord);

		// if it's not valid, push it into the problems container
		words[counter].end--;
		if (!isValid)
		{
			problems.push_back(words[counter]);
		}
		counter++;
	}
}
