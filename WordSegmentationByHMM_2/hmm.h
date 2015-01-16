#ifndef HMM_H
#define HMM_H

#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>

const int N = 4;
const int M = 4677;

using namespace std;

//����HMMģ��
class HMM{

public:
	int n;			//״̬��Ŀ
	int m;			//���ܵĹ۲������Ŀ
	double Pi[N];		//��ʼ״̬����
	double A1[N][N];	//״̬ת�Ƹ��ʾ���
	double A2[N][N][N];	//״̬ת�Ƹ��ʾ���
	double B1[N][M];	//���ŷ�����ʾ���
	double B2[N][N][M];	//���ŷ�����ʾ���
	HMM();
	HMM(string f_pi, string f_a1, string f_a2, string f_b1, string f_b2);
};

//�޲ι��캯��
HMM::HMM(){

}

//�вι��캯��
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

	//��ȡPi
	getline(fin_1, line);
	istringstream strstm_1(line);
	for (int i = 0; i < N; i++){
		strstm_1 >> word;
		Pi[i] = atof(word.c_str());
	}

	//��ȡA1
	for (int i = 0; i < N; i++){
		getline(fin_2, line);
		istringstream strstm_2(line);
		for (int j = 0; j < N; j++){
			strstm_2 >> word;
			A1[i][j] = atof(word.c_str());
		}
	}

	//��ȡA2	
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

	//��ȡB1
	for (int i = 0; i < N; i++){
		getline(fin_4, line);
		istringstream strstm_4(line);
		for (int j = 0; j < M; j++){
			strstm_4 >> word;
			B1[i][j] = atof(word.c_str());
		}
	}

	//��ȡB2
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