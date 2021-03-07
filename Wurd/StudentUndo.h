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

		// functions that do
	void addToChanges(change& curBatch, std::string& in) { curBatch.m_changes += in; }
	bool batchIfApplicable(change& newChange);

		// functions that get
	int getChangeRow(const change& curChange) const { return curChange.m_row; }
	int getChangeCol(const change& curChange) const { return curChange.m_col; }
	Action getChangeAction(const change& curChange) const { return curChange.m_action; }
	std::string getChanges(const change& curChange) const { return curChange.m_changes; }

		// data members
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

		if (top.m_action == DELETE && newChange.m_action == DELETE && colBackspaceDiff == 1)
		{
			addToChanges(newChange, m_undoStack.top().m_changes);
			m_undoStack.pop();
			m_undoStack.push(newChange);
			return true;
		}
		else if (top == newChange)
		{
			addToChanges(m_undoStack.top(), newChange.m_changes);
			return true;
		}
		else if (newChange.m_action == INSERT && top.m_action == INSERT && colInsertDiff == 1)
		{
			addToChanges(newChange, m_undoStack.top().m_changes);
			m_undoStack.pop();
			m_undoStack.push(newChange);
			return true;
		}
	}
	return false;
}

#endif // STUDENTUNDO_H_
