
#pragma once
/*����Point�ඨ���ͷ�ļ�,ͬʱ����getDistance()����
*/
#include <time.h>
#include <fstream>
using namespace std;

#define alpha	5
#define beta	2
#define R 185
#define S 200
#define PI180 0.0174532925199432957694
#define LENGTH_PER_RAD 111226.29021121707545

//��������������ľ�γ�����꣬������ŷ�Ͼ��룬��λΪ��
double getDistance(double lat1,double long1,double lat2,double long2)
{
	double deltaLat=lat1-lat2;
	double deltaLong=(long2-long1)*cos(lat1 * PI180);
	return LENGTH_PER_RAD*sqrt(deltaLat*deltaLat+deltaLong*deltaLong);
}

//Point��Ķ���
class Point
{
public:
	int pointId;//��ʾ��ȫ�ֱ�������PointSet�еı��
	int trajId;//�����Ĺ켣�ı��
	int cluId;//������cluster�ı��
	//time_t time;//��¼�ɼ�ʱ��
	struct tm time;
	double x,y;//x���ȣ�yγ��
	double movDir;//�õ�˿̵��˶����򣬱�ʾΪ��x��������ļнǣ���Χ��0��~360��
	bool classified;//�ж��Ƿ���expand�����м���
	//Point();
	Point(char *);
	double coh(Point p);//����õ�͵�*p��ֱ����ض�
};

//����һ���ַ���������һ��Point���˴��ַ�����ʵΪ���ļ������һ���ı�
Point::Point(char * s)
{
	//�ļ���ʽpointId,trajId,time,x,y,movDir
	sscanf(s,"%d %d %2d/%2d/%4d %2d:%2d:%2d %Lf %Lf %Lf",//����
						&pointId,
						&trajId,
						&time.tm_mday,
						&time.tm_mon,
						&time.tm_year,
						&time.tm_hour,
						&time.tm_min,
						&time.tm_sec,
						&x,
						&y,
						&movDir);
	//time.tm_year -= 1900;
    //time.tm_mon --;

    //time.tm_isdst=-1;
 
	cluId=-1;
	classified=false;

};

//�������������ضȣ����幫ʽ�����paper
double Point::coh(Point p)
{
	double distance=getDistance(x,y,p.x,p.y);
	double a=getDistance(x,y,p.x,p.y)/S;//�������Ͱ뾶�ı�ֵ
	double b=abs(sin(movDir-p.movDir));//sin(�ǶȲ�)�ľ���ֵ
	double c=-pow(a,alpha);
	double d=pow(b,beta);
	double temp= exp(c)*d;
	return temp;
}

