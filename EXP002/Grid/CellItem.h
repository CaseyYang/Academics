#ifndef CELLITEM_H
#define CELLITEM_H

struct CellItem
{
	int cellID;
	double minDist;

	CellItem(const int ID, const double minDist)
	{
		this->cellID = ID;
		this->minDist = minDist;
	}

	CellItem(const CellItem& other)
	{
		this->cellID = other.cellID;
		this->minDist = other.minDist;
	}

	CellItem& operator=(const CellItem& other)
	{
		this->cellID = other.cellID;
		this->minDist = other.minDist;
		return *this;
	}

	friend bool operator<(const CellItem lhs, const CellItem rhs);
};

bool operator<(const CellItem lhs, const CellItem rhs)
{
	if (lhs.minDist > rhs.minDist)
	{
		return true;
	}

	return false;
}


#endif