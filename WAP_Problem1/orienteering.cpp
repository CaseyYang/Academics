#include<iostream>
#include<fstream>//only for debug
#include<vector>
#include<algorithm>
using namespace std;

#define MAX_DIST 1000000000

class MazeNode;
class Orienteering;

int dis[1200000][21];
int mazeWidth, mazeHeight;
MazeNode** maze = NULL;
vector<MazeNode*> checkPoints = vector<MazeNode*>();;
MazeNode* start = NULL, *goal = NULL;
bool validFlag = true;

class MazeNode{
public:
	char objectType;
	int row;
	int col;
	int dist;
	int predictDist;
	bool openListFlag;
	bool closedListFlag;
	MazeNode* parentNode;
	MazeNode() {}
	void Init(char objectType, int row, int col){
		this->objectType = objectType;
		this->row = row;
		this->col = col;
		predictDist = dist = MAX_DIST;
		closedListFlag = openListFlag = false;
		parentNode = NULL;
	}
	void Clear(){
		predictDist = dist = MAX_DIST;
		closedListFlag = openListFlag = false;
		parentNode = NULL;
	}
	void PushOpenList(vector<MazeNode*> &openList){
		openList.push_back(this);
		openListFlag = true;
	}
	void PushClosedList(vector<MazeNode*> &closedList){
		closedList.push_back(this);
		closedListFlag = true;
	}
	void UpdateParentNodeAndDist(MazeNode* parentNode){
		this->parentNode = parentNode;
		this->dist = parentNode->dist + 1;
	}
	void UpdatePredictDist(MazeNode* goal){
		predictDist = abs(goal->row - this->row) + abs(goal->col - this->col);
	}
	void ToString(){
		cout << "(" << row << "," << col << ") dist=" << dist << "; predictDist=" << predictDist << "; openListFlag=" << openListFlag << "; closedListFlag=" << closedListFlag << endl;
	}
};

class Orienteering{
public:
	void main();
};

void ReadIn(){
	cin >> mazeWidth >> mazeHeight;
	maze = new MazeNode*[mazeHeight];
	for (int row = 0; row < mazeHeight; ++row){
		maze[row] = new MazeNode[mazeWidth];
		for (int col = 0; col < mazeWidth; ++col){
			char objectType;
			cin >> objectType;
			maze[row][col].Init(objectType, row, col);
			if (objectType == '@'){
				checkPoints.push_back(&maze[row][col]);
			}
			if (objectType == 'S'){
				start = &maze[row][col];
			}
			if (objectType == 'G'){
				goal = &maze[row][col];
			}
		}
	}
}

void ReadInFromFile(){
	ifstream fin("input.txt");
	fin >> mazeWidth >> mazeHeight;
	maze = new MazeNode*[mazeHeight];
	for (int row = 0; row < mazeHeight; ++row){
		maze[row] = new MazeNode[mazeWidth];
		for (int col = 0; col < mazeWidth; ++col){
			char objectType;
			fin >> objectType;
			maze[row][col].Init(objectType, row, col);
			if (objectType == '@'){
				checkPoints.push_back(&maze[row][col]);
			}
			if (objectType == 'S'){
				start = &maze[row][col];
			}
			if (objectType == 'G'){
				goal = &maze[row][col];
			}
		}
	}
	fin.close();
}

bool Compare(const MazeNode* n1, const MazeNode * n2){
	return (n1->dist + n1->predictDist) > (n2->dist + n2->predictDist) ? true : false;
}

MazeNode* GetNeighborNode(MazeNode* curNode, int i){
	MazeNode* neighborNode = NULL;
	switch (i){
	case 0:
		if (curNode->col - 1 >= 0){
			neighborNode = &maze[curNode->row][curNode->col - 1];
		}
		break;
	case 1:
		if (curNode->col + 1 < mazeWidth){
			neighborNode = &maze[curNode->row][curNode->col + 1];
		}
		break;
	case 2:
		if (curNode->row - 1 >= 0){
			neighborNode = &maze[curNode->row - 1][curNode->col];
		}
		break;
	case 3:
		if (curNode->row + 1 < mazeHeight){
			neighborNode = &maze[curNode->row + 1][curNode->col];
		}
		break;
	}
	return neighborNode;
}

int ShortestPath(int** neighborTable, int size, int start, int goal){
	for (int i = 0; i < (1 << size); ++i){
		for (int j = 0; j < size; ++j)  dis[i][j] = MAX_DIST;
	}
	dis[1 << start][start] = 0;
	for (int i = 1; i < (1 << size); ++i) {
		for (int j = 0; j < size; ++j) {
			if (i&(1 << j)){
				for (int k = 0; k < size; ++k) {
					if (j != k && ((i&(1 << k)) == 0)) {
						if (dis[i ^ (1 << k)][k] > dis[i][j] + neighborTable[j][k]){
							dis[i ^ (1 << k)][k] = dis[i][j] + neighborTable[j][k];
						}
					}
				}
			}
		}
	}
	return dis[(1 << size) - 1][goal];
}

int AStar(MazeNode *start, MazeNode *goal){
	vector<MazeNode*> usedNodes = vector<MazeNode*>();
	start->dist = start->predictDist = 0;
	vector<MazeNode*> openList = vector<MazeNode*>();
	vector<MazeNode*> closedList = vector<MazeNode*>();
	start->PushOpenList(openList);
	usedNodes.push_back(start);
	MazeNode* curNode;
	do{
		make_heap(openList.begin(), openList.end(), Compare);
		pop_heap(openList.begin(), openList.end(), Compare);
		curNode = openList.back();
		openList.pop_back();
		curNode->openListFlag = false;
		curNode->PushClosedList(closedList);
		for (int i = 0; i < 4; i++){
			MazeNode* neighborNode = GetNeighborNode(curNode, i);
			if (neighborNode == NULL || neighborNode->objectType == '#' || neighborNode->closedListFlag){
				continue;
			}
			else
			{
				if (neighborNode->openListFlag == false){
					neighborNode->PushOpenList(openList);
					usedNodes.push_back(neighborNode);
					neighborNode->UpdateParentNodeAndDist(curNode);
					if (neighborNode->predictDist == MAX_DIST){
						neighborNode->UpdatePredictDist(goal);
					}
				}
				else{
					if (curNode->dist + 1 < neighborNode->dist){
						neighborNode->UpdateParentNodeAndDist(curNode);
					}
				}
			}
		}
	} while (!goal->closedListFlag && !openList.empty());
	if (goal->closedListFlag){
		int resultCount = 0;
		for (curNode = goal; curNode != start; curNode = curNode->parentNode){
			++resultCount;
		}
		for (int i = 0; i < usedNodes.size(); ++i){
			usedNodes[i]->Clear();
		}
		return resultCount;
	}
	else{
		return -1;
	}
}

void Orienteering::main(){
	validFlag = true;
	//input the maze
	ReadIn();
	if (mazeHeight < 1 || mazeHeight>100 || mazeWidth < 1 || mazeHeight>100 || start == NULL || goal == NULL || checkPoints.size() > 18){
		validFlag = false;
	}
	//shortest path among checkpoints
	int** neighborTable = new int*[checkPoints.size() + 2];
	neighborTable[0] = new int[checkPoints.size() + 2];
	neighborTable[checkPoints.size() + 1] = new int[checkPoints.size() + 2];
	for (int j = 0; j < checkPoints.size() + 2; ++j){
		neighborTable[0][j] = neighborTable[checkPoints.size() + 1][j] = 0;
	}
	for (int i = 0; i < checkPoints.size(); ++i){
		neighborTable[i + 1] = new int[checkPoints.size() + 2];
		for (int j = 0; j < checkPoints.size() + 2; ++j){
			neighborTable[i + 1][j] = 0;
		}
	}
	for (int i = 0; i < checkPoints.size(); ++i){
		for (int j = i + 1; j < checkPoints.size(); ++j){
			int dist = AStar(checkPoints[i], checkPoints[j]);
			if (dist == -1){
				validFlag = false;
				return;
			}
			else{
				neighborTable[j + 1][i + 1] = neighborTable[i + 1][j + 1] = dist;
			}
		}
	}
	//shortest path between each checkpoint and start
	for (int i = 0; i < checkPoints.size(); ++i){
		int dist = AStar(start, checkPoints[i]);
		if (dist == -1){
			validFlag = false;
			return;
		}
		else{
			neighborTable[i + 1][0] = neighborTable[0][i + 1] = dist;
		}
	}
	//shortest path between each checkpoint and goal
	for (int i = 0; i < checkPoints.size(); ++i){
		int dist = AStar(checkPoints[i], goal);
		if (dist == -1){
			validFlag = false;
			return;
		}
		else{
			neighborTable[checkPoints.size() + 1][i + 1] = neighborTable[i + 1][checkPoints.size() + 1] = dist;
		}
	}
	//shortest path between start and goal
	int dist = AStar(start, goal);
	if (dist == -1){
		validFlag = false;
		return;
	}
	else{
		neighborTable[0][checkPoints.size() + 1] = neighborTable[checkPoints.size() + 1][0] = dist;
	}
	//cout << "所有最短路计算完成" << endl;//only for debug
	//OutputCheckPoints(checkPoints);//only for debug
	//OutputNeighborTable(neighborTable, checkPoints.size() + 2);//only for debug
	//shortest path in the neighbor table
	int routeCount = ShortestPath(neighborTable, checkPoints.size() + 2, 0, checkPoints.size() + 1);
	cout << routeCount;
	return;
}

//void OutputCheckPoints(vector<MazeNode*> checkPoints){
//	for each (auto var in checkPoints)
//	{
//		var->ToString();
//	}
//}

//void OutputNeighborTable(int** neighborTable, int size){
//	for (int i = 0; i < size; ++i){
//		for (int j = 0; j < size; ++j){
//			cout << neighborTable[i][j] << " ";
//		}
//		cout << endl;
//	}
//	cout << endl;
//}


int main(int argc, char* argv[]){
	Orienteering o;
	o.main();
	return validFlag ? 0 : -1;
}