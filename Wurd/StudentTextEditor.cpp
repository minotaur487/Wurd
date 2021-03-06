#include "StudentTextEditor.h"
#include "Undo.h"
#include <string>
#include <vector>

#include <list>
#include <fstream>
#include <iostream>
using namespace std;

TextEditor* createTextEditor(Undo* un)
{
	return new StudentTextEditor(un);
}

// O(1)
StudentTextEditor::StudentTextEditor(Undo* undo)
 : TextEditor(undo), m_curPos(Coord(0,0))
{
	m_curPosPtr = m_text.begin();
	m_totalLines = 1;
}

// O(N) time where N is the number of lines in the file currently being edited.
// I believe destructing an std linked list is O(N)
StudentTextEditor::~StudentTextEditor()
{
	// TODO
}

// loads but I can't tell if the carriage return character is removed							!!!
// I think getLines is supposed to result in the displace but I am not sure						!!!
// O(M+N) M is num of lines in editor currently and N is num of lines in new file being loaded
bool StudentTextEditor::load(std::string file) {
	ifstream infile(file);
	// Should go through whole document
	if (!infile)
	{
		cerr << "Bad state, file can't load" << endl;
		return false;
	}
	{
		// reset existing file, check if i have to do this if new
		reset();

		// load text ... should be O(N)
		string line;
		while (getline(infile, line))	// getline returns infile and has a deliminator of \n
		{
			int size = line.size();
			if (size != 0 && line[size - 2] == '\r')	// Avoid deleting because I have to iterate there and erasing reinitializes a vector?
			{
				line[size - 1] = '\0';	// See if I get this right			!!!
			}
			m_text.push_back(line);
			m_totalLines++;
		}

		// should be O(M)
		setFirstPos();
		setCurCol(0);
		setCurRow(0);
		return true;
	}
}

// O(M) M is num of lines in editor
bool StudentTextEditor::save(std::string file) {
	ofstream outfile(file);
	if (!outfile)
	{
		cerr << "New file can't be created" << endl;
		return false;
	}
	else
	{
		for (auto line : m_text)		// DUNNO IF I'M DOING THIS RIGHT?			!!!
		{
			// line[size - 1] = '\0';		// maybe this instead
			outfile << line << '\n';
		}
		return true;
	}
}

// O(N) N number of rows being edited
void StudentTextEditor::reset() {
	// O(N)
	while (!m_text.empty())
		m_text.pop_back();
	setFirstPos();
	setCurCol(0);
	setCurRow(0);
	getUndo()->clear();
}

void StudentTextEditor::move(Dir dir) {
	int cRow = getCurRow();
	int cCol = getCurCol();
	switch (dir)
	{
	case UP:
	{
		if (cRow == 0)
			return;
		incrementCurRow(-1);
		m_curPosPtr--;
		int cCol = getCurCol();
		executeUpDownEdgeCase(cCol);
		break;
	}
	case DOWN:
	{
		if (cRow == m_text.size())
			return;
		m_curPosPtr++;
		incrementCurRow(1);
		int cCol = getCurCol();
		executeUpDownEdgeCase(cCol);
		break;
	}
	case RIGHT:
	{
		if (cRow == getLastPositionOfText().m_row && cCol == getLastPositionOfText().m_col)
			return;
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
		if (cRow == 0 && cCol == 0)
			return;
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
		setCurCol(0);
		break;
	}
	case END:
	{
		setCurCol((*m_curPosPtr).size());
		break;
	}
	}
}

void StudentTextEditor::del() {
	int cCol = getCurCol();
	if (cCol == (*m_curPosPtr).size())
	{
		auto it = m_curPosPtr;
		it++;
		(*m_curPosPtr) += (*it);
		m_text.erase(it);
		m_totalLines--;
// PUSH INTO UNDO									!!!
	}
	else if (cCol == getLastPositionOfText().m_col && getCurRow() == getLastPositionOfText().m_row)
		return;
	else
		(*m_curPosPtr).erase(getCurCol(), 1);
// PUSH INTO UNDO									!!!
}

void StudentTextEditor::backspace() {
	// TODO
}

// O(L) where L is the length of the line of text containing the current editing position
void StudentTextEditor::insert(char ch) {
	if (ch == '\t')
	{
		string in = "    ";
		(*m_curPosPtr).insert(getCurCol(), in);
		incrementCurCol(4);
	}
	else
	{
		(*m_curPosPtr).insert(getCurCol(), 1, ch);
		incrementCurCol(1);
	}
// PUSH CH INTO UNDO									!!!
}

// O(L) where L is the length of the line of text.
// Command must not have a runtime that depends on the num of lines being edited
void StudentTextEditor::enter() {
	// If current editing pos is just past end of last line, add a new empty line
	if (getCurRow() == getLastPositionOfText().m_row && getCurCol() == getLastPositionOfText().m_col)
	{
		m_text.push_back("\n");
		m_totalLines++;
		m_curPosPtr++;
// PUSH \n INTO UNDO									!!!
	}
	else if (getCurCol() == 0)
	{
		m_text.insert(m_curPosPtr, "\n");
		m_totalLines++;
// PUSH \n INTO UNDO									!!!
	}
	else
	{
		// Split text
		int size = (*m_curPosPtr).size();		// O(1) I think
		string s1 = (*m_curPosPtr).substr(0, getCurCol());		// O(L)
		string s2 = (*m_curPosPtr).substr(getCurCol());		// O(L)

		// Split lines
		m_text.insert(m_curPosPtr, s1);
		(*m_curPosPtr) = s2;
// PUSH \n INTO UNDO									!!!
	}
	// shift down
	incrementCurRow(1);
	setCurCol(0);
}

void StudentTextEditor::getPos(int& row, int& col) const {
	row = getCurRow();
	col = getCurCol();
}

int StudentTextEditor::getLines(int startRow, int numRows, std::vector<std::string>& lines) const {
	int size = m_text.size();
	if (startRow < 0 || numRows < 0 || startRow > size)
		return -1;

	// clear vector lines values
	lines.clear();	// O(lines.size())

	if (startRow == size)
		return 0;

	// Get to startRow
	int curRow = getCurRow();
	int counter = 0;
	int diff = curRow - startRow;
	auto it = m_curPosPtr;
	// O(abs(curRow - startRow)
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

	// Add lines
	counter = 0;
	while (counter != numRows && it != m_text.end())	// O(numRows * L) where L is avg line length
	{
		lines.push_back(*it);
		it++;
		counter++;
	}
	return counter;
}

void StudentTextEditor::undo() {
	// TODO
}
