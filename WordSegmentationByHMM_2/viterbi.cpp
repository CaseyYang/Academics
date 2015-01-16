#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stack>
#include "hmm.h"
#include "db.h"

using namespace std;

HMM hmm("Pi.mat", "A1.mat", "A2.mat", "B1.mat", "B2.mat");	//初始化HMM模型
DB db("db.txt");			//初始化字典


/*
* Viterbi算法进行分词，二阶马尔柯夫过程
*/
string viterbiTwo(string str_in){

	//计算输入句子中的汉字个数
	int row = str_in.size() / 3;
	string str_out = "";

	//如果输入字符串为空，则直接返回空
	if (row == 0){
		return str_out;
	}
	//如果只有一个字的话，则直接输出即可
	if (row < 2){
		str_out = str_in + "/";
		return str_out;
	}

	//分配矩阵空间
	double **delta = new double *[row];
	int **path = new int *[row];
	for (int i = 0; i < row; i++){
		delta[i] = new double[N]();
		path[i] = new int[N]();
	}

	//中间变量
	string cchar = "";	//存放汉字
	int min_path = -1;
	double val = 0.0;
	double min_val = 0.0;

	//初始化矩阵,给delta和path矩阵的第一行赋初值
	cchar = str_in.substr(0, 3);
	int cchar_num = db.getObservIndex(cchar);
	for (int i = 0; i < N; i++){
		delta[0][i] = hmm.Pi[i] + hmm.B1[i][cchar_num];	//对数
		path[0][i] = -1;
	}

	//给delta和path的后续行赋值（对数）
	for (int t = 1; t < row; t++){
		cchar = str_in.substr(3 * t, 3);
		cchar_num = db.getObservIndex(cchar);
		for (int j = 0; j < N; j++){
			min_val = 100000.0;
			min_path = -1;
			for (int i = 0; i < N; i++){
				val = delta[t - 1][i] + hmm.A1[i][j];
				if (val < min_val){
					min_val = val;
					min_path = i;
				}
			}

			delta[t][j] = min_val + hmm.B1[j][cchar_num];
			path[t][j] = min_path;
		}
	}

	//找delta矩阵最后一行的最大值
	min_val = 100000.0;
	min_path = -1;
	for (int i = 0; i < N; i++){
		if (delta[row - 1][i] < min_val){
			min_val = delta[row - 1][i];
			min_path = i;
		}
	}

	//从min_path出发,回溯得到最可能的路径
	stack<int> path_st;
	path_st.push(min_path);
	for (int i = row - 1; i > 0; i--){
		min_path = path[i][min_path];
		path_st.push(min_path);
	}

	//释放二维数组
	for (int i = 0; i < row; i++){
		delete[]delta[i];
		delete[]path[i];
	}
	delete[]delta;
	delete[]path;

	//根据标记好的状态序列分词
	int pos = 0;
	int index = -1;
	while (!path_st.empty()){
		index = path_st.top();
		path_st.pop();
		str_out.insert(str_out.size(), str_in, pos, 3);
		if (index == 2 || index == 3){
			//状态为E或S
			str_out.append("/");
		}
		pos += 3;
	}
}




/*
* Viterbi算法进行分词：三阶马尔柯夫过程
*/
string viterbiThree(string str_in){

	//计算输入句子中的汉字个数
	int row = str_in.size() / 3;
	string str_out = "";

	//如果输入字符串为空，则直接返回空
	if (row == 0){
		return str_out;
	}
	//如果只有一个字的话，则直接输出即可
	if (row < 2){
		str_out = str_in + "/";
		return str_out;
	}

	//分配矩阵空间
	double ***delta = new double **[row];
	int ***path = new int **[row];
	for (int i = 0; i < row; i++){
		delta[i] = new double *[N];
		path[i] = new int *[N];
		for (int j = 0; j < N; j++){
			delta[i][j] = new double[N];
			path[i][j] = new int[N];
			for (int k = 0; k < N; k++){
				delta[i][j][k] = 0.0;
				path[i][j][k] = 0;
			}
		}
	}

	//初始化矩阵,给delta和path矩阵的第1个面赋初值
	//初始状态需要两个面，第0面不赋值，只给第1个面赋值
	string cchar_1 = str_in.substr(0, 3);	//第1个字
	string cchar_2 = str_in.substr(3, 3);	//第2个字
	int num_1 = db.getObservIndex(cchar_1);	//第1个字的编号
	int num_2 = db.getObservIndex(cchar_2);	//第2个字的编号
	for (int i = 0; i < N; i++){
		for (int j = 0; j < N; j++){
			delta[1][i][j] = hmm.Pi[i] + hmm.B1[i][num_1] +
				hmm.A1[i][j] + hmm.B2[i][j][num_2];	//对数
			path[1][i][j] = -1;
		}
	}

	//中间变量
	string cchar_3 = "";	//存放汉字
	int min_path = -1;
	double val = 0.0;
	double min_val = 0.0;

	//给delta和path的后续面赋值（对数）
	//第0、1面为初始面，后续面从2开始，到row-1为止
	for (int t = 2; t < row; t++){
		cchar_3 = str_in.substr(3 * t, 3);
		int num_3 = db.getObservIndex(cchar_3);
		for (int j = 0; j < N; j++){
			for (int k = 0; k < N; k++){
				min_val = 100000.0;
				min_path = -1;
				for (int i = 0; i < N; i++){
					val = delta[t - 1][i][j] + hmm.A2[i][j][k];
					if (val < min_val){
						min_val = val;
						min_path = i;
					}
				}
				delta[t][j][k] = min_val + hmm.B2[j][k][num_3];
				path[t][j][k] = min_path;
			}
		}
	}

	//找delta矩阵最后一个面的最大值，最后一个面为row-1
	min_val = 100000.0;
	int min_path_i = -1;
	int min_path_j = -1;
	for (int i = 0; i < N; i++){
		for (int j = 0; j < N; j++){
			if (delta[row - 1][i][j] < min_val){
				min_val = delta[row - 1][i][j];
				min_path_i = i;
				min_path_j = j;
			}
		}
	}

	//从min_path_i和min_path_j出发,回溯得到最可能的路径
	//回溯从row-1开始，到2为止
	stack<int> path_st;
	path_st.push(min_path_j);
	path_st.push(min_path_i);
	for (int t = row - 1; t > 1; t--){
		int min_path_k = path[t][min_path_i][min_path_j];
		path_st.push(min_path_k);
		min_path_j = min_path_i;
		min_path_i = min_path_k;
	}

	//释放三维数组
	for (int i = 0; i < row; i++){
		for (int j = 0; j < N; j++){
			delete[]delta[i][j];
			delete[]path[i][j];
		}
		delete[]delta[i];
		delete[]path[i];
	}
	delete[]delta;
	delete[]path;

	//根据标记好的状态序列分词
	int pos = 0;
	int index = -1;
	while (!path_st.empty()){
		index = path_st.top();
		path_st.pop();
		str_out.insert(str_out.size(), str_in, pos, 3);
		if (index == 2 || index == 3){
			//状态为E或S
			str_out.append("/");
		}
		pos += 3;
	}
}

