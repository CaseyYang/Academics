#ifndef HMM_H
#define HMM_H

#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>

const int N = 4;
const int M = 4677;

using namespace std;

//定义HMM模型
class HMM{

public:
	int n;			//状态数目
	int m;			//可能的观察符号数目
	double Pi[N];		//初始状态概率
	double A1[N][N];	//状态转移概率矩阵
	double A2[N][N][N];	//状态转移概率矩阵
	double B1[N][M];	//符号发射概率矩阵
	double B2[N][N][M];	//符号发射概率矩阵
	HMM();
	HMM(string f_pi, string f_a1, string f_a2, string f_b1, string f_b2);
};

//无参构造函数
HMM::HMM(){

}

//有参构造函数
HMM::HMM(string f_pi, string f_a1, string f_a2, string f_b1, string f_b2){
	ifstream fin_1(f_pi.c_str());
	ifstream fin_2(f_a1.c_str());
	ifstream fin_3(f_a2.c_str());
	ifstream fin_4(f_b1.c_str());
	ifstream fin_5(f_b2.c_str());
	if (!(fin_1 && fin_2 && fin_3 && fin_4 && fin_5)){
		exit(-1);
	}

	n = N;
	m = M;

	string line = "";
	string word = "";

	//读取Pi
	getline(fin_1, line);
	istringstream strstm_1(line);
	for (int i = 0; i < N; i++){
		strstm_1 >> word;
		Pi[i] = atof(word.c_str());
	}

	//读取A1
	for (int i = 0; i < N; i++){
		getline(fin_2, line);
		istringstream strstm_2(line);
		for (int j = 0; j < N; j++){
			strstm_2 >> word;
			A1[i][j] = atof(word.c_str());
		}
	}

	//读取A2	
	for (int i = 0; i < N; i++){
		for (int j = 0; j < N; j++){
			getline(fin_3, line);
			istringstream strstm_3(line);
			for (int k = 0; k < N; k++){
				strstm_3 >> word;
				A2[i][j][k] = atof(word.c_str());
			}
		}
	}

	//读取B1
	for (int i = 0; i < N; i++){
		getline(fin_4, line);
		istringstream strstm_4(line);
		for (int j = 0; j < M; j++){
			strstm_4 >> word;
			B1[i][j] = atof(word.c_str());
		}
	}

	//读取B2
	for (int i = 0; i < N; i++){
		for (int j = 0; j < N; j++){
			getline(fin_5, line);
			istringstream strstm_5(line);
			for (int k = 0; k < M; k++){
				strstm_5 >> word;
				B2[i][j][k] = atof(word.c_str());
			}
		}
	}

	fin_1.close();
	fin_2.close();
	fin_3.close();
	fin_4.close();
	fin_5.close();
}

#endif