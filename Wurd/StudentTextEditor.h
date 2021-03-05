#ifndef STUDENTTEXTEDITOR_H_
#define STUDENTTEXTEDITOR_H_

#include "TextEditor.h"
#include <list>

class Undo;

class StudentTextEditor : public TextEditor {
public:

	StudentTextEditor(Undo* undo);
	~StudentTextEditor();
	bool load(std::string file);
	bool save(std::string file);
	void reset();
	void move(Dir dir);
	void del();
	void backspace();
	void insert(char ch);
	void enter();
	void getPos(int& row, int& col) const;
	int getLines(int startRow, int numRows, std::vector<std::string>& lines) const;
	void undo();

private:
	struct Coord
	{
		int m_row;
		int m_col;
	};

		// Data Members
	std::list<std::list<char>> m_text;
	std::list<std::list<char>>::iterator m_curPos;
	int m_totalLines;	// Is initialization 1 or 0 lines

		// Functions
	std::list<std::list<char>>::iterator const getFirstPos();	// Might cause const issue
	int getCurRow() const;
	int getCurCol() const;

};

#endif // STUDENTTEXTEDITOR_H_
