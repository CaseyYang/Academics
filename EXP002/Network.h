#pragma once
#include "Trajectory.h"
#include "Cluster.h"
//��tm���͵�ʱ��תΪdouble��
double change(tm t)
{
	double h=t.tm_hour;
	double m=((double)t.tm_min)/60;
	return h+m;
}

class TimeSlot
{
public:
	vector<double> tSeq;
	double weight;
	double firstT;
	double lastT;
	double center;
	double SSE;
	TimeSlot()
	{
		SSE=9999;
	}
	TimeSlot(vector<double> ts);
	double getCenter();
	void creatTS(vector<double> ts);
	double getSSE();
};

TimeSlot::TimeSlot(vector<double> ts)
{
	creatTS(ts);
}

void TimeSlot::creatTS(vector<double> ts)
{
	tSeq=ts;
	weight=ts.size();
	firstT=*ts.begin();
	lastT=*(ts.rbegin());
	center=getCenter();
	SSE=getSSE();
}

double TimeSlot::getCenter()
{
	double sum=0;
	for(int i=0;i<tSeq.size();i++)
		sum+=tSeq[i];
	return sum/tSeq.size();
}

double TimeSlot::getSSE()
{
	double SSE=0;
	for(int i=0;i<tSeq.size();i++)
	{
		SSE+=pow((center-tSeq[i]),2);
	}
	return SSE;
}
bool earlier(TimeSlot ts1,TimeSlot ts2)
{
	return ts1.firstT<ts2.firstT;
}
class Edge
{
public:
	vector<double> tSeq;
	vector<TimeSlot> vts;
	double travelTime;
	bool exist;
	double weight;
	Edge()
	{
		weight=0;
		exist=false;
		travelTime=0;
	}
};


class Network
{
public:
	int nodeNum;
	int edgeNum;
	Edge **matrix;
	Network();
	void creatNetwork(fstream &fin,fstream &fout);
	//void timeParti(fstream &fout);
};

Network::Network()
{
	nodeNum=edgeNum=0;
	matrix=NULL;
}

/*��GPS�켣ת��Ϊcluster�켣��ͬʱ����һ��·������ʱ��ͳ��ÿ����ͨ������
����һ��edge���͵Ķ�ά����network������ű���Ϣ��
α���룺
��ʼ���ڽӾ���
�ӹ켣�ļ������ζ���ÿһ���켣�������´���
	�����ҵ��ù켣�ĵ�һ��cluster����Ϊtemp1,���Ӧ�Ĺ켣�еĵ�ΪinEdge��temp1����ù켣��cluster�����У�
	���ϼ��temp1����һ�����ڵ�cluster����Ϊtemp2����Ӧ�켣�еĵ�Ϊi��
		��temp2����켣��cluster�����С�
		����inEdge��i������cluster�����֮���ŷʽ����;
		��С��һ��ֵ:
			��inEdge���ʱ�����ñߵ�ʱ������tSeq��
			��������б��������������ߣ�
				�ñ�exsitֵ��Ϊtrue��������traveltime
		temp1=temp2;
		inEdge=i;
	���µĹ켣����켣��TrajSet�У�
*/
extern vector<Point> PointSet;//���ȫ��GPS�����Ϣ
extern vector<Cluster> ClusterSet;//�������cluster
extern vector<Trajectory> TrajSet;
extern fstream foutEdge;
void Network::creatNetwork(fstream &fin,fstream &fout)
{
	//edgeGoogle.txt���潫Ҫ��google map�ϻ��ıߵ�����
	foutEdge.open("edgeGoogle.txt");
	foutEdge.setf(ios::fixed);
	foutEdge.precision(6);

	nodeNum=ClusterSet.size();
	matrix=new Edge*[nodeNum];
	for(int i=0;i<nodeNum;i++)
		matrix[i]=new Edge[nodeNum];
	Trajectory trajTemp;
	//fout2.open("matlabData.txt",ios::out);
	//�����ڽӾ���
	char buffer[100];
	fin.getline(buffer,100);
	fout<<buffer<<endl;
	while(!fin.eof())
	{
		fin.getline(buffer,100);
		fout<<buffer<<endl;
		trajTemp.creatTraj(buffer);
		int i=trajTemp.startPoint;
		int temp1,temp2;//������ǰ������cluster
		while((i<=trajTemp.endPoint)&&(temp1=PointSet[i].cluId)==-1)
		{
			i++;
		}
		if(i>trajTemp.endPoint)
			continue;
		trajTemp.cluSeq.push_back(temp1);
		fout<<temp1<<' ';
		if(i==trajTemp.endPoint)
			continue;
		//i����ǰcluster��Ӧ�ĵ�����
		//int step=0;//��¼�켣����������cluster֮������˶���GPS��
		int inEdge;
		for(inEdge=i++;i<=trajTemp.endPoint;i++)
		{
			//step++;
			if(((temp2=PointSet[i].cluId)!=-1)&&(temp1!=temp2))//���㲻����ͬһ��cluster
			{
				double x1=ClusterSet[temp1].centerX;
				double y1=ClusterSet[temp1].centerY;
				double x2=ClusterSet[temp2].centerX;
				double y2=ClusterSet[temp2].centerY;
				double dis=(x1-x2)*(x1-x2)+(y1-y2)*(y1-y2);
				dis=sqrt(dis);
				//if((dis<0.031)&&(step>=3))
				if(dis<0.031)
				{
					fout<<temp2<<' ';//��trajectory.txt���cluster������Ϣ
					struct tm t=PointSet[inEdge].time;
					if(t.tm_hour>=22)
						t.tm_hour=21;
					else if(t.tm_hour<4)
						t.tm_hour=4;
					double td=change(t);
					matrix[temp1][temp2].tSeq.push_back(td);
					matrix[temp1][temp2].weight++;
					//if(matrix[temp1][temp2].weight>=2)
					if(matrix[temp1][temp2].exist==false)
					{
						edgeNum++;
						matrix[temp1][temp2].exist=true;
						matrix[temp1][temp2].travelTime=getDistance(x1,y1,x2,y2)/1000/15;
					}
				}
				//step=0;
				trajTemp.cluSeq.push_back(temp2);
				temp1=temp2;
				inEdge=i;
			}
		}
		fout<<endl;
		TrajSet.push_back(trajTemp);
	}
	for(int i=0;i<ClusterSet.size();i++)
	{
		for(int j=i;j<ClusterSet.size();j++)
		{
			if(matrix[i][j].exist==true)
			{
				double w=matrix[i][j].weight+matrix[j][i].weight;
				double l; 
				if(w<50)
					l=1;
				else if(w<100)
					l=2;
				else if(w<200)
					l=4;
				else if(w<300)
					l=5;
				else
					l=6;
				foutEdge<<ClusterSet[i].centerY<<' '<<ClusterSet[i].centerX<<' '<<ClusterSet[j].centerY<<' '<<ClusterSet[j].centerX<<' '<<l<<endl;
			}
		}
	}
}
/*
	//cluster��켣�����С����
	/*fstream foutTest("testOut.txt");
	int testNum=1;
	int i=0;
	while(i<testNum)
	{
		vector<int> clu1,clu2;
		//int cluId=rand()%nodeNum;
		int cluId=6;
		clu1=ClusterSet[cluId].members;
		int flag=0;
		int cluId2;
		int a[timeSlotNum];
		double b[timeSlotNum];
		memset(a,0,sizeof(int)*timeSlotNum);
		memset(b,0,sizeof(double)*timeSlotNum);
		int j;
		//�ҵ�cluId��ĳ����һ��cluster:cluId2
		for(j=0;j<nodeNum;j++)
			if(matrix[cluId][j].exist==true)
			{
				flag=1;
				break;
			}
		if(flag!=1)
			continue;
		i++;
		//cluId2=j;
		cluId2=182;
		clu2=ClusterSet[cluId2].members;
		flag=0;
		sort(clu1.begin(),clu1.end());
		sort(clu2.begin(),clu2.end());
		vector<int>::iterator iter1,iter2;
		iter1=clu1.begin();
		iter2=clu2.begin();
		vector<int> save;
		while(iter1!=clu1.end()&&iter2!=clu2.end())
		{
			if(*iter1==*iter2)
			{
				iter1++;
				iter2++;
				save.push_back(*iter1);
			}
			if(*iter1>*iter2)
				iter2++;
			else
				iter1++;
		}
		cout<<save.size();
				/*for(k=0;k<timeSlotNum-1;k++)
				{
					int h=PointSet[pId].time.tm_hour;
					if(h>=matrix[cluId][cluId2].w[k]&&h<matrix[cluId][cluId2].w[k+1])
						break;
				}
				if(a[k]==1)
					continue;
				ts=k;
				k=pId;
				int count=0;
				double angle=0;
				while(PointSet[k].cluId!=cluId2)
				{
					angle+=PointSet[k+1].movDir-PointSet[k].movDir;
					count++;
				}
				angle=angle/count;
				a[ts]=1;
				b[ts]=angle;
			
		foutTest<<"from "<<cluId<<" to "<<cluId2<<":";
		for(j=0;j<timeSlotNum;j++)
		{
			if(a[j]==1)
			{
				foutTest<<" timeslot"<<j<<':'<<b[j]<<';';
			}
		}
		foutTest<<endl;*/
	//}

