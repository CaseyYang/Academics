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

const int MaxLength = 100000;//表示不连通

//动态热度
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
//交叉口
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
//路段
class Edge{
public:
	int EdgeID;
	int StartNodeID;
	int EndNodeID;
	double length;
	double Popularity;
	double shortestTime;
	vector<HotInterval> HotIntervalList;//路段的动态热度数据
	set<int> TrajSetContainedThisEdge;//为加快程序运行速度，预先记录有哪些轨迹通过该条边
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
//真实轨迹
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
		//	cout<<"轨迹"<<TrajID<<"第"<<this->TrajRoutes.size()<<"条边和之后一条边不相连！"<<endl;
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
//查询
class Query
{
public:
	int TrajID;//轨迹ID（序号）
	int StartNodeID;//共同的起点
	int EndNodeID;//共同的终点
	//double passTime;//这类轨迹所对应的DeltaT值
	double startTime;//查询开始时间
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
//实验轨迹
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
			cout<<"见鬼了！ResultTraj中NodeIDList和TimeStampList数量不一致！"<<endl;
			return false;
		}
	}
};


//全局参数
int NumOfNodes = 0;//点个数
int NumOfEdges = 0;//边个数
int NumOfTrajs = 0;//轨迹条数
int NumOfQueries = 0;//查询条目个数
int NumofResultTrajs=0;//实验结果轨迹条数
map<int, Node> NodeSet;//键值对<点ID，点>
vector<Edge> EdgeList;//边集合
vector<Traj> TrajList;//轨迹集合
vector<vector<Edge>> graph;//路网
vector<Query>QuerySet;//查询点对集合
vector<ResultTraj>ResultTrajs;//实验结果轨迹集合
vector<vector<double>> Distances;//任意两路口间球面距离
vector<Traj> CompareTrajs;//计算DTWD时和实验结果轨迹比较的原始轨迹的子集
const double Speed = 20;//速度（单位是km/h）
double DeltaT;//Delta_T
int StartNodeID;//起点
int DestinationNodeID;//终点

//调试用：记录各函数执行时间
double totalTimeForCalculateH;
int totalCountForCalculateH;
double totalTimeForUpdateTrajSet;
int totalCountForUpdateTrajSet;
double totalTimeForAssumeUpdateTrajSet;
int totalCountForAssumeUpdateTrajSet;


//读入点信息
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
		cout << "点信息文件出错！" << endl;
	}
	//cout << "点信息：" << setprecision(11) << NodeSet[nodeId].x << " " << setprecision(11)<< NodeSet[nodeId].y << endl;
	reader.close();
}
//读入边信息
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
				cout << "边信息文件出错！" << endl;
				return;
			}
		}
	}
	//cout << "边信息：" << EdgeList.back().StartNodeID << " " << EdgeList.back().EndNodeID <<" "<<setprecision(11)<<EdgeList.back().length<<endl;
	reader.close();
}
//读入边的热度信息
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
					cout<<"热度信息对应的轨迹不存在！"<<endl;
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
		cout << "边热度信息文件出错！" << endl;
	}
	//cout << "热度信息：" << EdgeList.back().StartNodeID << " " << EdgeList.back().EndNodeID<< " " << EdgeList.back().HotIntervalList.back().StartTimeStamp << " " << EdgeList.back().HotIntervalList.back().Popularity << " " << EdgeList.back().HotIntervalList.back().EndTimeStamp << endl;
	reader.close();
}
//读入轨迹信息
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
					//	cout<<"因为真实轨迹所经过的边在路网上并不存在，使得计算热度时会存在相当于真实数据量减少/增加的问题"<<endl;
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
		cout << "轨迹文件出错！";
		return;
	}
	reader.close();
	//cout << "轨迹信息：" << (graph[451][442].TrajSetContainedThisEdge.find(--count) != graph[451][442].TrajSetContainedThisEdge.end()) << endl;
}
//读入查询点集
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
		cout << "查询点集文件出错！";
		return;
	}
	reader.close();
	//cout << "查询信息：" << QuerySet.back().trajId<<" "<<QuerySet.back().StartNodeID << " " << QuerySet.back().EndNodeID << " " << QuerySet.back().startTime << " " << QuerySet.back().passTime << " " << endl;
}

//预处理：计算任意两点间的最短时间
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

//辅助函数：得到给定点i的H值
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
				//静态热度
				//result+=EdgeList[j].Popularity;

				//动态热度
				double theEaliestArrvialTime = NodeSet[i].StartTime + formTime;
				double theLatestDepartureTime = NodeSet[StartNodeID].StartTime + DeltaT - nextTime;
				//根据2014年1月10日邮件更改
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
//辅助函数：假设把给定边加入现有序列，为了计算一个可能的G值
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
//辅助函数：根据2014年1月5日的邮件重写计算G值的函数
double CalculateG(int parentNodeID,int currentNodeID){
	Edge& currentEdge=graph[parentNodeID][currentNodeID];
	double currentPopularity=0;
	double startTime=NodeSet[parentNodeID].StartTime;
	//找到相适应时段的热度
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
//辅助函数：对openList进行按F=G+H进行降序排序
bool compare(const int & m1, const int & m2) {
	return (NodeSet[m1].Gvalue + NodeSet[m1].Hvalue) < (NodeSet[m2].Gvalue + NodeSet[m2].Hvalue);
}
//辅助函数：输出路径
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
//辅助函数：输出路径至js文件
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

//最短路
bool NormalAStar(int startNodeID,int destination){
	int currentNodeID=startNodeID;
	if(graph[currentNodeID][destination].shortestTime<MaxLength){//确定有解
		while(currentNodeID!=destination){//终止条件
			cout<<currentNodeID<<endl;
			for(int i=0;i<NumOfNodes;i++){
				if(0 < graph[currentNodeID][i].length&&graph[currentNodeID][i].length<MaxLength){//currentNodeID和i连通
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
//A*算法
bool AStar(int queryIndex)
{
	//感觉startNodeID和destinationNodeID没有必要，与全局变量StartNodeID和DestinationsNodeID重复
	//int startNodeID = QuerySet[queryIndex].StartNodeID;
	//int destinationNodeID = QuerySet[queryIndex].EndNodeID;
	vector<int> openList;//候选集
	set<int> processedEdgeList;//记录哪些边已经被包含在查询结果中
	set<int> closedList;//不用再考虑的格子，即其
	set<int> concernedTrajSet;//理应对应每种候选结果而不是全局变量？
	//bool resultFromNormalAStar=false;//和传统A*算法有关，其他地方没用
	openList.push_back(StartNodeID);
	NodeSet[StartNodeID].Balance=DeltaT;
	int currentNodeID;
	//while(openList
	while (!openList.empty() && closedList.count(DestinationNodeID) != 1)//结束条件
	{
		currentNodeID = openList.back();
		openList.pop_back();
		closedList.insert(currentNodeID);
		//处理currentPoint
		if (NodeSet[currentNodeID].parentNodeID != -1)//如果parentNodeID为-1，则说明是起始点
		{
			processedEdgeList.insert(graph[NodeSet[currentNodeID].parentNodeID][currentNodeID].EdgeID);
			concernedTrajSet.insert(graph[currentNodeID][NodeSet[currentNodeID].parentNodeID].TrajSetContainedThisEdge.begin(), graph[currentNodeID][NodeSet[currentNodeID].parentNodeID].TrajSetContainedThisEdge.end());//最好也重新写
			//当初为何要在这里再次计算G值？
			//NodeSet[currentNodeID].Gvalue = (concernedTrajSet.size()+0.0) / processedEdgeList.size();//TODO: 最好重新写
			//NodeSet[currentNodeID].Balance = NodeSet[NodeSet[currentNodeID].parentNodeID].Balance - graph[NodeSet[currentNodeID].parentNodeID][currentNodeID].length / (Speed*1000);
			//if(NodeSet[currentPoint].Balance<0){
			//	cout<<"Balance小于零！"<<endl;
			//	resultFromNormalAStar=NormalAStar(NodeSet[currentPoint].parentNodeID,destination);
			//	cout<<resultFromNormalAStar<<endl;
			//	break;
			//}
		}
		for (int i = 0; i < NumOfNodes; i++)
		{
			if (closedList.count(i)==0 && 0<graph[currentNodeID][i].length && graph[currentNodeID][i].length<MaxLength)//点i和当前待扩展点之间存在边
			{
				double gOfCurrentCandidateNode = CalculateG(currentNodeID,i);//2014年1月5日提出的计算G值的方法
				//int size = AssumeUpdateTrajSet(graph[i][currentNodeID], concernedTrajSet);//2014年1月5日更改需求之前的G值，类似现在的gOfCurrentCandidateNode
				if (find(openList.begin(), openList.end(), i) != openList.end())//判断点i是否已存在于openList中
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
	//输出路径
	if (closedList.count(DestinationNodeID) == 1)
	{
		return true;
	}
	else
	{
		cout << "没有找到最短路径！" << endl;
		return false;
	}
}


//以下是实验结果处理部分

//读入实验结果轨迹
//读入查询结果轨迹信息
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
					cout<<"读入查询结果轨迹出错！第"<<indexofTraj<<"条轨迹Node和TimeStamp个数不匹配！"<<endl;
					return;
				}
			}
			indexofTraj++;
		}
	}
	else
	{
		cout<<"查询结果文件出错！";
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
//辅助函数：对TrajList按实际热度进行降序排序
bool compareByActualHotness(const ResultTraj & traj1, const ResultTraj & traj2) {
	return traj1.ActualHotness > traj2.ActualHotness;
}
//获得实际热度
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
	//输出前k条轨迹
	ofstream fout("实际热度_"+fileName+".txt");
	for(int i=0;i<ResultTrajs.size();i++){
		fout<<ResultTrajs.at(i).ActualHotness<<endl;
	}
	fout.close();
}

//计算DTWD
//辅助函数：计算两点间球面距离
double Distance(Node &node1,Node &node2){
	double deltaLat=node1.y-node2.y;
	double deltaLong=(node2.x-node1.x)*cos(node1.y*M_PI/180);
	return 111226.29021121707545*sqrt(deltaLat*deltaLat+deltaLong*deltaLong);
}
//预处理：计算任意两点间距离
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
//动态规划计算DTWD
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
	//cout << "读取文件时间：" << totaltime << "秒！" << endl;

	//临时代码，输出带坐标的Eage文件供可视化
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
		//对比实验
		//ofstream foutForQueryTime("CompareExperimentQueryTimeFor"+queryFileNames[fileIndex]+".txt");
		//实验
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

			//调试用的变量清空
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
			cout << "A*算法时间：" << totaltime << "秒！" << endl;
			//cout << "函数AssumeUpdateTrajSet用时" << totalTimeForAssumeUpdateTrajSet << "秒,调用了" << totalCountForAssumeUpdateTrajSet << "次" << endl;
			//cout << "函数CalculateH用时" << totalTimeForCalculateH << "秒,调用了" << totalCountForCalculateH << "次" << endl;
			//cout<<"函数UpdateTrajSet用时"<<totalTimeForUpdateTrajSet<<"秒,调用了"<<totalCountForUpdateTrajSet<<"次"<<endl;
			cout << "第" << i + 1 << "个查询结束！" << endl << endl;
		}
		foutForQueryTime.close();

		//实验结果处理
		for(int fileIndex=0;fileIndex<1;fileIndex++){
			ResultTrajs.clear();
			CompareTrajs.clear();
			ReadInResultTrajs("Result_"+queryFileNames[fileIndex]);
			//计算实际热度
			GetActualHotness(queryFileNames[fileIndex]);
			//计算DTWD
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