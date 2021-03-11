#ifndef STUDENTSPELLCHECK_H_
#define STUDENTSPELLCHECK_H_

#include "SpellCheck.h"

#include <string>
#include <vector>

class StudentSpellCheck : public SpellCheck {
public:
	StudentSpellCheck() { root = nullptr; }
	virtual ~StudentSpellCheck();
	bool load(std::string dict_file);
	bool spellCheck(std::string word, int maxSuggestions, std::vector<std::string>& suggestions);
	void spellCheckLine(const std::string& line, std::vector<Position>& problems);

private:
	struct trieNode
	{
		trieNode() : variation() {}
		trieNode* variation[28];
	};

		// data members
	trieNode* root;

		// helper functions
	bool isValidWord(const std::string& word) const;
	void cleanUp(trieNode*& root);
};

#endif  // STUDENTSPELLCHECK_H_
