//关于轨迹类定义的头文件
#include "basic.h"
using namespace std;
class Trajectory
{
public:
	int trajId;//轨迹在TrajSet中的编号
	int startPoint;//该轨迹序列的第一个点的编号
	int endPoint;//该轨迹序列最后一个点的编号
	vector<int> cluSeq;//GPS点序列转化成的cluster序列
	Trajectory();
	void creatTraj(char *);
};
Trajectory::Trajectory()
{
}
void Trajectory::creatTraj(char *s)
{
	sscanf(s,"%d %d %d",
						&trajId,
						&startPoint,
						&endPoint
						);
}