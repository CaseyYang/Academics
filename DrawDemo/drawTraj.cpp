#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <time.h>
#include "MapDrawer.h"
#include <list>

#define eps 1e-8
using namespace std;

MapDrawer md;
double minLat = 1.22;
double maxLat = 1.5;
double minLon = 103.620;
double maxLon = 104.0;
int size = 3000; //图片分辨率


/************************************************************************/
/*根据轨迹文件名如"input_000011.txt"返回MM文件名"output_000011.txt"         */
/************************************************************************/
string getMMFileName(string fileName)
{
	string str = "000000";
	for (int i = 0; i < 6; i++)
		str[i] = fileName[i + 6];
	return "output_" + str + ".txt";
}

/************************************************************************/
/*读一条"input_000011.txt"(fileName), 同时读一条"output_000011.txt"        */
/************************************************************************/
void readOneTrajectory(std::string folderDir, std::string fileName)
{
	FILE *fpIn, *fpOut;
	double lat, lon,preLon,preLat;
	bool start = true;
	int time, roadId;
	int confidence;
	fpIn = fopen((folderDir + "//input//" + fileName).c_str(), "r");
	fpOut = fopen((folderDir + "//output//" + getMMFileName(fileName)).c_str(), "r");
	while (!feof(fpIn))
	{
		int flag = fscanf(fpIn, "%d,%lf,%lf", &time, &lat, &lon);
		fscanf(fpOut, "%d,%d,%d", &time, &roadId, &confidence);
		if (start){
			preLat = lat;
			preLon = lon;
			start = false;
		}
		//防止末行读入两遍
		if (flag == -1)
			break;
		if (roadId == -1) //map matching失败
		{
			md.drawBigPoint(Gdiplus::Color::Red, lat, lon);
			//md.drawLine(Gdiplus::Color::Red, preLat, preLon, lat, lon);
		}
		else //map matching成功
		{
			md.drawBigPoint(Gdiplus::Color::Green, lat, lon);
			//md.drawLine(Gdiplus::Color::Green, preLat, preLon, lat, lon);
		}
		preLat = lat;
		preLon = lon;

	}
	fclose(fpIn);
	fclose(fpOut);
}

/************************************************************************/
/* 遍历folderDir下的所有文件                                               */
/************************************************************************/
void scanTrajFolder(string folderDir)
{
	/*文件目录结构为
	* folderDir
	* |-input
	*   |-input_000011.txt ...
	* |-output
	*   |-output_000011.txt ...
	*/
	string tempS = folderDir + "\\input\\" + "*.txt";
	const char* dir = tempS.c_str();
	_finddata_t file;
	long lf;
	if ((lf = _findfirst(dir, &file)) == -1l)
		return;
	else
	{
		do
		{
			readOneTrajectory(folderDir, file.name);
		} while (_findnext(lf, &file) == 0);
		_findclose(lf);
		return;
	}
}

void main()
{
	int startTime = clock();
	string mapFilePath = "D:\\Document\\MDM Lab\\Data\\MapMatching数据\\WA_EdgeGeometry.txt";
	string trajDir = "D:\\Document\\MDM Lab\\Data\\MapMatching数据";
	md.setArea(minLat, maxLat, minLon, maxLon);
	md.setResolution(size);
	/*zooming part*/
	//size = 3000;
	//md.zoomIn(8900, 7150, 400, 300, size);
	//printf("minlat:%lf,maxlat:%lf,minlon:%lf,maxlon:%lf\n", md.minLat, md.maxLat, md.minLon, md.maxLon);
	/*zooming part end*/
	md.newBitmap();
	cout << "start drawing..." << endl;

	md.lockBits();
	md.drawMap(Gdiplus::Color::Blue, mapFilePath);
	scanTrajFolder(trajDir);
	md.unlockBits();

	md.saveBitmap("out.png");
	int endTime = clock();
	cout << "running time: " << (endTime - startTime) / 1000.0 << "s" << endl;
	system("pause");
}