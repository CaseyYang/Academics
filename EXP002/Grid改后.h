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
	vector<int> range_query(const double x, const double y, const double radius);	//��Χ��ѯ�����ص��ID

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
	double length;	//����߳�

	double max_x, min_x;	//����߽���Ϣ
	double max_y, min_y;

	int locate(const int nodeID);	//�жϸõ����ĸ������У���������ID
	int locate(const double x, const double y);

	double getMinDist(const double x, const double y, const int cellID);	//����������С����
	double getMaxDist(const double x, const double y, const int cellID);	//��������������
};

Grid::Grid(const string nodeFile, const int tier)
{
	/*
	*		�������Ϣ����¼�߽�����
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
		//���벢���������
		int no;
		double x, y;
		fin >> no;
		fin >> x;
		fin >> y;
		nodes.push_back(Node(x, y));

		//���±߽�����
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
	*		��������
	*/

	this->tier = tier;

	//��������߳�
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
	length *= 1.0001;	//������䳤�ʵ�����ʹ������߿���Һ��±߽��ϲ�����ڵ�
	
	//��ÿ������䵽����������
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
	vector<int> result;	//�����
	
	//��ȡ���ĵ��������񣬲��������ĸ����������չ�Ĵ���
	int centerCell = locate(x, y);
	int left = centerCell % tier;
	int right = tier - (centerCell % tier) - 1;
	int up = centerCell / tier;
	int down = tier - (centerCell / tier) - 1;

	priority_queue<CellItem> candidateCells;
	bool *valid = new bool[tier*tier]; //��Ӧ�����Ƿ�������У�����Ϊtrue
	for (int i=0; i!=tier*tier; ++i)
	{
		valid[i] = false;
	}
	
	candidateCells.push(CellItem(centerCell, 0.0));
	valid[centerCell] = true;
	int times = 1;	//��ɢ����
	while (!candidateCells.empty())
	{
		CellItem nextCell = candidateCells.top();
		candidateCells.pop();

		if (nextCell.minDist > radius)	//������ȫ��Բ������
		{
			break;
		}

		double cellMaxDist = getMaxDist(x, y, nextCell.cellID);
		vector<int> nodesOfCell = cells[nextCell.cellID].getAllNodes();
		if (cellMaxDist<=radius)	//������ȫ��Բ����
		{
			for (int i=0; i!=nodesOfCell.size(); ++i)
			{
				result.push_back(nodesOfCell[i]);
			}
		}
		else	//������Բ�����ཻ
		{
			//�������ÿ�����Ƿ�����Բ��
			for (int i=0; i!=nodesOfCell.size(); ++i)
			{
				int nextNode = nodesOfCell[i];
				if (getDistance(x, y, nodes[nextNode].x, nodes[nextNode].y) <= radius)
				{
					result.push_back(nextNode);
				}
			}
		}

		//��ɢ�ھ�����
		if (candidateCells.empty())
		{
			bool upValid = false;
			bool downValid = false;
			bool leftValid = false;
			bool rightValid = false;

			if (times<=up)	//δ�����
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
			if (times<=down)	//δ�����
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
			if (times<=left)	//δ�����
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
			if (times<=right)	//δ�����
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
	//����������Ͻǵ�����
	double cellMinX = min_x + (cellID % tier) * length;
	double cellMaxY = max_y - (cellID / tier) * length;

	//�ж����������λ��

	if (x<cellMinX)	//�������
	{
		if (y > cellMaxY)	//����
		{
			return getDistance(x, y, cellMinX, cellMaxY);
		}
		else if (y >= cellMaxY-length)	//����
		{
			return getDistance(x, y, cellMinX, y);//cellMinX - x;
		}
		else	//����
		{
			return getDistance(x, y, cellMinX, cellMaxY-length);
		}
	}
	else if (x<cellMinX+length)	//����������
	{
		if (y > cellMaxY)	//����
		{
			return y - cellMaxY;
		}
		else if (y >=cellMaxY - length)	//����
		{
			return 0.0;
		}
		else	//����
		{
			return cellMaxY - length - y;
		}
	}
	else	//�����ұ�
	{
		if (y > cellMaxY)	//����
		{
			return getDistance(x, y, cellMinX+length, cellMaxY);
		}
		else if (y >= cellMaxY-length)	//����
		{
			return getDistance(x, y, cellMinX+length, y);//x - (cellMinX + length);
		}
		else	//����
		{
			return getDistance(x, y, cellMinX+length, cellMaxY-length);
		}
	}
}

double Grid::getMaxDist(const double x, const double y, const int cellID)
{
	//����������Ͻǵ�����
	double cellMinX = min_x + (cellID % tier) * length;
	double cellMaxY = max_y - (cellID / tier) * length;

	//�ж����������λ��

	if (x<cellMinX+length*0.5)	//����ֱƽ�������
	{
		if (y>cellMaxY-length*0.5)	//����ˮƽƽ�����ϱ�
		{
			return getDistance(x, y, cellMinX+length, cellMaxY-length);
		}
		else	//����ˮƽƽ�����±�
		{
			return getDistance(x, y, cellMinX+length, cellMaxY);
		}
	}
	else	//����ֱƽ�����ұ�
	{
		if (y>cellMaxY-length*0.5)	//����ˮƽƽ�����ϱ�
		{
			return getDistance(x, y, cellMinX, cellMaxY-length);
		}
		else	//����ˮƽƽ�����±�
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