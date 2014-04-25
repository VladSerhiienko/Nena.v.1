#include "app.precompiled.h"
#include "qt_git.revwalk_modelview.mo.h"

#define _this_size std::distance(host->cbegin(), host->cend())

Nena::Qt::Revwalk::Revwalk(Git::Revwalk *host)
	: host(host)
{
}

int Nena::Qt::Revwalk::rowCount(QModelIndex const &parent) const
{
	if (host) return _this_size;
}

int Nena::Qt::Revwalk::columnCount(QModelIndex const &parent) const
{
	return 2;
}

QVariant Nena::Qt::Revwalk::data(QModelIndex const &index, int role) const
{
	CommitIterator i;

	if (role == ::Qt::DisplayRole)
	if (host && (_this_size) > index.column())
	switch (index.column())
	{
	case 0: i = std::next(host->cbegin(), index.row());
		return (*i)->ShortTime.c_str();
	case 1: i = std::next(host->cbegin(), index.row());
		return (*i)->Message.c_str();
	default: return "#";
	}

	return QVariant();
}

QVariant Nena::Qt::Revwalk::headerData(int section, ::Qt::Orientation orientation, int role) const
{
	if (role == ::Qt::DisplayRole)
	{
		if (orientation == ::Qt::Vertical) return _this_size - section;
		else if (orientation == ::Qt::Horizontal) switch (section)
		{
		case 0: return "Time";
		case 1: return "Message";
		}
	}

	return QVariant();
}
