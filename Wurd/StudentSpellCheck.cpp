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
	// implement an inorder traversal i think to delete everything			!!!
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
					int res = ch - 'a';
					if (ptr->variation[res] == nullptr)
					{
						ptr->variation[res] = new trieNode;
						ptr = ptr->variation[res];
					}
					else
						continue;
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
	return false; // TODO
}

void StudentSpellCheck::spellCheckLine(const std::string& line, std::vector<SpellCheck::Position>& problems) {
	// TODO
}
