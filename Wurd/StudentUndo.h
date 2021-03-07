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
	void addToChanges(change& curBatch, char ch) { curBatch.m_changes += ch; }
	void batchIfApplicable(change& newChange, char ch);

		// functions that get
	int getChangeRow(const change& curChange) const { return curChange.m_row; }
	int getChangeCol(const change& curChange) const { return curChange.m_col; }
	Action getChangeAction(const change& curChange) const { return curChange.m_action; }
	std::string getChanges(const change& curChange) const { return curChange.m_changes; }

		// data members
	std::stack<change> m_undoStack;
};

inline
void StudentUndo::batchIfApplicable(StudentUndo::change& newChange, char ch)
{
	if (!m_undoStack.empty())
	{
		change top = m_undoStack.top();
		int colDiff = top.m_col - top.m_changes.size() - newChange.m_col;

		if (top == newChange)
		{
			addToChanges(top, ch);	// O(L) where L is length of top.m_changes
			return;
		}
		else if (newChange.m_action == INSERT && top.m_action == INSERT && colDiff == 0)
		{
			addToChanges(top, ch);	// O(L) where L is length of top.m_changes
			return;
		}
	}
}

#endif // STUDENTUNDO_H_
