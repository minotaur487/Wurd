#include "StudentSpellCheck.h"
#include <string>
#include <vector>

#include <fstream>
#include <iostream>
using namespace std;

SpellCheck* createSpellCheck()
{
	return new StudentSpellCheck;
}

StudentSpellCheck::~StudentSpellCheck() {
	cleanUp(root);		// not certain this cleans up properly			!!!
}

void StudentSpellCheck::cleanUp(trieNode* cur)
{
	if (cur == nullptr)
		return;

	for (int i = 0; i < 28; i++)
		cleanUp(cur->variation[i]);
	delete cur;
}

// O(N) N is num of lines in dict. Assume that upper bound of length of input line is constant
bool StudentSpellCheck::load(std::string dictionaryFile) {
	ifstream infile(dictionaryFile);
	// Should go through whole dictionary
	if (!infile)
	{
		cerr << "Bad state, dictionary can't load" << endl;
		return false;
	}
	{
		// load text ... should be O(N)
		string line;
		trieNode* ptr = root;
		while (getline(infile, line))	// O(N)
		{
			// goes through each word/line
			ptr = root;
			for (int i = 0; i < line.length(); i++)		// O(1)
			{
				// alphabet is contiguous, apostrophe is before lowercase
				if (isalpha(line[i]))
				{
					char ch = tolower(line[i]);
					int pos = ch - 'a';
					if (ptr->variation[pos] == nullptr)
					{
						ptr->variation[pos] = new trieNode;
						ptr = ptr->variation[pos];
					}
					else
					{
						ptr = ptr->variation[pos];
						continue;
					}
				}
				else if (line[i] == '\'')
				{
					ptr->variation[26] = new trieNode;
					ptr = ptr->variation[26];
				}
			}
			// indicate end of word
			ptr->variation[27] = new trieNode;
		}
		return true;
	}
}

bool StudentSpellCheck::spellCheck(std::string word, int max_suggestions, std::vector<std::string>& suggestions) {
	// go through each character in word
	if (isValidWord(word))		// O(L)
		return true;

	suggestions.clear();			// find out time complexity					!!!

	string transformedWord;
	for (auto x : word)
	{
		transformedWord += tolower(x);
	}
	string possibleSuggestion;
	for (int i = 0; i < transformedWord.length(); i++)		// O(L * 27 * L)
	{
		for (int j = 0; j < 27; j++)
		{
			if (suggestions.size() == max_suggestions)
				return false;

			possibleSuggestion = transformedWord;
			char letter = 'a' + j;
			possibleSuggestion[i] = letter;
			if (isValidWord(possibleSuggestion))
				suggestions.push_back(possibleSuggestion);
		}
	}
	return false;
}

// O(L)
bool StudentSpellCheck::isValidWord(const string& word) const
{
	if (word.length() <= 0)
		return false;

	trieNode* ptr = root;
	bool isValid = true;

	for (auto ch : word)
	{
		// check for apostrophe
		if (ch == '\'' && ptr->variation[26] != nullptr)
			ptr = ptr->variation[26];
		else if (ch == '\'')
		{
			isValid = false;
			break;
		}

		// check current letter
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
	if (isValid && ptr->variation[27] != nullptr)
		return true;
	return false;
}

void StudentSpellCheck::spellCheckLine(const std::string& line, std::vector<SpellCheck::Position>& problems) {
	problems.clear();

	string temp;
	int start, pos;
	start = pos = 0;
	for (auto ch : line)			// O(S)
	{
		if (!isalpha(ch) && ch != '\'')
		{
			if (temp.length() == 0)
			{
				pos++;
				start = pos;
				continue;
			}
			bool isValid = isValidWord(temp);
			if (!isValid)
			{
				SpellCheck::Position misspelledWord;
				misspelledWord.start = start;
				misspelledWord.end = pos - 1;
				problems.push_back(misspelledWord);
			}
			pos++;
			start = pos;
			temp.clear();
		}
		else
		{
			temp += ch;
			pos++;
		}
	}
	if (temp.length() != 0)
	{
		bool isValid = isValidWord(temp);
		if (!isValid)
		{
			SpellCheck::Position misspelledWord;
			misspelledWord.start = start;
			misspelledWord.end = pos;
			problems.push_back(misspelledWord);
			temp.clear();
		}
	}
}
