#include "Network.h"
#include "parameter.h"
#include "Grid.h"
#include <stack>
#include <functional>
using namespace std;

int clusterId=-1;
fstream fin,fout,fout1;
fstream foutEdge;
Grid g;
Network n;
vector<Point> PointSet;//���ȫ��GPS�����Ϣ
vector<Cluster> ClusterSet;//�������cluster
vector<Trajectory> TrajSet;
void cohExpanding(fstream &fout);
void expand(int pId,fstream &fout);//��һ����p�������ҵ�һ��Ǳ�ڵ�cluster
void expand(int pId);
void networkTP(Network &n,fstream &fout);
void timeParti(Edge &e);
void K_means(double center1,double center2,TimeSlot ts,TimeSlot &ts1,TimeSlot &ts2);//�����������ĵ�λ�ã����ѳ������أ����������ص��ܵ�SSE
void q();
void pathFile();
int main()
{
	clock_t c1,c2;
	//int offSetByCaseyYang=atoi(argv[1]);
	//cout<<offSetByCaseyYang;
	
	g.creatGrid("newTrucks.txt",100);
	
	
	fstream fileout1,fileout2,fileout3,fileout4;
	fileout1.open("output1.txt",ios::out);
	fileout1<<"var data = ["<<endl;
	fileout2.open("output2.txt",ios::out);
	fileout3.open("output3.txt",ios::out);
	fileout4.open("output4.txt",ios::out);
	for(int i=0;i<PointSet.size();i=i+4)
	{
		switch(PointSet[i].time.tm_hour){
			case 4:
			case 5:
			case 6:
			case 7:
			case 8:fileout1<<"{"<<endl<<"y:"<<PointSet[i].y<<","<<endl<<"x:"<<PointSet[i].x<<","<<endl<<"t:"<<0<<endl<<"},"<<endl;break;
			case 9:
			case 10:
			case 11:
			case 12:fileout2<<PointSet[i].y<<endl<<PointSet[i].x<<endl<<" "<<endl;break;
			case 13:
			case 14:
			case 15:
			case 16:
			case 17:
			case 18:fileout3<<PointSet[i].y<<endl<<PointSet[i].x<<endl<<" "<<endl;break;
			case 19:
			case 20:
			case 21:
			case 22:fileout4<<PointSet[i].y<<endl<<PointSet[i].x<<endl<<" "<<endl;break;
			default:;
		};
	}
	fileout1<<"]";
	fileout1.close();
	fileout2.close();
	fileout3.close();
	fileout4.close();
	
	/*
	fout.open("represent.txt");
	c1=clock();
	cohExpanding(fout);
	c2=clock();
	cout<<"��ʱ"<<(c2-c1)<<"����"<<endl;
	cout<<"cluster����Ϊ��"<<ClusterSet.size()<<endl;
	fout.close();

	fin.open("Trajectories.txt");
	fout.open("newTrajectories.txt",ios::out);
	n.creatNetwork(fin,fout);
	fin.close();
	fout.close();

	fout.open("edge.txt",ios::out);
	networkTP(n,fout);
	fout.close();

	fstream fileout5;
	fileout5.open("output5.txt",ios::out);
	for(int j=0;j<ClusterSet.size();j++){
		if(n.matrix[j][j].exist==true){
			for(int i=0;i<n.matrix[j][j].tSeq.size();i++)
			{
				cout<<n.matrix[j][j].tSeq[i]<<endl;
			}
			break;
		}
	}
	fileout5.close();
	*/

	return 0;

	/*q();
	pathFile();*/
	/*char c;
	cin>>c;*/
} 


//coherence expanding algorithm for clustering intersections
void cohExpanding(fstream &fout)
{
	fout.setf(ios::fixed);
	fout.precision(6);
	cout<<"��ʼ����cohexpanding�㷨"<<endl;
	int n=PointSet.size()/3;
	vector<Point>::iterator iter=PointSet.begin();
	for(int i=0;i+3<PointSet.size();i=i+3)//1��6
	{		
		if(PointSet[i].classified==false)
		{
			expand(PointSet[i].pointId,fout);
			PointSet[i].classified=true;
		}
	}
	cout<<"cohexpanding�㷨����"<<endl;
	fout<<ClusterSet.size();
	fout.close();
}

//α����
/*��һ����p�������ҵ�һ��Ǳ�ڵ�cluster
����һ�����pId��
seeds��ʼΪ�գ�
��pIdѹ��seeds������;
while( seeds���в�Ϊ�� ͬʱ ��ѭ������������һ��ֵ )
{
	��seeds��ȡ����Ԫ��;
	�Ըõ�ΪԲ�ģ�RΪ�뾶���ҵ�һ�鷶Χ�ڵĵ�pointsId��
	��pointsId�е�ÿ���������²�����
	{
		if(�õ�֮ǰû�б���Χ��ѯ���ʹ�)
		{
			classified=true;
			����seedId��õ�֮�����ضȣ�
			�����ضȴ���ĳһ��ֵ�����õ�����seeds�����֣����⽫�õ�����һ����������Ϊ��ѡcluster�еĵ㡣
		}
	}
	����������ĳһ��ֵ�����´���һ��cluster����ֵΪ���飺
	{

	}
}
*/
void expand(int pId,fstream &fout)
{
	int seedId;
	map<double,int,std::greater<int>> seeds;
	vector<int> resultId;
	vector<int> pointsId;
	vector<int>::iterator iter;  
	int count=0;
	seeds.insert(make_pair(0,pId));
	resultId.push_back(pId);
	while( seeds.empty()==0&&count<10 )
	{
		count++;
		seedId=seeds.begin()->second;
		seeds.erase(seeds.begin());
		pointsId=g.range_query(PointSet[seedId].x,PointSet[seedId].y,R);
		for(iter=pointsId.begin();iter!=pointsId.end();iter++)
		{
			if(PointSet[*iter].classified==false)
			{
				PointSet[*iter].classified=true;
				double coh=PointSet[seedId].coh(PointSet[*iter]);
				if(coh>cohBound )
				{
					seeds.insert(make_pair(coh,*iter));
					resultId.push_back(*iter);
				}
			}					
		}
	}
	if(resultId.size()>cluBound)
	{
		if(g.FindNeighbor(pId,R*3)==1)
			return;
		clusterId++;
		Cluster cluster(clusterId,resultId);
		ClusterSet.push_back(cluster);
		fout<<cluster.centerY<<' '<<cluster.centerX<<endl;
		//fout<<cluster.centerY<<endl;
		//fout<<cluster.centerX<<endl;
		for(iter=resultId.begin();iter!=resultId.end();iter++)
		{
			PointSet[*iter].cluId=clusterId;
		}
	}
}

void networkTP(Network &n,fstream &fout)
{
	int edgecount=0;
	fout.setf(ios::fixed);
	for(int i=0;i<n.nodeNum;i++)
	{
		for(int j=0;j<n.nodeNum;j++)
		{
			if(n.matrix[i][j].exist==true)
			{
				timeParti(n.matrix[i][j]);
				fout<<edgecount<<' '<<i<<"->"<<j<<' ';
				fout.precision(2);				
				fout<<n.matrix[i][j].travelTime<<' '<<n.matrix[i][j].vts.size()<<' ';
				for(int k=0;k<n.matrix[i][j].vts.size();k++)
				{
					fout.precision(1);
					fout<<n.matrix[i][j].vts[k].firstT<<'-'<<n.matrix[i][j].vts[k].lastT<<':';
					fout.precision(2);
					fout<<n.matrix[i][j].vts[k].weight<<' ';
				}
				fout<<endl;
				edgecount++;
			}
		}
	}
}
/*����K��ֵ�㷨
����һ����e�����ݽ���ñߵ���ʷ���ݵ�ʱ���¼������ʱ��Σ�
α���룺
���ߵ�ʱ���¼��С��������
��ʼ���ر�vts��������һ���µ�TimeSlotvt��������ź���ļ�¼��
repeat
	�Ӵر�vts��ȡ��һ��SSE����ts��
	//��ѡ����vt���ж�ζ��֡����顱
	for i=1 to ������� do
		//ÿ��ѡ���������ʼ���ģ����巽��Ϊ��
			//�ֱ�Ϊts��ʼ����ts���ĵģ��������+1����֮i����ts��ʼ����tsĩβ�ģ��������+1����֮i��
		ʹ�û���K��ֵ������ѡ����ts
	�Ӷ���������ѡ�������С��SSE������ts
	��������ts��ӵ�vts��
until �ر��а���K����
*/
int SSEbound=2;
int maxLen=3;
void timeParti(Edge &e)
{
	vector<double> tSeq=e.tSeq;
	sort(tSeq.begin(),tSeq.end());

	//�������������ô�죿

	vector<TimeSlot> vts;
	TimeSlot ts(tSeq);
	vts.push_back(ts);

	while(1)
	{
		double maxSSE=vts[0].SSE;
		int maxPos=0;
		for(int i=1;i<vts.size();i++)
		{
			if(vts[i].SSE>maxSSE)
			{
				maxSSE=vts[i].SSE;
				maxPos=i;
			}
		}
		
		if(maxSSE<SSEbound||vts.size()>=5)
			break;
		ts=vts[maxPos];
		vts.erase(vts.begin()+maxPos);
		TimeSlot ts1,ts2,temp1,temp2;
		int count=3;//����K��ֵʵ�����
		double center1,center2;
		for(int i=1;i<count;i++)
		{
			center1=ts.firstT+(ts.center-ts.firstT)/count*i;
			center2=ts.center+(ts.lastT-ts.center)/3*i;
			K_means(center1,center2,ts,temp1,temp2);
			if(temp1.SSE+temp2.SSE<ts1.SSE+ts2.SSE)
			{
				ts1=temp1;
				ts2=temp2;
			}
		}
		vts.push_back(ts1);
		vts.push_back(ts2);
	}

	e.tSeq.clear();
	sort(vts.begin(),vts.end(),earlier);
	for(int i=0;i<vts.size();i++)
	{
		double len;
		if(i>0&&i<vts.size())
		{
			len=vts[i].firstT-vts[i-1].lastT;
			vts[i].firstT-=len/2;
			vts[i-1].lastT+=len/2;
		}
		if(i==0)
		{
			len=vts[i].firstT-4;
			if(len>maxLen)
			{
				vts[i].firstT-=len/2;
				TimeSlot temp;
				temp.firstT=4;
				temp.lastT=4+len/2;
				temp.center=4+len/4;
				temp.weight=vts[i].weight/3;
				vts.insert(vts.begin(),temp);
			}
			else
			{
				vts[i].firstT=4;
			}
		}
		if(i==vts.size()-1)
		{
			len=22-vts[i].lastT;
			if(len>maxLen)
			{
				vts[i].lastT+=len/2;
				TimeSlot temp;
				temp.firstT=22-len/2;
				temp.lastT=22;
				temp.center=22-len/4;
				temp.weight=vts[i].weight/3;
				vts.push_back(temp);
			}
			else
			{
				vts[i].lastT=22;
			}
		}
	}
	e.vts=vts;
}

/*
����������ʼ���ģ��õ��������ѵĴ�
α���룺
repeat
	��ÿ����ָ�ɵ����������center1��center2���γ������أ�
	���¼���ÿ���ص�����c1��c2��
	�ж�c1��center1��c2��center2�ľ�������������������ĳһ��ֵʱ���˳�ѭ����
������������ts1��ts2��
*/
void K_means(double center1,double center2,TimeSlot ts,TimeSlot &ts1,TimeSlot &ts2)//�����������ĵ�λ�ã����ѳ�������
{
	vector<double> v=ts.tSeq;
	vector<double> v1,v2;
	double c1,c2;
	while(1)
	{
		for(int i=0;i<v.size();i++)
		{
			if(abs(v[i]-center1)<abs(v[i]-center2))
				v1.push_back(v[i]);
			else
				v2.push_back(v[i]);
		}
		double sum=0;
		for(int i=0;i<v1.size();i++)
			sum+=v1[i];
		c1=sum/v1.size();
		sum=0;
		for(int i=0;i<v2.size();i++)
			sum+=v2[i];
		c2=sum/v2.size();
		if(abs(c1-center1)<0.5&&abs(c2-center2)<0.5)
			break;
		center1=c1;
		center2=c2;
		v1.clear();
		v2.clear();
	}
	ts1.creatTS(v1);
	ts2.creatTS(v2);
}
//
void pathFile()
{
	ifstream fin;
	fin.open(".\\path.txt");
	
	string str=".\\results\\pathOffen";
	//for(int i=0;i<100;i++)
	int i=0;
	while(fin.eof()==false)
	{
		char s[3]={'0'};
		itoa(i,s,10);
		string name=str;
		name.append(s);
		name.append(".txt");
		ofstream fout(name);

		int count;
		fin>>count;
		for(int j=0;j<count;j++)
		{
			int id;
			fin>>id;
			fout<<setiosflags(ios::fixed)<<setprecision(6);
			fout<<ClusterSet[id].centerY<<' '<<ClusterSet[id].centerX<<endl;
		}
		fout.close();
		i++;
	}
	fin.close();
}

/*������ѯ�ļ�
����floyd�����·����������������ͨ��ʱ�䣬Ȼ��������ͨ��ʱ��=����ʱ��+���ͨ��ʱ���*ratio
�������ÿ�����·���������ļ�
*/
void q()
{
	double ratio=1.3;
	fstream foutquery,foutShort;
	foutquery.open("query.txt",ios::app);
	string str=".\\results\\pathShort";
	int cluSize=ClusterSet.size();
	double **A=new double*[cluSize];
	int **path=new int*[cluSize];
	double MAXTIME=9999;
	/*for(int i=0;i<cluSize;i++)
	{
		for(int j=0;j<cluSize;j++)
		{
			A[i][j]=9999
		memset(A[i],9999,sizeof(double)*cluSize);
		memset(path[i],-1,sizeof(int)*cluSize);
	}*/
	for(int i=0;i<cluSize;i++)
	{
		A[i]=new double[cluSize];
		path[i]=new int[cluSize];
		for(int j=0;j<cluSize;j++)
		{
			if(n.matrix[i][j].exist==true)
			{
				A[i][j]=n.matrix[i][j].travelTime;
				path[i][j]=i;
			}
			else
			{
				path[i][j]=-1;
				if(i==j)
					A[i][j]=0;
				else
					A[i][j]=MAXTIME;
			}
		}
	}
	for(int k=0;k<cluSize;k++)
	{
		for(int i=0;i<cluSize;i++)
		{
			for(int j=0;j<cluSize;j++)
			{
				if(A[i][k]+A[k][j]<A[i][j])
				{
					A[i][j]=A[i][k]+A[k][j];
					path[i][j]=k;
				}
			}
		}
	}
	int queryNum=100;
	int start,end;
	foutquery<<queryNum<<endl;
	foutquery<<setiosflags(ios::fixed)<<setprecision(2);
	srand(0);
	for(int i=0;i<queryNum;i++)
	{
		do
		{
			start=rand()%cluSize;
			end=rand()%cluSize;
		}while(end==start||path[start][end]==-1);
		double startTime=400+(rand()%1600);//���ɳ���ʱ�䣬��4�㵽20��֮��
		startTime=startTime/100;
		double travelTime=A[start][end]*ratio;//Ϊͨ��ʱ������ޣ��˴���Ϊ���ͨ��ʱ���1.5��
		foutquery<<i<<' '<<start<<' '<<end<<' '<<startTime<<' '<<startTime+travelTime<<endl;

		//������·���ļ�
		stack<int> pathReverse;
		pathReverse.push(end);
		int pre=path[start][end];
		while(pre!=-1)
		{
			pathReverse.push(pre);
			pre=path[start][pre];
		}
		char s[3]={'0'};
		itoa(i,s,10);
		string name=str;
		name.append(s);
		name.append(".txt");
		ofstream fout(name);
		fout<<setiosflags(ios::fixed)<<setprecision(6);
		int next=start;
		while(pathReverse.empty()==false)
		{
			int id=pathReverse.top();
			pathReverse.pop();
			fout<<ClusterSet[id].centerY<<' '<<ClusterSet[id].centerX<<endl;
		}
		fout.close();
	}
	foutquery.close();
}