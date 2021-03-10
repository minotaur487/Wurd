#include "StudentUndo.h"
#include <stack>

Undo* createUndo()
{
	return new StudentUndo;
}

// O(1) avg case, may increase up to O(current line length where operation occurred) infrequently
void StudentUndo::submit(const Action action, int row, int col, char ch)
{
	change newChange = change(action, row, col, ch);	// what if user deletes a couple times, inserts, undos, then deletes?		!!!
	switch (action)
	{
	// need to implement accounting for deleting and backspacing at end of line			!!!
	case DELETE:
	{
		if (batchIfApplicable(newChange))		// O(L) where L is length of top.m_changes
			return;
		break;
	}
	case INSERT:
	{
		if (batchIfApplicable(newChange))	// O(L) where L is length of top.m_changes
			return;
		break;
	}
	default:
		break;
	}
	m_undoStack.push(newChange);	// O(1)
}

// O(1) avg case, may increase to O(L of deleted char that need to be restored) for restoration cases
StudentUndo::Action StudentUndo::get(int &row, int &col, int& count, std::string& text) 
{
	if (m_undoStack.empty())
		return ERROR;

	if (m_undoStack.top().m_action == INSERT)
		count = m_undoStack.top().m_changes.size();
	else
		count = 1;
	
	row = m_undoStack.top().m_row;
	if (m_undoStack.top().m_action == INSERT)		// row and column requirements sound contradictory ffs		!!!
	{
		col = m_undoStack.top().m_col - count;
	}
	else
	{
		col = m_undoStack.top().m_col;
	}

	if (m_undoStack.top().m_action == DELETE)
		text = m_undoStack.top().m_changes;	// O(L)
	else
		text = "";
	StudentUndo::Action operation = m_undoStack.top().m_action;

	// undo operation removed
	m_undoStack.pop();

	switch (operation)
	{
	case INSERT:
		return DELETE;
	case DELETE:
		return INSERT;
	case SPLIT:
		return JOIN;
	case JOIN:
		return SPLIT;
	default:
		return ERROR;
	}
}

void StudentUndo::clear() 
{
	while (!m_undoStack.empty())	// O(N)
	{
		m_undoStack.pop();
	}
}
