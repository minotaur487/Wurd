#ifndef STUDENTUNDO_H_
#define STUDENTUNDO_H_

#include "Undo.h"

#include <stack>
#include <string>

class StudentUndo : public Undo {
public:

	void submit(Action action, int row, int col, char ch = 0);
	Action get(int& row, int& col, int& count, std::string& text);
	void clear();

private:
	struct change
	{
		change(Action action, int row, int col, char ch = 0)
			: m_row(row), m_col(col), m_action(action)
		{
			m_changes = std::string(1, ch);
		}

		bool operator==(const change& prevChange) const
		{
			if (m_col == prevChange.m_col && m_row == prevChange.m_row && m_action == prevChange.m_action)
				return true;
			return false;
		}
		
			// data members
		int m_row, m_col;
		std::string m_changes;
		Action m_action;
	};

		// helper functions
	void addToChanges(change& curBatch, std::string& in) { curBatch.m_changes += in; }
	bool batchIfApplicable(change& newChange);

		// data member
	std::stack<change> m_undoStack;
};

inline
bool StudentUndo::batchIfApplicable(StudentUndo::change& newChange)
{
	if (!m_undoStack.empty())
	{
		change top = m_undoStack.top();
		int colInsertDiff = newChange.m_col - top.m_col;
		int colBackspaceDiff = top.m_col - newChange.m_col;

		bool backspaceBool = top.m_action == DELETE && newChange.m_action == DELETE && colBackspaceDiff == 1;
		bool insertBool = newChange.m_action == INSERT && top.m_action == INSERT && colInsertDiff == 1;

		// if the most recent operation is consecutive backspace or a consecutive insert
		if (insertBool || backspaceBool)
		{
			// merge new and previous operations
			addToChanges(newChange, m_undoStack.top().m_changes);
			m_undoStack.pop();
			m_undoStack.push(newChange);
			return true;
		}
		// if the most recent operation is delete and the new operation is a consecutive delete
		else if (top == newChange)
		{
			addToChanges(m_undoStack.top(), newChange.m_changes);
			return true;
		}
	}
	return false;
}

#endif // STUDENTUNDO_H_
