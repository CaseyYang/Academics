#ifndef GRID_H
#define GRID_H

#include <fstream>
#include <string>
#include <queue>
#include <vector>

#include "Node.h"
#include "config.h"
#include "Cell.h"
#include "CellItem.h"

using namespace std;

class Grid
{
public:
	Grid(const string nodeFile, const int tier);
	vector<int> range_query(const double x, const double y, const double radius);	//范围查询，返回点的ID

	vector<int> forTest(const int source, const double radius);

	
	double getX(const int nodeID)
	{
		return nodes[nodeID].x;
	}
	double getY(const int nodeID)
	{
		return nodes[nodeID].y;
	}
	

private:
	vector<Node> nodes;
	vector<Cell> cells;
	int tier;
	double length;	//网格边长

	double max_x, min_x;	//网格边界信息
	double max_y, min_y;

	int locate(const int nodeID);	//判断该点在哪个网格中，返回网格ID
	int locate(const double x, const double y);

	double getMinDist(const double x, const double y, const int cellID);	//点和网格的最小距离
	double getMaxDist(const double x, const double y, const int cellID);	//点和网格的最大距离
};

Grid::Grid(const string nodeFile, const int tier)
{
	/*
	*		读入点信息并记录边界坐标
	*/

	ifstream fin(nodeFile.c_str());
	
	int numOfNodes;
	fin >> numOfNodes;

	max_x = -1.0 * BIGVALUE;
	max_y = -1.0 * BIGVALUE;
	min_x = BIGVALUE;
	min_y = BIGVALUE;

	for (int i=0; i!=numOfNodes; ++i)
	{
		//读入并保存点坐标
		int no;
		double x, y;
		fin >> no;
		fin >> x;
		fin >> y;
		nodes.push_back(Node(x, y));

		//更新边界坐标
		if (x > max_x)
		{
			max_x = x;
		}
		else if (x < min_x)
		{
			min_x = x;
		}

		if (y > max_y)
		{
			max_y = y;
		}
		else if (y < min_y)
		{
			min_y = y;
		}
	}
	fin.close();

	/*
	*		划分网格
	*/

	this->tier = tier;

	//计算网格边长
	double extent_x = max_x - min_x;
	double extent_y = max_y - min_y;
	
	if (extent_x > extent_y)
	{
		length = extent_x / tier;
	}
	else
	{
		length = extent_y / tier;
	}
	length *= 1.0001;	//将网格变长适当扩大，使总网格边框的右和下边界上不会存在点
	
	//将每个点分配到各个网格中
	for (int i=0; i!=tier*tier; ++i)
	{
		cells.push_back(Cell());
	}

	for (int i=0; i!=nodes.size(); ++i)
	{
		int cellID = locate(i);
		cells[cellID].insert(i);
	}
}

vector<int> Grid::range_query(const double x, const double y, const double radius)
{
	vector<int> result;	//结果集
	
	//获取中心点所在网格，并计算向四个方向最多扩展的次数
	int centerCell = locate(x, y);
	int left = centerCell % tier;
	int right = tier - (centerCell % tier) - 1;
	int up = centerCell / tier;
	int down = tier - (centerCell / tier) - 1;

	priority_queue<CellItem> candidateCells;
	bool *valid = new bool[tier*tier]; //对应网格是否已入队列，已入为true
	for (int i=0; i!=tier*tier; ++i)
	{
		valid[i] = false;
	}
	
	candidateCells.push(CellItem(centerCell, 0.0));
	valid[centerCell] = true;
	int times = 1;	//扩散次数
	while (!candidateCells.empty())
	{
		CellItem nextCell = candidateCells.top();
		candidateCells.pop();

		if (nextCell.minDist > radius)	//网格完全在圆外面面
		{
			break;
		}

		double cellMaxDist = getMaxDist(x, y, nextCell.cellID);
		vector<int> nodesOfCell = cells[nextCell.cellID].getAllNodes();
		if (cellMaxDist<=radius)	//网格完全被圆包含
		{
			for (int i=0; i!=nodesOfCell.size(); ++i)
			{
				result.push_back(nodesOfCell[i]);
			}
		}
		else	//网格与圆部分相交
		{
			//检查网格每个点是否落于圆中
			for (int i=0; i!=nodesOfCell.size(); ++i)
			{
				int nextNode = nodesOfCell[i];
				if (getDistance(x, y, nodes[nextNode].x, nodes[nextNode].y) <= radius)
				{
					result.push_back(nextNode);
				}
			}
		}

		//扩散邻居网格
		if (candidateCells.empty())
		{
			bool upValid = false;
			bool downValid = false;
			bool leftValid = false;
			bool rightValid = false;

			if (times<=up)	//未上溢出
			{
				upValid = true;

				int upMidCell = centerCell - times * tier;
				if (!valid[upMidCell])
				{
					candidateCells.push(CellItem(upMidCell, getMinDist(x, y, upMidCell)));
				}
				for (int i=1; i<=getMin(times, left); ++i)
				{
					int nextCell = upMidCell - i;
					if (!valid[nextCell])
					{
						candidateCells.push(CellItem(nextCell, getMinDist(x, y, nextCell)));
					}
				}
				for (int i=1; i<=getMin(times, right); ++i)
				{
					int nextCell = upMidCell + i;
					if (!valid[nextCell])
					{
						candidateCells.push(CellItem(nextCell, getMinDist(x, y, nextCell)));
					}
				}
			}
			if (times<=down)	//未下溢出
			{
				downValid = true;

				int downMidCell = centerCell + times * tier;
				if (!valid[downMidCell])
				{
					candidateCells.push(CellItem(downMidCell, getMinDist(x, y, downMidCell)));
				}
				for (int i=1; i<=getMin(times, left); ++i)
				{
					int nextCell = downMidCell - i;
					if (!valid[nextCell])
					{
						candidateCells.push(CellItem(nextCell, getMinDist(x, y, nextCell)));
					}
				}
				for (int i=1; i<=getMin(times, right); ++i)
				{
					int nextCell = downMidCell + i;
					if (!valid[nextCell])
					{
						candidateCells.push(CellItem(nextCell, getMinDist(x, y, nextCell)));
					}
				}
			}
			if (times<=left)	//未左溢出
			{
				leftValid = true;

				int leftMidCell = centerCell - times;
				if (!valid[leftMidCell])
				{
					candidateCells.push(CellItem(leftMidCell, getMinDist(x, y, leftMidCell)));
				}
				for (int i=1; i<=getMin(times, up); ++i)
				{
					int nextCell = leftMidCell - tier * i;
					if (!valid[nextCell])
					{
						candidateCells.push(CellItem(nextCell, getMinDist(x, y, nextCell)));
					}
				}
				for (int i=1; i<=getMin(times, down); ++i)
				{
					int nextCell = leftMidCell + tier * i;
					if (!valid[nextCell])
					{
						candidateCells.push(CellItem(nextCell, getMinDist(x, y, nextCell)));
					}
				}
			}
			if (times<=right)	//未右溢出
			{
				rightValid = true;

				int rightMidCell = centerCell + times;
				if (!valid[rightMidCell])
				{
					candidateCells.push(CellItem(rightMidCell, getMinDist(x, y, rightMidCell)));
				}
				for (int i=1; i<=getMin(times, up); ++i)
				{
					int nextCell = rightMidCell - tier * i;
					if (!valid[nextCell])
					{
						candidateCells.push(CellItem(nextCell, getMinDist(x, y, nextCell)));
					}
				}
				for (int i=1; i<=getMin(times, down); ++i)
				{
					int nextCell = rightMidCell + tier * i;
					if (!valid[nextCell])
					{
						candidateCells.push(CellItem(nextCell, getMinDist(x, y, nextCell)));
					}
				}
			}


			times++;
		}
	}

	return result;
}

int Grid::locate(const int nodeID)
{
	double x = nodes[nodeID].x;
	double y = nodes[nodeID].y;

	int cellID = static_cast<int>((max_y-y)/length)*tier + static_cast<int>((x-min_x)/length);

	return cellID;
}

int Grid::locate(const double x, const double y)
{
	int cellID = static_cast<int>((max_y-y)/length)*tier + static_cast<int>((x-min_x)/length);

	return cellID;
}

double Grid::getMinDist(const double x, const double y, const int cellID)
{
	//获得网格左上角点坐标
	double cellMinX = min_x + (cellID % tier) * length;
	double cellMaxY = max_y - (cellID / tier) * length;

	//判断与网格相对位置

	if (x<cellMinX)	//网格左边
	{
		if (y > cellMaxY)	//左上
		{
			return getDistance(x, y, cellMinX, cellMaxY);
		}
		else if (y >= cellMaxY-length)	//正左
		{
			return getDistance(x, y, cellMinX, y);//cellMinX - x;
		}
		else	//左下
		{
			return getDistance(x, y, cellMinX, cellMaxY-length);
		}
	}
	else if (x<cellMinX+length)	//网格上中下
	{
		if (y > cellMaxY)	//正上
		{
			return y - cellMaxY;
		}
		else if (y >=cellMaxY - length)	//正中
		{
			return 0.0;
		}
		else	//正下
		{
			return cellMaxY - length - y;
		}
	}
	else	//网格右边
	{
		if (y > cellMaxY)	//右上
		{
			return getDistance(x, y, cellMinX+length, cellMaxY);
		}
		else if (y >= cellMaxY-length)	//正右
		{
			return getDistance(x, y, cellMinX+length, y);//x - (cellMinX + length);
		}
		else	//右下
		{
			return getDistance(x, y, cellMinX+length, cellMaxY-length);
		}
	}
}

double Grid::getMaxDist(const double x, const double y, const int cellID)
{
	//获得网格左上角点坐标
	double cellMinX = min_x + (cellID % tier) * length;
	double cellMaxY = max_y - (cellID / tier) * length;

	//判断与网格相对位置

	if (x<cellMinX+length*0.5)	//网格垂直平分线左边
	{
		if (y>cellMaxY-length*0.5)	//网格水平平分线上边
		{
			return getDistance(x, y, cellMinX+length, cellMaxY-length);
		}
		else	//网格水平平分线下边
		{
			return getDistance(x, y, cellMinX+length, cellMaxY);
		}
	}
	else	//网格垂直平分线右边
	{
		if (y>cellMaxY-length*0.5)	//网格水平平分线上边
		{
			return getDistance(x, y, cellMinX, cellMaxY-length);
		}
		else	//网格水平平分线下边
		{
			return getDistance(x, y, cellMinX, cellMaxY);
		}
	}
}


vector<int> Grid::forTest(const int source, const double radius)
{
	vector<int> result;

	double x = nodes[source].x;
	double y = nodes[source].y;

	for (int i=0; i!=nodes.size(); ++i)
	{
		double x1 = nodes[i].x;
		double y1 = nodes[i].y;

		if (getDistance(x, y, x1, y1)<=radius)
		{
			result.push_back(i);
		}
	}

	ofstream fout("out.txt");
	for (int i=0; i!=result.size(); ++i)
	{
		fout<<result[i]<<"\t"<<locate(result[i])<<endl;
	}

	return result;
}


#endif