#ifndef CELL_H
#define CELL_H

#include <vector>

using namespace std;

class Cell
{
public:
	Cell();
	void insert(const int pointId);
	vector<int> getAllPoints();

private:
	vector<int> points;
};

Cell::Cell()
{
	points.clear();
}

void Cell::insert(const int pointID)
{
	points.push_back(pointID);
}

vector<int> Cell::getAllPoints()
{
	return points;
}

#endif