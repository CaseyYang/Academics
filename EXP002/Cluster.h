//#pragma once
//����Cluster�ඨ���ͷ�ļ�
#include "basic.h"
using namespace std;

class Cluster
{
public:
	int clusterId;//��CluSet�еı��
	int size;//cluster�Ĵ�С������������GPS�ĸ���
	double centerX,centerY;//ȡcluster�����е�ľ�ֵ������cluster��λ��
	vector<int> members;//�����ŵļ���
	Cluster(int Id,vector<int> pointsId);
};
extern vector<Point> PointSet;//���ȫ��GPS�����Ϣ
//����cluster��ź����еĵ�ı�ţ�����һ��cluster
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


