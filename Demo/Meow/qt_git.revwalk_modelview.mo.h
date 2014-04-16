#pragma once

#include "git.revwalk.h"

namespace Nena
{
	namespace Qt
	{
		struct Revwalk : public QAbstractTableModel
		{
			typedef Git::Revwalk::Commit::List::const_iterator CommitIterator;

			int rowCount(_In_opt_ QModelIndex const &parent = QModelIndex()) const;
			int columnCount(_In_opt_ QModelIndex const &parent = QModelIndex()) const;
			QVariant data(_In_ QModelIndex const &index, _In_ int role) const;
			QVariant headerData(_In_ int section, _In_::Qt::Orientation orientation, _In_ int role) const;

			Revwalk(Git::Revwalk *host = NULL);
			Git::Revwalk *host;

		private:

			Q_OBJECT
		};
	}
}