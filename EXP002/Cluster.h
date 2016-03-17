//#pragma once
//关于Cluster类定义的头文件
#include "basic.h"
using namespace std;

class Cluster
{
public:
	int clusterId;//在CluSet中的编号
	int size;//cluster的大小，即所包含的GPS的个数
	double centerX,centerY;//取cluster的所有点的均值来代表cluster的位置
	vector<int> members;//点的序号的集合
	Cluster(int Id,vector<int> pointsId);
};
extern vector<Point> PointSet;//存放全部GPS点的信息
//传入cluster编号和其中的点的编号，构造一个cluster
Cluster::Cluster(int Id,vector<int> pointsId)
{
	clusterId=Id;
	members=pointsId;
	size=pointsId.size();
	double sumX=0;
	double sumY=0;
	for(int i=0;i<size;i++)
	{
		sumX+=PointSet[pointsId[i]].x;
		sumY+=PointSet[pointsId[i]].y;
	}
	centerX=sumX/size;
	centerY=sumY/size;
}


