#include<iostream>
#include<vector>
#include<fstream>
#include<string>
#include<set>
#include<algorithm>
#include<map>
#include<queue>
#include<stack>
#include<sstream>
#include<time.h>
#include<iomanip>
#define _USE_MATH_DEFINES
#include<math.h>
#define eps 1e-8
using namespace std;

const int MaxLength = 100000;//��ʾ����ͨ

//��̬�ȶ�
class HotInterval
{
public:
	double StartTimeStamp;
	double EndTimeStamp;
	double Popularity;
	HotInterval(double start, double end, double popularity)
	{
		StartTimeStamp = start;
		EndTimeStamp = end;
		Popularity = popularity;
	}
};
//�����
class Node{
public:
	int NodeID;
	double x;
	double y;
	int parentNodeID;
	double Balance;
	double Gvalue;
	double Hvalue;
	double StartTime;
	int NumOfParentNodes;
	Node(int nodeId, double x, double y)
	{
		this->x = x;
		this->y = y;
		NodeID = nodeId;
		Gvalue = Hvalue = parentNodeID = -1;
		StartTime = Balance = 0;
		NumOfParentNodes=0;
	}
	Node()
	{
		NodeID = -1;
		x = y = 0;
		parentNodeID = -1;
		StartTime = Hvalue = Gvalue = Balance = 0;
		NumOfParentNodes=0;
	}
};
//·��
class Edge{
public:
	int EdgeID;
	int StartNodeID;
	int EndNodeID;
	double length;
	double Popularity;
	double shortestTime;
	vector<HotInterval> HotIntervalList;//·�εĶ�̬�ȶ�����
	set<int> TrajSetContainedThisEdge;//Ϊ�ӿ���������ٶȣ�Ԥ�ȼ�¼����Щ�켣ͨ��������
	Edge(int startId, int endId, double length)
	{
		this->StartNodeID = startId;
		this->EndNodeID = endId;
		this->length = length;
		this->EdgeID = count++;
		this->shortestTime = MaxLength;
	}
	Edge()
	{
		this->EdgeID = -1;
		this->EndNodeID = -1;
		this->length = MaxLength;
		this->Popularity = -1;
		this->StartNodeID = -1;
		this->shortestTime = MaxLength;
	}
	friend bool operator<(const Edge& a, const Edge& b)
	{
		return a.EdgeID < b.EdgeID;
	}
	friend bool operator==(const Edge& a, const Edge& b)
	{
		return a.EdgeID == b.EdgeID;
	}
private:
	static int count;
};
int Edge::count = 0;
//��ʵ�켣
class Traj{
public:
	vector<Edge> TrajRoutes;
	set<int> TrajRouteSet;
	vector<int> TrajNodes;
	int TrajID;
	Traj(){
		TrajRoutes = vector<Edge>();
		TrajNodes = vector<int>();
		TrajID=count++;
	}
	void Add(Edge& edge)
	{
		//proof of data problem
		//if(this->TrajRoutes.size()>0&&this->TrajRoutes.back().EndNodeID!=edge.StartNodeID){
		//	cout<<"�켣"<<TrajID<<"��"<<this->TrajRoutes.size()<<"���ߺ�֮��һ���߲�������"<<endl;
		//}
		this->TrajRoutes.push_back(edge);
		this->TrajRouteSet.insert(edge.EdgeID);
	}
	void AddNode(int nodeID){
		TrajNodes.push_back(nodeID);
	}
private:
	static int count;
};
int Traj::count=0;
//��ѯ
class Query
{
public:
	int TrajID;//�켣ID����ţ�
	int StartNodeID;//��ͬ�����
	int EndNodeID;//��ͬ���յ�
	//double passTime;//����켣����Ӧ��DeltaTֵ
	double startTime;//��ѯ��ʼʱ��
	Query(int id, int start, int end, double startTime)
	{
		TrajID = id;
		StartNodeID = start;
		EndNodeID = end;
		//passTime = time;
		this->startTime = startTime;
	}
	Query()
	{
		TrajID = -1;
		StartNodeID = -1;
		EndNodeID = -1;
		//passTime = 0;
	}
};
//ʵ��켣
class ResultTraj{
public:
	vector<int> NodeIDList;
	vector<double> TimeStampList;
	int IndexOfRawTrajs;
	double ActualHotness;
	ResultTraj(){
		NodeIDList=vector<int>();
		TimeStampList=vector<double>();
		IndexOfRawTrajs=0;
		ActualHotness=0;
	}
	bool Add(int NodeID,double timeStamp){
		if(NodeIDList.size()==TimeStampList.size()){
			NodeIDList.push_back(NodeID);
			TimeStampList.push_back(timeStamp);
			return true;
		}
		else
		{
			cout<<"�����ˣ�ResultTraj��NodeIDList��TimeStampList������һ�£�"<<endl;
			return false;
		}
	}
};


//ȫ�ֲ���
int NumOfNodes = 0;//�����
int NumOfEdges = 0;//�߸���
int NumOfTrajs = 0;//�켣����
int NumOfQueries = 0;//��ѯ��Ŀ����
int NumofResultTrajs=0;//ʵ�����켣����
map<int, Node> NodeSet;//��ֵ��<��ID����>
vector<Edge> EdgeList;//�߼���
vector<Traj> TrajList;//�켣����
vector<vector<Edge>> graph;//·��
vector<Query>QuerySet;//��ѯ��Լ���
vector<ResultTraj>ResultTrajs;//ʵ�����켣����
vector<vector<double>> Distances;//������·�ڼ��������
vector<Traj> CompareTrajs;//����DTWDʱ��ʵ�����켣�Ƚϵ�ԭʼ�켣���Ӽ�
const double Speed = 20;//�ٶȣ���λ��km/h��
double DeltaT;//Delta_T
int StartNodeID;//���
int DestinationNodeID;//�յ�

//�����ã���¼������ִ��ʱ��
double totalTimeForCalculateH;
int totalCountForCalculateH;
double totalTimeForUpdateTrajSet;
int totalCountForUpdateTrajSet;
double totalTimeForAssumeUpdateTrajSet;
int totalCountForAssumeUpdateTrajSet;


//�������Ϣ
void ReadInNode()
{
	ifstream reader("Node.txt");
	int nodeId;
	double x;
	double y;
	if (reader)
	{
		reader >> NumOfNodes;
		graph = vector<vector<Edge>>(NumOfNodes);
		for (int i = 0; i < NumOfNodes; i++)
		{
			graph[i].resize(NumOfNodes);
			graph[i][i].length = 0;
			reader >> nodeId;
			reader >> setprecision(11)>>y;
			reader >> setprecision(11)>>x;
			NodeSet[nodeId] = Node(nodeId, x, y);
		}
	}
	else
	{
		cout << "����Ϣ�ļ�����" << endl;
	}
	//cout << "����Ϣ��" << setprecision(11) << NodeSet[nodeId].x << " " << setprecision(11)<< NodeSet[nodeId].y << endl;
	reader.close();
}
//�������Ϣ
void ReadInEdge(){
	ifstream reader("Edge.txt");
	int start;
	int end;
	double length;
	if (reader)
	{
		reader >> NumOfEdges;
		while (reader >> start)
		{
			reader >> end;
			reader >> setprecision(11)>> length;
			if (start < NumOfNodes && end < NumOfNodes && length < MaxLength)
			{
				graph[start][end] = Edge(start, end, length);
				//graph[start][end] = graph[end][start];
				EdgeList.push_back(graph[start][end]);
			}
			else
			{
				cout << "����Ϣ�ļ�����" << endl;
				return;
			}
		}
	}
	//cout << "����Ϣ��" << EdgeList.back().StartNodeID << " " << EdgeList.back().EndNodeID <<" "<<setprecision(11)<<EdgeList.back().length<<endl;
	reader.close();
}
//����ߵ��ȶ���Ϣ
void ReadInEdgePopularity()
{
	ifstream reader("EdgePopularity.txt");
	int startNode;
	int endNode;
	double startTimeStamp;
	double endTimeStamp;
	double hotness = 0;
	//double popularity;
	int countOfInterval;
	if (reader)
	{
		reader >> startNode;
		while (reader >> startNode)
		{
			reader >> endNode;
			//int edgeIndex=-1;
			//for (int i = 0; i<EdgeList.size(); i++){
			//	if (EdgeList[i].StartNodeID == startNode && EdgeList[i].EndNodeID == endNode||EdgeList[i].StartNodeID == endNode && EdgeList[i].EndNodeID == startNode){
			//		edgeIndex=i;
			//		break;
			//	}
			//}
			reader >> countOfInterval;
			while (countOfInterval > 0)
			{
				countOfInterval--;
				string str;
				reader >> str;
				string::size_type hyphen = str.find_first_of('-', 0);
				string::size_type colon = str.find_first_of(':', 0);
				string substrStartTime = str.substr(0, hyphen);
				string substrEndTime = str.substr(hyphen + 1, colon - hyphen-1);
				string substrHotness = str.substr(colon + 1);
				stringstream sstr(substrStartTime);
				sstr >> startTimeStamp;
				sstr.clear();
				sstr.str(substrEndTime);
				sstr >> endTimeStamp;
				sstr.clear();
				sstr.str(substrHotness);
				double currentHotness;
				sstr >>setprecision(11)>> currentHotness;
				sstr.clear();
				//proof of data problem
				if(graph[startNode][endNode].EdgeID==-1){
					cout<<"�ȶ���Ϣ��Ӧ�Ĺ켣�����ڣ�"<<endl;
				}
				graph[startNode][endNode].HotIntervalList.push_back(HotInterval(startTimeStamp, endTimeStamp, currentHotness));
				hotness += currentHotness;
				//if(edgeIndex!=-1){
				//	EdgeList[edgeIndex].HotIntervalList.push_back(HotInterval(startTimeStamp, endTimeStamp, currentHotness));
				//	hotness += currentHotness;
				//}
			}
			//if(edgeIndex!=-1){
			//	EdgeList[edgeIndex].Popularity = hotness;
			//}
			//hotness = 0;
		}
	}
	else
	{
		cout << "���ȶ���Ϣ�ļ�����" << endl;
	}
	//cout << "�ȶ���Ϣ��" << EdgeList.back().StartNodeID << " " << EdgeList.back().EndNodeID<< " " << EdgeList.back().HotIntervalList.back().StartTimeStamp << " " << EdgeList.back().HotIntervalList.back().Popularity << " " << EdgeList.back().HotIntervalList.back().EndTimeStamp << endl;
	reader.close();
}
//����켣��Ϣ
void ReadInTraj()
{
	ifstream reader("Traj.txt");
	int length;
	int id;
	int previousID = -1;
	double time;
	int count = 0;
	if (reader)
	{
		reader >> NumOfTrajs;
		TrajList = vector<Traj>(NumOfTrajs);
		while (reader >> length)
		{
			previousID = -1;
			for (int i = 0; i < length; i++)
			{
				reader >> id;
				reader >> setprecision(11)>> time;
				TrajList[count].AddNode(id);
				if (previousID != -1)
				{
					TrajList[count].Add(graph[previousID][id]);
					//proof of data problem
					//if(graph[previousID][id].EdgeID==-1){
					//	cout<<"��Ϊ��ʵ�켣�������ı���·���ϲ������ڣ�ʹ�ü����ȶ�ʱ������൱����ʵ����������/���ӵ�����"<<endl;
					//}
					graph[previousID][id].TrajSetContainedThisEdge.insert(count);
				}
				previousID = id;
			}
			count++;
		}
	}
	else
	{
		cout << "�켣�ļ�����";
		return;
	}
	reader.close();
	//cout << "�켣��Ϣ��" << (graph[451][442].TrajSetContainedThisEdge.find(--count) != graph[451][442].TrajSetContainedThisEdge.end()) << endl;
}
//�����ѯ�㼯
void ReadInQuerySet(string fileName)
{
	ifstream reader(fileName+".txt");
	int id;
	int useless;
	int queryStartNodeID;
	int queryEndNodeID;
	double startTime;
	if (reader)
	{
		reader >> NumOfQueries;
		while (reader >> id)
		{
			reader >> useless;
			reader >> useless;
			reader >> queryStartNodeID;
			reader >> queryEndNodeID;
			reader >> setprecision(11)>> startTime;
			QuerySet.push_back(Query(id, queryStartNodeID, queryEndNodeID, startTime));
		}
	}
	else
	{
		cout << "��ѯ�㼯�ļ�����";
		return;
	}
	reader.close();
	//cout << "��ѯ��Ϣ��" << QuerySet.back().trajId<<" "<<QuerySet.back().StartNodeID << " " << QuerySet.back().EndNodeID << " " << QuerySet.back().startTime << " " << QuerySet.back().passTime << " " << endl;
}

//Ԥ���������������������ʱ��
void CalculateShortestTime()
{
	vector<vector<double>> A(NumOfNodes);
	vector<vector<double>> path(NumOfNodes);
	for (int i = 0; i < NumOfNodes; i++)
	{
		A[i] = vector<double>(NumOfNodes);
		path[i] = vector<double>(NumOfNodes);
		for (int j = 0; j < NumOfNodes; j++)
		{
			A[i][j] = graph[i][j].length;
			path[i][j] = -1;
		}
	}
	int i, j, k;
	for (k = 0; k < NumOfNodes; k++)
	{
		for (i = 0; i < NumOfNodes; i++)
		{
			for (j = 0; j < NumOfNodes; j++)
			{
				if (A[i][k] != MaxLength && A[k][j] != MaxLength && A[i][j]>A[i][k] + A[k][j])
				{
					A[i][j] = A[i][k] + A[k][j];
					path[i][j] = k;
				}
			}
		}
	}
	for (i = 0; i < NumOfNodes; i++)
	{
		for (j = 0; j < NumOfNodes; j++)
		{
			if (A[i][j] < MaxLength)
			{
				double shortestTime = A[i][j] / (Speed*1000);
				graph[i][j].shortestTime = shortestTime;
				//graph[j][i].shortestTime = shortestTime;
			}
		}
	}
}

//�����������õ�������i��Hֵ
double CalculateH(int i, set<int> &processedEdgeList)
{
	//double start, finish;
	//start = clock();
	int countForResult=0;
	double result = 0;
	for (int j = 0; j < NumOfEdges; j++)
	{
		//if(processedEdgeList.find(EdgeList[j])==processedEdgeList.end())
		if (find(processedEdgeList.begin(), processedEdgeList.end(), j) == processedEdgeList.end())
		{
			double formTime = graph[i][EdgeList[j].StartNodeID].shortestTime;
			double nextTime = graph[EdgeList[j].EndNodeID][DestinationNodeID].shortestTime;
			double totalTime = formTime + nextTime + EdgeList[j].shortestTime;
			if (totalTime <= NodeSet[i].Balance)
			{
				//��̬�ȶ�
				//result+=EdgeList[j].Popularity;

				//��̬�ȶ�
				double theEaliestArrvialTime = NodeSet[i].StartTime + formTime;
				double theLatestDepartureTime = NodeSet[StartNodeID].StartTime + DeltaT - nextTime;
				//����2014��1��10���ʼ�����
				double currentEdgePopularity = 0;
				int countForCurrentEdge=0;
				for (int k = 0; k < EdgeList[j].HotIntervalList.size(); k++)
				{
					if (EdgeList[j].HotIntervalList[k].StartTimeStamp < theLatestDepartureTime && EdgeList[j].HotIntervalList[k].EndTimeStamp > theEaliestArrvialTime)
					{
						currentEdgePopularity += EdgeList[j].HotIntervalList[k].Popularity;
						countForCurrentEdge++;
					}
				}
				currentEdgePopularity/=countForCurrentEdge;
				result+=currentEdgePopularity;
				countForResult++;
			}
		}
	}
	result/=countForResult;
	//finish = clock();
	//totalCountForCalculateH++;
	//totalTimeForCalculateH += (double)(finish - start) / CLOCKS_PER_SEC;

	return result;
	//return result/countForResult;
}
//��������������Ѹ����߼����������У�Ϊ�˼���һ�����ܵ�Gֵ
//int AssumeUpdateTrajSet(Edge& edge, set<int> concernedTrajSet)
//{
//	double start, finish;
//	//start = clock();
//	concernedTrajSet.insert(edge.TrajSetContainedThisEdge.begin(), edge.TrajSetContainedThisEdge.end());
//	//finish = clock();
//	//totalCountForAssumeUpdateTrajSet++;
//	//totalTimeForAssumeUpdateTrajSet += (double)(finish - start) / CLOCKS_PER_SEC;
//	return concernedTrajSet.size();
//}
//��������������2014��1��5�յ��ʼ���д����Gֵ�ĺ���
double CalculateG(int parentNodeID,int currentNodeID){
	Edge& currentEdge=graph[parentNodeID][currentNodeID];
	double currentPopularity=0;
	double startTime=NodeSet[parentNodeID].StartTime;
	//�ҵ�����Ӧʱ�ε��ȶ�
	for (int k = 0; k < currentEdge.HotIntervalList.size(); k++)
	{
		if (currentEdge.HotIntervalList[k].StartTimeStamp <= startTime && startTime<=currentEdge.HotIntervalList[k].EndTimeStamp)
		{
			currentPopularity =currentEdge.HotIntervalList[k].Popularity;
			break;
		}
	}
	return (NodeSet[parentNodeID].Gvalue*NodeSet[parentNodeID].NumOfParentNodes+currentPopularity)/(NodeSet[parentNodeID].NumOfParentNodes+1);
}
//������������openList���а�F=G+H���н�������
bool compare(const int & m1, const int & m2) {
	return (NodeSet[m1].Gvalue + NodeSet[m1].Hvalue) < (NodeSet[m2].Gvalue + NodeSet[m2].Hvalue);
}
//�������������·��
void OutputPathToTxtFile(int queryIndex, string fileName)
{
	stack<int> nextIdStack;
	int currentPoint = DestinationNodeID;
	int count = 0;
	while (currentPoint != -1)
	{
		count++;
		nextIdStack.push(currentPoint);
		currentPoint = NodeSet[currentPoint].parentNodeID;
	}
	ofstream writer("Result_"+fileName+".txt", ofstream::app);
	ofstream writer2("Ratio_"+fileName+".txt", ofstream::app);
	writer << QuerySet[queryIndex].TrajID << " " << count << endl;
	int parentPointID=-1;
	double arriveTime=0;
	double routeLength=0;
	while (!nextIdStack.empty())
	{
		if(parentPointID==-1){
			arriveTime=QuerySet[queryIndex].startTime;
			parentPointID=nextIdStack.top();	
		}
		else{
			routeLength+=graph[parentPointID][nextIdStack.top()].length;
			arriveTime+=graph[parentPointID][nextIdStack.top()].shortestTime;
		}
		writer << nextIdStack.top() << " "<<arriveTime<<endl;
		nextIdStack.pop();
	}
	writer<<endl;
	double ratio=routeLength/(graph[StartNodeID][DestinationNodeID].shortestTime*(Speed*1000));
	if(ratio>1000){
		ratio=ratio/1000;
	}
	else{
		if(ratio>100){
			ratio=ratio/100;
		}
	}
	writer2<<ratio<<endl;
	writer.close();
	writer2.close();
}
//�������������·����js�ļ�
void OutputPathToJSFile(int queryIndex)
{
	stack<int> nextIdStack;
	int currentPoint = DestinationNodeID;
	int count = 0;
	while (currentPoint != -1)
	{
		count++;
		nextIdStack.push(currentPoint);
		currentPoint = NodeSet[currentPoint].parentNodeID;
	}
	stringstream ss;
	string s;
	ss << queryIndex;
	ss >> s;
	ofstream writer("data"+s+".js", ofstream::app);
	writer<<"data"<<s<<" = ["<<endl;
	while (!nextIdStack.empty())
	{
		Node &currentNode=NodeSet.at(nextIdStack.top());
		writer << "{ NodeID:"<<currentNode.NodeID<<", NodeX:"<<currentNode.x<<", NodeY:"<<currentNode.y << " },"<<endl;
		nextIdStack.pop();
	}
	writer <<"]"<<endl;
	writer.close();
}

//���·
bool NormalAStar(int startNodeID,int destination){
	int currentNodeID=startNodeID;
	if(graph[currentNodeID][destination].shortestTime<MaxLength){//ȷ���н�
		while(currentNodeID!=destination){//��ֹ����
			cout<<currentNodeID<<endl;
			for(int i=0;i<NumOfNodes;i++){
				if(0 < graph[currentNodeID][i].length&&graph[currentNodeID][i].length<MaxLength){//currentNodeID��i��ͨ
					if(abs((graph[currentNodeID][i].shortestTime+graph[destination][i].shortestTime)-graph[currentNodeID][destination].shortestTime)<1e-8){
						NodeSet[i].parentNodeID=currentNodeID;
						currentNodeID=i;
						break;
					}
				}
			}
		}
		return true;
	}
	else{
		return false;
	}
}
//A*�㷨
bool AStar(int queryIndex)
{
	//�о�startNodeID��destinationNodeIDû�б�Ҫ����ȫ�ֱ���StartNodeID��DestinationsNodeID�ظ�
	//int startNodeID = QuerySet[queryIndex].StartNodeID;
	//int destinationNodeID = QuerySet[queryIndex].EndNodeID;
	vector<int> openList;//��ѡ��
	set<int> processedEdgeList;//��¼��Щ���Ѿ��������ڲ�ѯ�����
	set<int> closedList;//�����ٿ��ǵĸ��ӣ�����
	set<int> concernedTrajSet;//��Ӧ��Ӧÿ�ֺ�ѡ���������ȫ�ֱ�����
	//bool resultFromNormalAStar=false;//�ʹ�ͳA*�㷨�йأ������ط�û��
	openList.push_back(StartNodeID);
	NodeSet[StartNodeID].Balance=DeltaT;
	int currentNodeID;
	//while(openList
	while (!openList.empty() && closedList.count(DestinationNodeID) != 1)//��������
	{
		currentNodeID = openList.back();
		openList.pop_back();
		closedList.insert(currentNodeID);
		//����currentPoint
		if (NodeSet[currentNodeID].parentNodeID != -1)//���parentNodeIDΪ-1����˵������ʼ��
		{
			processedEdgeList.insert(graph[NodeSet[currentNodeID].parentNodeID][currentNodeID].EdgeID);
			concernedTrajSet.insert(graph[currentNodeID][NodeSet[currentNodeID].parentNodeID].TrajSetContainedThisEdge.begin(), graph[currentNodeID][NodeSet[currentNodeID].parentNodeID].TrajSetContainedThisEdge.end());//���Ҳ����д
			//����Ϊ��Ҫ�������ٴμ���Gֵ��
			//NodeSet[currentNodeID].Gvalue = (concernedTrajSet.size()+0.0) / processedEdgeList.size();//TODO: �������д
			//NodeSet[currentNodeID].Balance = NodeSet[NodeSet[currentNodeID].parentNodeID].Balance - graph[NodeSet[currentNodeID].parentNodeID][currentNodeID].length / (Speed*1000);
			//if(NodeSet[currentPoint].Balance<0){
			//	cout<<"BalanceС���㣡"<<endl;
			//	resultFromNormalAStar=NormalAStar(NodeSet[currentPoint].parentNodeID,destination);
			//	cout<<resultFromNormalAStar<<endl;
			//	break;
			//}
		}
		for (int i = 0; i < NumOfNodes; i++)
		{
			if (closedList.count(i)==0 && 0<graph[currentNodeID][i].length && graph[currentNodeID][i].length<MaxLength)//��i�͵�ǰ����չ��֮����ڱ�
			{
				double gOfCurrentCandidateNode = CalculateG(currentNodeID,i);//2014��1��5������ļ���Gֵ�ķ���
				//int size = AssumeUpdateTrajSet(graph[i][currentNodeID], concernedTrajSet);//2014��1��5�ո�������֮ǰ��Gֵ���������ڵ�gOfCurrentCandidateNode
				if (find(openList.begin(), openList.end(), i) != openList.end())//�жϵ�i�Ƿ��Ѵ�����openList��
				{
					if (gOfCurrentCandidateNode>NodeSet[i].Gvalue)
					{
						NodeSet[i].Gvalue = gOfCurrentCandidateNode;
						NodeSet[i].Hvalue = CalculateH(i, processedEdgeList);
						NodeSet[i].Balance=NodeSet[currentNodeID].Balance-graph[currentNodeID][i].shortestTime;
						NodeSet[i].parentNodeID = currentNodeID;
						NodeSet[i].StartTime = NodeSet[currentNodeID].StartTime + graph[currentNodeID][i].shortestTime;
					}
				}
				else
				{
					openList.push_back(i);
					NodeSet[i].Gvalue = gOfCurrentCandidateNode;
					NodeSet[i].Hvalue = CalculateH(i, processedEdgeList);
					NodeSet[i].Balance=NodeSet[currentNodeID].Balance-graph[currentNodeID][i].shortestTime;
					NodeSet[i].parentNodeID = currentNodeID;
					NodeSet[i].StartTime = NodeSet[currentNodeID].StartTime + graph[currentNodeID][i].shortestTime;
				}
			}
		}
		sort(openList.begin(), openList.end(), compare);
	}
	//���·��
	if (closedList.count(DestinationNodeID) == 1)
	{
		return true;
	}
	else
	{
		cout << "û���ҵ����·����" << endl;
		return false;
	}
}


//������ʵ����������

//����ʵ�����켣
//�����ѯ����켣��Ϣ
void ReadInResultTrajs(string fileName){
	ifstream fin1(fileName+".txt");
	if (fin1)
	{
		ResultTrajs = vector<ResultTraj>();
		CompareTrajs = vector<Traj>();
		int indexofTraj = 0;
		int indexofRawTrajs = -1;
		while(fin1>>indexofRawTrajs){
			int count = 0;
			fin1>>count;
			ResultTrajs.push_back(ResultTraj());
			ResultTrajs.back().IndexOfRawTrajs=indexofRawTrajs;
			CompareTrajs.push_back(Traj());
			for(int i=0;i<count;i++){
				int nodeID;
				fin1>>nodeID;
				double time;
				fin1>>time;
				if(!ResultTrajs.back().Add(nodeID,time)){
					cout<<"�����ѯ����켣������"<<indexofTraj<<"���켣Node��TimeStamp������ƥ�䣡"<<endl;
					return;
				}
			}
			indexofTraj++;
		}
	}
	else
	{
		cout<<"��ѯ����ļ�����";
	}
	fin1.close();
	for(int i=0;i<ResultTrajs.size();i++){
		ResultTraj& currentResultTraj = ResultTrajs.at(i);
		Traj &currentRawTraj=TrajList.at(currentResultTraj.IndexOfRawTrajs);
		for(int j = 0;j < currentRawTraj.TrajRoutes.size();j++){
			Node &currentRawNode = NodeSet[currentRawTraj.TrajNodes.at(j)];
			if(currentResultTraj.NodeIDList.at(0) == currentRawNode.NodeID){
				int offset=0;
				do{
					CompareTrajs.at(i).TrajNodes.push_back(currentRawTraj.TrajNodes.at(j+offset));
					offset++;
				}while(((j+offset)<currentRawTraj.TrajNodes.size())&&(currentRawTraj.TrajNodes.at(j+offset)!=currentResultTraj.NodeIDList.back()));
				break;
			}
		}
	}
}
//������������TrajList��ʵ���ȶȽ��н�������
bool compareByActualHotness(const ResultTraj & traj1, const ResultTraj & traj2) {
	return traj1.ActualHotness > traj2.ActualHotness;
}
//���ʵ���ȶ�
void GetActualHotness(string fileName){
	for(int i=0;i<ResultTrajs.size();i++){
		for(int j=0;j<ResultTrajs.at(i).NodeIDList.size();j++){
			for(int k=0;k<ResultTrajs.size();k++){
				if(k!=i){
					for(int l=0;l<ResultTrajs.at(k).NodeIDList.size();l++){
						if(ResultTrajs.at(k).TimeStampList.at(l)>=ResultTrajs.at(i).TimeStampList.at(j)-eps && ResultTrajs.at(k).TimeStampList.at(l)<=ResultTrajs.at(i).TimeStampList.at(j)+eps){
							ResultTrajs.at(i).ActualHotness++;
							break;
						}
					}
				}
			}
		}
		ResultTrajs.at(i).ActualHotness/=ResultTrajs.at(i).NodeIDList.size();
	}
	sort(ResultTrajs.begin(), ResultTrajs.end(), compareByActualHotness);
	//���ǰk���켣
	ofstream fout("ʵ���ȶ�_"+fileName+".txt");
	for(int i=0;i<ResultTrajs.size();i++){
		fout<<ResultTrajs.at(i).ActualHotness<<endl;
	}
	fout.close();
}

//����DTWD
//��������������������������
double Distance(Node &node1,Node &node2){
	double deltaLat=node1.y-node2.y;
	double deltaLong=(node2.x-node1.x)*cos(node1.y*M_PI/180);
	return 111226.29021121707545*sqrt(deltaLat*deltaLat+deltaLong*deltaLong);
}
//Ԥ��������������������
void CalculateDistance(){
	Distances=vector<vector<double>>(NumOfNodes);
	for(int i=0;i<NumOfNodes;i++){
		Distances.at(i)=vector<double>(NumOfNodes);
		for(int j=0;j<NumOfNodes;j++){
			if(i==j){
				Distances[i][j]=0;
			}
			else{
				Distances[i][j]=Distance(NodeSet[i],NodeSet[j]);
			}
		}
	}
}
//��̬�滮����DTWD
double DTWDbyDP(vector<int> &traj1,vector<int> &traj2){
	vector<vector<double>> matrix = vector<vector<double>>(traj1.size()+1);
	for(int i = 0;i<traj1.size()+1;i++){
		matrix.at(i) = vector<double>(traj2.size()+1);
		for(int j = 0;j<traj2.size()+1;j++){
			matrix.at(i).at(j) = 1000000;
		}
	}
	matrix.at(0).at(0)=0;
	for(int i=1;i<traj1.size()+1;i++){
		for(int j=1;j<traj2.size()+1;j++){
			matrix.at(i).at(j)=(Distances[traj1.at(i-1)][traj2.at(j-1)]+min(matrix.at(i-1).at(j),min(matrix.at(i-1).at(j-1),matrix.at(i).at(j-1))));
			//matrix.at(i).at(j)/=i;
		}
	}
	return matrix.at(traj1.size()).at(traj2.size());
}

void main()
{
	clock_t start, finish;
	double totaltime;
	start = clock();

	ReadInNode();
	ReadInEdge();
	ReadInTraj();
	ReadInEdgePopularity();
	CalculateShortestTime();

	//finish = clock();
	//totaltime = (double)(finish - start) / CLOCKS_PER_SEC;
	//cout << "��ȡ�ļ�ʱ�䣺" << totaltime << "�룡" << endl;

	//��ʱ���룬����������Eage�ļ������ӻ�
	//ofstream fout("RoadNetworkVisualization.js");
	//fout << "data = [" << endl;
	//cout.precision(11);
	//for (int i = 0; i < EdgeList.size(); i++){
	//	fout << "{ startID:" << EdgeList[i].StartNodeID << ", startX:" << setprecision(11) << NodeSet[EdgeList[i].StartNodeID].x << ", startY:" << setprecision(11) << NodeSet[EdgeList[i].StartNodeID].y << ", endID:" << EdgeList[i].EndNodeID << ", endX:" << setprecision(11) << NodeSet[EdgeList[i].EndNodeID].x << ", endY:" << setprecision(11)<< NodeSet[EdgeList[i].EndNodeID].y << ", length:" << EdgeList[i].length << "}," << endl;
	//}
	//fout << "]";
	//fout.close();
	string queryFileNames[]={"queryFromHottest(1)"};

	for(int fileIndex=0;fileIndex<1;fileIndex++){
		//�Ա�ʵ��
		//ofstream foutForQueryTime("CompareExperimentQueryTimeFor"+queryFileNames[fileIndex]+".txt");
		//ʵ��
		ofstream foutForQueryTime("QueryTime_"+queryFileNames[fileIndex]+".txt");
		QuerySet.clear();
		ReadInQuerySet(queryFileNames[fileIndex]);
		for (int i = 0; i < QuerySet.size(); i++)
		{
			for (int j = 0; j < NumOfNodes; j++)
			{
				NodeSet[i].StartTime = NodeSet[j].Balance = NodeSet[j].Gvalue = NodeSet[j].Hvalue = NodeSet[j].parentNodeID = -1;
				NodeSet[j].NumOfParentNodes = 0;
			}
			StartNodeID = QuerySet[i].StartNodeID;
			NodeSet[StartNodeID].StartTime = QuerySet[i].startTime;
			DestinationNodeID = QuerySet[i].EndNodeID;
			//DeltaT = QuerySet[i].passTime*1.2;
			DeltaT=graph[StartNodeID][DestinationNodeID].shortestTime*2;

			//�����õı������
			//totalTimeForAssumeUpdateTrajSet = totalTimeForCalculateH = totalTimeForUpdateTrajSet = 0;
			//totalCountForAssumeUpdateTrajSet = totalCountForCalculateH = totalCountForUpdateTrajSet = 0;
			start = clock();

			if(AStar(i)){
				finish = clock();

				//OutputPathToJSFile(queryIndex, destination);
				OutputPathToTxtFile(i,queryFileNames[fileIndex]);
			}

			totaltime = (double)(finish - start) / CLOCKS_PER_SEC;
			foutForQueryTime<<totaltime<<endl;
			cout << "A*�㷨ʱ�䣺" << totaltime << "�룡" << endl;
			//cout << "����AssumeUpdateTrajSet��ʱ" << totalTimeForAssumeUpdateTrajSet << "��,������" << totalCountForAssumeUpdateTrajSet << "��" << endl;
			//cout << "����CalculateH��ʱ" << totalTimeForCalculateH << "��,������" << totalCountForCalculateH << "��" << endl;
			//cout<<"����UpdateTrajSet��ʱ"<<totalTimeForUpdateTrajSet<<"��,������"<<totalCountForUpdateTrajSet<<"��"<<endl;
			cout << "��" << i + 1 << "����ѯ������" << endl << endl;
		}
		foutForQueryTime.close();

		//ʵ��������
		for(int fileIndex=0;fileIndex<1;fileIndex++){
			ResultTrajs.clear();
			CompareTrajs.clear();
			ReadInResultTrajs("Result_"+queryFileNames[fileIndex]);
			//����ʵ���ȶ�
			GetActualHotness(queryFileNames[fileIndex]);
			//����DTWD
			ofstream foutForNDTW("NDTW_"+queryFileNames[fileIndex]+".txt");
			CalculateDistance();
			for(int i=0;i<ResultTrajs.size();i++){
				//cout<<DTWD(resultTrajs.at(i).trajNodes,compareTrajs.at(i).trajNodes,resultTrajs.at(i).trajNodes.size()-1,compareTrajs.at(i).trajNodes.size()-1)<<endl;
				foutForNDTW<<DTWDbyDP(ResultTrajs.at(i).NodeIDList,CompareTrajs.at(i).TrajNodes)/ResultTrajs.at(i).NodeIDList.size()<<endl;
			}
			foutForNDTW.close();
		}
	}
}