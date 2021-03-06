#ifndef STUDENTTEXTEDITOR_H_
#define STUDENTTEXTEDITOR_H_

#include "TextEditor.h"
#include <list>
#include <string>

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

	// testing func //
	//remove when done//
std::list<std::string> getText() { return m_text; }
private:
	// (0,0) top left
	struct Coord
	{
		Coord(int col, int row) : m_row(row), m_col(col) {}
		int m_row;
		int m_col;
	};

		// Data Members
	//std::list<std::list<char>> m_text;
	//std::list<std::list<char>>::iterator m_curPosPtr;
	std::list<std::string> m_text;
	std::list<std::string>::iterator m_curPosPtr;
	Coord m_curPos;
	int m_totalLines;	// Is initialization 1 or 0 lines


		// Functions
	void setFirstPos() { m_curPosPtr = m_text.begin(); };
	void incrementCurRow(int spots) { m_curPos.m_row += spots; }
	void incrementCurCol(int spots) { m_curPos.m_col += spots; }
	void setCurRow(int pos) { m_curPos.m_col = pos; };
	void setCurCol(int pos) { m_curPos.m_col = pos; };
	int getCurRow() const { return m_curPos.m_row; };
	int getCurCol() const { return m_curPos.m_col; };
};

#endif // STUDENTTEXTEDITOR_H_
