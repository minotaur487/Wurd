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
	case Action::DELETE:
	{
		batchIfApplicable(newChange, ch);		// O(L) where L is length of top.m_changes
		return;
	}
	case Action::INSERT:
	{
		batchIfApplicable(newChange, ch);	// O(L) where L is length of top.m_changes
		return;
	}
	}
	m_undoStack.push(newChange);	// O(1)
}

// O(1) avg case, may increase to O(L of deleted char that need to be restored) for restoration cases
StudentUndo::Action StudentUndo::get(int &row, int &col, int& count, std::string& text) 
{
	if (m_undoStack.empty())
		return Action::ERROR;

	change top = m_undoStack.top();
	row = top.m_row;
	col = top.m_col;

	if (top.m_action == Action::INSERT)
		count = top.m_changes.size();
	else
		count = 1;			// for some reason, spec doesn't include batching of inserts?		!!!
	
	if (top.m_action == Action::DELETE)
		text = top.m_changes;	// O(L)
	else
		text = "";

	// undo operation removed
	m_undoStack.pop();

	switch (top.m_action)
	{
	case INSERT:
		return Action::DELETE;
	case DELETE:
		return Action::INSERT;
	case SPLIT:
		return Action::JOIN;
	case JOIN:
		return Action::SPLIT;
	}
}

void StudentUndo::clear() 
{
	while (!m_undoStack.empty())	// O(N)
	{
		m_undoStack.pop();
	}
}
