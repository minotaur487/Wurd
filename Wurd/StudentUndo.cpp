#include "StudentUndo.h"
#include <stack>


/*---------------------------------------------------------------
*		create StudentUndo object
---------------------------------------------------------------*/
Undo* createUndo()
{
	return new StudentUndo;
}


/*---------------------------------------------------------------
*		Allows for information about a change/operation
*		to be saved
---------------------------------------------------------------*/
void StudentUndo::submit(const Action action, int row, int col, char ch)
{
	change newChange = change(action, row, col, ch);
	switch (action)
	{
	// if action was delete, try to batch it
	case DELETE:
	{
		if (batchIfApplicable(newChange))
			return;
		break;
	}
	// if action was insert, try to batch it
	case INSERT:
	{
		if (batchIfApplicable(newChange))
			return;
		break;
	}
	default:
		break;
	}
	// otherwise push change onto stack
	m_undoStack.push(newChange);
}


/*---------------------------------------------------------------
*		Gets information about the last change/operation
---------------------------------------------------------------*/
StudentUndo::Action StudentUndo::get(int &row, int &col, int& count, std::string& text) 
{
	// return error if empty
	if (m_undoStack.empty())
		return ERROR;

	// If action to undo was insert, return the number of characters to delete
	if (m_undoStack.top().m_action == INSERT)
		count = m_undoStack.top().m_changes.size();
	else
		count = 1;
	
	// Return proper editing positions
	row = m_undoStack.top().m_row;
	if (m_undoStack.top().m_action == INSERT)
	{
		col = m_undoStack.top().m_col - count;
	}
	else
	{
		col = m_undoStack.top().m_col;
	}

	// if the action to undo was delete, return the text deleted
	if (m_undoStack.top().m_action == DELETE)
		text = m_undoStack.top().m_changes;	// O(L)
	else
		text = "";
	StudentUndo::Action operation = m_undoStack.top().m_action;

	// operation removed
	m_undoStack.pop();

	// return proper undo operation
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


/*---------------------------------------------------------------
*		Clears out the entire undo stack
---------------------------------------------------------------*/
void StudentUndo::clear() 
{
	while (!m_undoStack.empty())
	{
		m_undoStack.pop();
	}
}
