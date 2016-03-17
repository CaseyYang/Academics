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
vector<Point> PointSet;//存放全部GPS点的信息
vector<Cluster> ClusterSet;//存放所有cluster
vector<Trajectory> TrajSet;
void cohExpanding(fstream &fout);
void expand(int pId,fstream &fout);//从一个点p出发，找到一个潜在的cluster
void expand(int pId);
void networkTP(Network &n,fstream &fout);
void timeParti(Edge &e);
void K_means(double center1,double center2,TimeSlot ts,TimeSlot &ts1,TimeSlot &ts2);//给定两个质心的位置，分裂成两个簇，返回两个簇的总的SSE
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
	cout<<"用时"<<(c2-c1)<<"毫秒"<<endl;
	cout<<"cluster数量为："<<ClusterSet.size()<<endl;
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
	cout<<"开始进行cohexpanding算法"<<endl;
	int n=PointSet.size()/3;
	vector<Point>::iterator iter=PointSet.begin();
	for(int i=0;i+3<PointSet.size();i=i+3)//1到6
	{		
		if(PointSet[i].classified==false)
		{
			expand(PointSet[i].pointId,fout);
			PointSet[i].classified=true;
		}
	}
	cout<<"cohexpanding算法结束"<<endl;
	fout<<ClusterSet.size();
	fout.close();
}

//伪代码
/*从一个点p出发，找到一个潜在的cluster
传入一个点的pId；
seeds初始为空；
将pId压入seeds队列中;
while( seeds队列不为空 同时 该循环次数不超过一定值 )
{
	从seeds中取队首元素;
	以该点为圆心，R为半径，找到一组范围内的点pointsId；
	对pointsId中的每个点做如下操作：
	{
		if(该点之前没有被范围查询访问过)
		{
			classified=true;
			计算seedId与该点之间的相关度；
			如果相关度大于某一定值，将该点推入seeds中做种，另外将该点推入一个数组中作为候选cluster中的点。
		}
	}
	如果数组大于某一定值，则新创建一个cluster，赋值为数组：
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
/*二分K均值算法
输入一条边e，根据进入该边的历史数据的时间记录，划分时间段；
伪代码：
将边的时间记录从小到大排序；
初始化簇表vts，即创建一个新的TimeSlotvt，放入边排好序的记录；
repeat
	从簇表vts中取出一个SSE最大的ts；
	//对选定的vt进行多次二分“试验”
	for i=1 to 试验次数 do
		//每次选择的两个初始质心，具体方法为：
			//分别为ts开始处到ts中心的（试验次数+1）分之i处和ts开始处到ts末尾的（试验次数+1）分之i处
		使用基本K均值，二分选定的ts
	从二分试验中选择具有最小总SSE的两个ts
	将这两个ts添加到vts中
until 簇表中包含K个簇
*/
int SSEbound=2;
int maxLen=3;
void timeParti(Edge &e)
{
	vector<double> tSeq=e.tSeq;
	sort(tSeq.begin(),tSeq.end());

	//如果点数不够怎么办？

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
		int count=3;//基本K均值实验次数
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
输入两个初始质心，得到两个分裂的簇
伪代码：
repeat
	将每个点指派到最近的质心center1和center2，形成两个簇；
	重新计算每个簇的质心c1和c2；
	判断c1与center1，c2与center2的距离差，当两个距离差都不大于某一定值时，退出循环；
返回这两个簇ts1和ts2；
*/
void K_means(double center1,double center2,TimeSlot ts,TimeSlot &ts1,TimeSlot &ts2)//给定两个质心的位置，分裂成两个簇
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

/*创建查询文件
采用floyd法求得路网中任意两点的最短通过时间，然后令最晚通过时间=出发时间+最短通过时间段*ratio
另外输出每条最短路径的坐标文件
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
		double startTime=400+(rand()%1600);//生成出发时间，在4点到20点之间
		startTime=startTime/100;
		double travelTime=A[start][end]*ratio;//为通过时间的上限，此处设为最短通过时间的1.5倍
		foutquery<<i<<' '<<start<<' '<<end<<' '<<startTime<<' '<<startTime+travelTime<<endl;

		//输出最短路径文件
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