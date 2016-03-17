
#pragma once
/*关于Point类定义的头文件,同时包含getDistance()函数
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

//传入任意两个点的经纬度坐标，返回其欧氏距离，单位为米
double getDistance(double lat1,double long1,double lat2,double long2)
{
	double deltaLat=lat1-lat2;
	double deltaLong=(long2-long1)*cos(lat1 * PI180);
	return LENGTH_PER_RAD*sqrt(deltaLat*deltaLat+deltaLong*deltaLong);
}

//Point类的定义
class Point
{
public:
	int pointId;//标示在全局变量数组PointSet中的编号
	int trajId;//所属的轨迹的编号
	int cluId;//所属的cluster的编号
	//time_t time;//记录采集时间
	struct tm time;
	double x,y;//x经度，y纬度
	double movDir;//该点此刻的运动方向，表示为与x轴正方向的夹角，范围是0°~360°
	bool classified;//判断是否在expand过程中检查过
	//Point();
	Point(char *);
	double coh(Point p);//计算该点和点*p的直接相关度
};

//传入一行字符串，构造一个Point。此处字符串其实为从文件读入的一行文本
Point::Point(char * s)
{
	//文件格式pointId,trajId,time,x,y,movDir
	sscanf(s,"%d %d %2d/%2d/%4d %2d:%2d:%2d %Lf %Lf %Lf",//问题
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

//计算两个点的相关度，具体公式见相关paper
double Point::coh(Point p)
{
	double distance=getDistance(x,y,p.x,p.y);
	double a=getDistance(x,y,p.x,p.y)/S;//两点距离和半径的比值
	double b=abs(sin(movDir-p.movDir));//sin(角度差)的绝对值
	double c=-pow(a,alpha);
	double d=pow(b,beta);
	double temp= exp(c)*d;
	return temp;
}

