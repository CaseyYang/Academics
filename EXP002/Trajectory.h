//���ڹ켣�ඨ���ͷ�ļ�
#include "basic.h"
using namespace std;
class Trajectory
{
public:
	int trajId;//�켣��TrajSet�еı��
	int startPoint;//�ù켣���еĵ�һ����ı��
	int endPoint;//�ù켣�������һ����ı��
	vector<int> cluSeq;//GPS������ת���ɵ�cluster����
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