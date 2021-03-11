#include "StudentTextEditor.h"
#include "Undo.h"
#include <string>
#include <vector>
#include <list>
#include <fstream>
#include <iostream>
using namespace std;


/*---------------------------------------------------------------
*		create StudentTextEditor object
---------------------------------------------------------------*/
TextEditor* createTextEditor(Undo* un)
{
	return new StudentTextEditor(un);
}


/*---------------------------------------------------------------
*		Constructs the student text editor
---------------------------------------------------------------*/
StudentTextEditor::StudentTextEditor(Undo* undo)
 : TextEditor(undo), m_curPos(Coord(0,0))
{
	m_text.push_back("");		// deals with typing in waiting screen
	m_curPosPtr = m_text.begin();
}


/*---------------------------------------------------------------
*		Destructs the student text editor
---------------------------------------------------------------*/
StudentTextEditor::~StudentTextEditor()
{
}


/*---------------------------------------------------------------
*		If possible, clears current student text editor's
*		contents and loads the specified text
---------------------------------------------------------------*/
bool StudentTextEditor::load(std::string file) {
	ifstream infile(file);
	if (!infile)
	{
		cerr << "Bad state, file can't load" << endl;
		return false;
	}
	else
	{
		// clears current text editor contents and resets editing position and undo stack
		reset();

		// load text
		string line;
		while (getline(infile, line))
		{
			// if there is a return carriage at the end, delete it
			if (!(line.empty()) && line.back() == '\r')
			{
				line.pop_back();
			}
			m_text.push_back(line);
		}

		setFirstPos();
		setCurCol(0);
		setCurRow(0);
		return true;
	}
}


/*---------------------------------------------------------------
*		Overwrites the contents of the specified file
*		with the lines currently in the student
*		text editor
---------------------------------------------------------------*/
bool StudentTextEditor::save(std::string file) {
	ofstream outfile(file);
	if (!outfile)
	{
		cerr << "New file can't be created" << endl;
		return false;
	}
	else
	{
		// output each line into file
		for (auto line : m_text)
		{
			outfile << line << '\n';
		}
		return true;
	}
}


/*---------------------------------------------------------------
*		Clears the contents of the student text editor,
*		resets the current editing position, and
*		tells the undo object to clear itself
---------------------------------------------------------------*/
void StudentTextEditor::reset() {
	// clear student text editor contents
	while (!m_text.empty())
		m_text.pop_back();

	setFirstPos();
	setCurCol(0);
	setCurRow(0);

	getUndo()->clear();
}


/*---------------------------------------------------------------
*		Adjusts editing position when arrow keys are pressed
---------------------------------------------------------------*/
void StudentTextEditor::move(Dir dir) {
	int cRow = getCurRow();
	int cCol = getCurCol();

	switch (dir)
	{
	case UP:
	{
		// cursor is at the top of the file
		if (cRow == 0)
			return;

		incrementCurRow(-1);
		m_curPosPtr--;
		cCol = getCurCol();
		executeUpDownEdgeCase(cCol);
		break;
	}
	case DOWN:
	{
		// cursor is at the bottom of the file
		if (cRow == m_text.size() - 1)
			return;

		m_curPosPtr++;
		incrementCurRow(1);
		cCol = getCurCol();
		executeUpDownEdgeCase(cCol);
		break;
	}
	case RIGHT:
	{
		// if cursor is at the last position in the editor
		if (cRow == getLastPositionOfText().m_row && cCol == getLastPositionOfText().m_col)
			return;

		// if cursor is at the end of a line
		if (cCol == (*m_curPosPtr).size())
		{
			m_curPosPtr++;
			incrementCurRow(1);
			setCurCol(0);
			return;
		}

		incrementCurCol(1);
		break;
	}
	case LEFT:
	{
		// if cursor is at the first position in the text editor
		if (cRow == 0 && cCol == 0)
			return;

		// if the cursor is at the beginning of a line
		if (cCol == 0)
		{
			m_curPosPtr--;
			incrementCurRow(-1);
			setCurCol((*m_curPosPtr).size());
			return;
		}

		incrementCurCol(-1);
		break;
	}
	case HOME: 
	{
		// set cursor to the beginning of the line
		setCurCol(0);
		break;
	}
	case END:
	{
		// set cursor to the end of line
		setCurCol((*m_curPosPtr).size());
		break;
	}
	}
}


/*---------------------------------------------------------------
*		Deletes the character at the current editing
*		position or merges lines when the delete button
*		is pressed.
---------------------------------------------------------------*/
void StudentTextEditor::del() {
	int cCol = getCurCol();
	// if at the last position, do nothing
	if (cCol == getLastPositionOfText().m_col && getCurRow() == getLastPositionOfText().m_row)
		return;
	// else if it is at the end of a line
	else if (cCol == (*m_curPosPtr).size())
	{
		auto it = m_curPosPtr;
		it++;
		// merge current line with the one below
		(*m_curPosPtr) += (*it);
		m_text.erase(it);
		getUndo()->submit(Undo::Action::JOIN, getCurRow(), cCol);
	}
	else
	{
		// delete character at current editing position
		getUndo()->submit(Undo::Action::DELETE, getCurRow(), cCol, (*m_curPosPtr)[cCol]);
		(*m_curPosPtr).erase(cCol, 1);
	}
}


/*---------------------------------------------------------------
*		Deletes the character before the current editing
*		position or merges lines when the backspace button
*		is pressed.
---------------------------------------------------------------*/
void StudentTextEditor::backspace() {
	int cCol = getCurCol();
	// if the current line is empty
	if (cCol == 0 || (*m_curPosPtr).empty())
	{
		if (getCurRow() == 0)
			return;

		// set editing position to end of previous line
		auto it = m_curPosPtr;
		m_curPosPtr--;
		setCurCol((*m_curPosPtr).size());
		incrementCurRow(-1);

		// merge lines
		(*m_curPosPtr) += (*it);
		m_text.erase(it);

		getUndo()->submit(Undo::Action::JOIN, getCurRow(), getCurCol());
	}
	else if (cCol > 0)
	{
		// else, delete character before current editing position
		getUndo()->submit(Undo::Action::DELETE, getCurRow(), cCol - 1, (*m_curPosPtr)[cCol - 1]);
		(*m_curPosPtr).erase(cCol - 1, 1);		// O(L)
		incrementCurCol(-1);
	}
}


/*---------------------------------------------------------------
*		Inserts a new character at the current editing
*		position.
---------------------------------------------------------------*/
void StudentTextEditor::insert(char ch) {
	if (ch == '\t')
	{
		// insert tab as four spaces
		string in = "    ";
		(*m_curPosPtr).insert(getCurCol(), in);
		incrementCurCol(4);

		getUndo()->submit(Undo::Action::INSERT, getCurRow(), getCurCol(), '\t');	// \t is represented by four spaces		!!!
	}
	else
	{
		// insert character at current editing position
		(*m_curPosPtr).insert(getCurCol(), 1, ch);
		incrementCurCol(1);

		getUndo()->submit(Undo::Action::INSERT, getCurRow(), getCurCol(), ch);
	}
}


/*---------------------------------------------------------------
*		Inserts a line break at the current editing position,
*		splitting the current line in two.
---------------------------------------------------------------*/
void StudentTextEditor::enter() {
	getUndo()->submit(Undo::Action::SPLIT, getCurRow(), getCurCol());

	// If current editing pos is just past end of last line, add a new empty line
	if (getCurRow() == getLastPositionOfText().m_row && getCurCol() == getLastPositionOfText().m_col)
	{
		m_text.push_back("");
		m_curPosPtr++;
	}
	else if (getCurCol() == 0)
	{
		// insert a new line before the current line if at the beginning of a line
		m_text.insert(m_curPosPtr, "");
	}
	else
	{
		// Split text
		string s1 = (*m_curPosPtr).substr(0, getCurCol());
		string s2 = (*m_curPosPtr).substr(getCurCol());

		// Split lines
		m_text.insert(m_curPosPtr, s1);
		(*m_curPosPtr) = s2;
	}

	// shift down
	incrementCurRow(1);
	setCurCol(0);
}


/*---------------------------------------------------------------
*		Sets its parameters to the row and column of the
*		current editing position.
---------------------------------------------------------------*/
void StudentTextEditor::getPos(int& row, int& col) const {
	row = getCurRow();
	col = getCurCol();
}


/*---------------------------------------------------------------
*		Gets the specified rows of lines in the text editor
*		and places them into the lines variable, clearing
*		any previous data in that variable before adding
---------------------------------------------------------------*/
int StudentTextEditor::getLines(int startRow, int numRows, std::vector<std::string>& lines) const {
	int size = m_text.size();
	// if parameters are invalid, return -1
	if (startRow < 0 || numRows < 0 || startRow > size)
		return -1;

	// clear lines
	lines.clear();

	// if start row is at the end, return 0
	if (startRow == size)
		return 0;

	// get to startRow
	int curRow = getCurRow();
	int counter = 0;
	int diff = curRow - startRow;
	auto it = m_curPosPtr;
	if (diff < 0)
	{
		while (counter != diff)
		{
			it++;
			counter--;
		}
	}
	else if (diff > 0)
	{
		while (counter != diff)
		{
			it--;
			counter++;
		}
	}

	// Add each line to lines up to numRows or til the end of the text
	counter = 0;
	while (counter != numRows && it != m_text.end())
	{
		lines.push_back(*it);
		it++;
		counter++;
	}
	return counter;
}


/*---------------------------------------------------------------
*		Undos the last operation the user performed and
*		repositions the cursor (gets the undo operation from
*		TextEditor's Undo member variable)
---------------------------------------------------------------*/
void StudentTextEditor::undo() {
	int count, row, col;
	string text;
	Undo::Action operation = getUndo()->get(row, col, count, text);

	// based on a given undo operation
	switch (operation)
	{
	case Undo::Action::INSERT:
		getToRow(row);
		setCurCol(col);
		(*m_curPosPtr).insert(getCurCol(), text);
		break;
	case Undo::Action::DELETE:
	{
		getToRow(row);
		setCurCol(col);
		(*m_curPosPtr).erase(col, count);
		break;
	}
	case Undo::Action::JOIN:
	{
		getToRow(row);
		setCurCol(col);

		// merge current line with line below
		auto it = m_curPosPtr;
		it++;
		(*m_curPosPtr) += (*it);
		m_text.erase(it);
		break;
	}
	case Undo::Action::SPLIT:
	{
		getToRow(row);
		setCurCol(col);

		// Split text
		string s1 = (*m_curPosPtr).substr(0, getCurCol());
		string s2 = (*m_curPosPtr).substr(getCurCol());

		// Split lines
		auto it = m_curPosPtr;
		it++;
		m_text.insert(it, s2);
		(*m_curPosPtr) = s1;
		break;
	}
	case Undo::Action::ERROR:
		break;
	}
}


/*---------------------------------------------------------------
*		Repositions the editing position to the specified row
---------------------------------------------------------------*/
void StudentTextEditor::getToRow(int row)
{
	while (getCurRow() != row)
	{
		if (row > getCurRow())
		{
			m_curPosPtr++;
			incrementCurRow(1);
		}
		else if (row < getCurRow())
		{
			m_curPosPtr--;
			incrementCurRow(-1);
		}
	}
}