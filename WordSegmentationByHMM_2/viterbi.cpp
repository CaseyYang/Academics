#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stack>
#include "hmm.h"
#include "db.h"

using namespace std;

HMM hmm("Pi.mat", "A1.mat", "A2.mat", "B1.mat", "B2.mat");	//��ʼ��HMMģ��
DB db("db.txt");			//��ʼ���ֵ�


/*
* Viterbi�㷨���зִʣ���������·����
*/
string viterbiTwo(string str_in){

	//������������еĺ��ָ���
	int row = str_in.size() / 3;
	string str_out = "";

	//��������ַ���Ϊ�գ���ֱ�ӷ��ؿ�
	if (row == 0){
		return str_out;
	}
	//���ֻ��һ���ֵĻ�����ֱ���������
	if (row < 2){
		str_out = str_in + "/";
		return str_out;
	}

	//�������ռ�
	double **delta = new double *[row];
	int **path = new int *[row];
	for (int i = 0; i < row; i++){
		delta[i] = new double[N]();
		path[i] = new int[N]();
	}

	//�м����
	string cchar = "";	//��ź���
	int min_path = -1;
	double val = 0.0;
	double min_val = 0.0;

	//��ʼ������,��delta��path����ĵ�һ�и���ֵ
	cchar = str_in.substr(0, 3);
	int cchar_num = db.getObservIndex(cchar);
	for (int i = 0; i < N; i++){
		delta[0][i] = hmm.Pi[i] + hmm.B1[i][cchar_num];	//����
		path[0][i] = -1;
	}

	//��delta��path�ĺ����и�ֵ��������
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

	//��delta�������һ�е����ֵ
	min_val = 100000.0;
	min_path = -1;
	for (int i = 0; i < N; i++){
		if (delta[row - 1][i] < min_val){
			min_val = delta[row - 1][i];
			min_path = i;
		}
	}

	//��min_path����,���ݵõ�����ܵ�·��
	stack<int> path_st;
	path_st.push(min_path);
	for (int i = row - 1; i > 0; i--){
		min_path = path[i][min_path];
		path_st.push(min_path);
	}

	//�ͷŶ�ά����
	for (int i = 0; i < row; i++){
		delete[]delta[i];
		delete[]path[i];
	}
	delete[]delta;
	delete[]path;

	//���ݱ�Ǻõ�״̬���зִ�
	int pos = 0;
	int index = -1;
	while (!path_st.empty()){
		index = path_st.top();
		path_st.pop();
		str_out.insert(str_out.size(), str_in, pos, 3);
		if (index == 2 || index == 3){
			//״̬ΪE��S
			str_out.append("/");
		}
		pos += 3;
	}
}




/*
* Viterbi�㷨���зִʣ���������·����
*/
string viterbiThree(string str_in){

	//������������еĺ��ָ���
	int row = str_in.size() / 3;
	string str_out = "";

	//��������ַ���Ϊ�գ���ֱ�ӷ��ؿ�
	if (row == 0){
		return str_out;
	}
	//���ֻ��һ���ֵĻ�����ֱ���������
	if (row < 2){
		str_out = str_in + "/";
		return str_out;
	}

	//�������ռ�
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

	//��ʼ������,��delta��path����ĵ�1���渳��ֵ
	//��ʼ״̬��Ҫ�����棬��0�治��ֵ��ֻ����1���渳ֵ
	string cchar_1 = str_in.substr(0, 3);	//��1����
	string cchar_2 = str_in.substr(3, 3);	//��2����
	int num_1 = db.getObservIndex(cchar_1);	//��1���ֵı��
	int num_2 = db.getObservIndex(cchar_2);	//��2���ֵı��
	for (int i = 0; i < N; i++){
		for (int j = 0; j < N; j++){
			delta[1][i][j] = hmm.Pi[i] + hmm.B1[i][num_1] +
				hmm.A1[i][j] + hmm.B2[i][j][num_2];	//����
			path[1][i][j] = -1;
		}
	}

	//�м����
	string cchar_3 = "";	//��ź���
	int min_path = -1;
	double val = 0.0;
	double min_val = 0.0;

	//��delta��path�ĺ����渳ֵ��������
	//��0��1��Ϊ��ʼ�棬�������2��ʼ����row-1Ϊֹ
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

	//��delta�������һ��������ֵ�����һ����Ϊrow-1
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

	//��min_path_i��min_path_j����,���ݵõ�����ܵ�·��
	//���ݴ�row-1��ʼ����2Ϊֹ
	stack<int> path_st;
	path_st.push(min_path_j);
	path_st.push(min_path_i);
	for (int t = row - 1; t > 1; t--){
		int min_path_k = path[t][min_path_i][min_path_j];
		path_st.push(min_path_k);
		min_path_j = min_path_i;
		min_path_i = min_path_k;
	}

	//�ͷ���ά����
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

	//���ݱ�Ǻõ�״̬���зִ�
	int pos = 0;
	int index = -1;
	while (!path_st.empty()){
		index = path_st.top();
		path_st.pop();
		str_out.insert(str_out.size(), str_in, pos, 3);
		if (index == 2 || index == 3){
			//״̬ΪE��S
			str_out.append("/");
		}
		pos += 3;
	}
}

