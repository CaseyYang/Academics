#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <cmath>
#include <list>
#include "db.h"

using namespace std;

const int N = 4;		//����״̬����Ŀ
const int M = 4677;		//���ֵĸ���
const double VALUE = 1.0;	//ƽ���㷨���ӵ�ֵ


//�����ֵ����
DB db("db.txt");


/*
* ģ��ѵ������Ƶ��ת��ΪƵ�ʣ���1ƽ����
*/
void turingAdd(const int count[], double prob[], int len){
	double sum = 0.0;
	for (int i = 0; i < len; ++i){
		sum += count[i];
	}

	sum = sum + VALUE * len;
	for (int i = 0; i < len; ++i){
		prob[i] = -log((count[i] + VALUE) / sum);//ȡ����
	}
}


/*
* ģ��ѵ����������Ƶ��ת��ΪƵ��(�ŵ�-ͼ��ƽ��)
*/
void turingGood(const int count[], double prob[], int len){
	map<int, list<int> > freq_map;			//keyΪ��Ƶ��valueΪ�ô�Ƶ��Ӧ�ĺ����б�
	map<int, list<int> >::iterator iter;		//������
	int sum = 0;					//��Ƶ�ܺ�

	//��ʼ��freq_map
	for (int i = 0; i < len; i++){
		int freq = count[i];			//��Ƶ
		sum += freq;

		iter = freq_map.find(freq);
		if (iter != freq_map.end()){
			//�ô�Ƶ�Ѿ����ڣ��ѵ�ǰ�ʼ�����Ӧ��list
			freq_map[freq].push_back(i);
		}
		else{
			//�ô�Ƶ�����ڣ�������Ӧ�ĺ���list
			list<int> lst;
			lst.push_back(i);
			freq_map[freq] = lst;
		}
	}

	//��sum=0��������ʼ��Ϊ0.0����
	if (sum == 0){
		for (int i = 0; i < len; i++){
			prob[i] = 0.0;
		}
		return;
	}

	//����ƽ������
	iter = freq_map.begin();
	while (iter != freq_map.end()){
		double pr;	//Ƶ��
		int freq = iter->first;
		int freqsize = iter->second.size();
		if (++iter != freq_map.end()){
			int freq_2 = iter->first;
			if (freq_2 = freq + 1){
				int freqsize_2 = iter->second.size();
				pr = ((1.0 + freq) * freqsize_2) / (sum * freqsize);
			}
			else{
				pr = 1.0 * freq / sum;
			}
		}
		else{
			pr = 1.0 * freq / sum;
		}

		//������
		list<int> lst = (--iter)->second;
		list<int>::iterator iter_in = lst.begin();
		while (iter_in != lst.end()){
			int index = *iter_in;
			prob[index] = pr;
			++iter_in;
		}

		//׼���´ε���
		++iter;
	}

	//���ʹ�һ��
	double total = 0.0;
	for (int i = 0; i < len; i++){
		total += prob[i];
	}
	for (int i = 0; i < len; i++){
		prob[i] = -log((double)prob[i] / total);//ȡ����
	}
}


/*
* ������������HMMģ�͵Ĳ���
* ״̬ת�Ƹ��ʾ��󡢳�ʼ״̬���ʾ��󡢷��ŷ�����ʾ���
*/
int main(int argc, char *argv[]){
	if (argc < 2){
		cout << "Usage: " << argv[0] << " bmes_file !" << endl;
		exit(-1);
	}

	ifstream fin(argv[1]);
	if (!fin){
		cerr << "Open input file " << argv[1] << "filed !" << endl;
		exit(-1);
	}

	int Pi[N] = { 0 };		//��ʼ״̬���ִ���
	int A1[N][N] = { 0 };		//����״̬ת�ƴ���
	int A2[N][N][N] = { 0 };		//����״̬ת�ƴ���
	int B1[N][M] = { 0 };		//���׷��ŷ������
	int B2[N][N][M] = { 0 };		//���׷��ŷ������

	//��ȡ�ļ��е�״̬�͹۲�ֵ
	string line = "";			//���ÿһ�е�����
	int line_num = 0;			//���ӱ��
	int count = 0;
	while (getline(fin, line)){
		line_num++;
		char state;			//״̬
		string cchar = "";		//һ������
		int i, j, k, m;
		string::size_type pos = 0;	//��ǰ����λ��
		if ((pos = line.find("/", pos + 1)) != string::npos){
			//��ȡ���ӵĵ�һ��״̬
			state = line.at(pos + 1);
			i = db.getStateIndex(state);
			Pi[i]++;
			//��ȡ���ӵĵ�һ���۲�ֵ
			cchar = line.substr(pos - 3, 3);
			m = db.getObservIndex(cchar);
			B1[i][m]++;

			if ((pos = line.find("/", pos + 1)) != string::npos){
				//��ȡ���ӵĵڶ���״̬
				state = line.at(pos + 1);
				j = db.getStateIndex(state);
				A1[i][j]++;
				//��ȡ���ӵĵڶ����۲�ֵ
				cchar = line.substr(pos - 3, 3);
				m = db.getObservIndex(cchar);
				B1[j][m]++;
				B2[i][j][m]++;

				while ((pos = line.find("/", pos + 1)) != string::npos){
					//��ȡ���ӵ�����״̬
					state = line.at(pos + 1);
					k = db.getStateIndex(state);
					A1[j][k]++;
					A2[i][j][k]++;
					//��ȡ���ӵ������۲�ֵ
					cchar = line.substr(pos - 3, 3);
					m = db.getObservIndex(cchar);
					B1[k][m]++;
					B2[j][k][m]++;

					//׼���´ε���
					i = j;
					j = k;
				}
			}

		}
	}
	fin.close();

	//�������
	ofstream fout_1("Pi.mat");	//��ʼ���ʾ���
	ofstream fout_2("A1.mat");	//����״̬ת�ƾ���
	ofstream fout_3("A2.mat");	//����״̬ת�ƾ���
	ofstream fout_4("B1.mat");	//���׷�����ʾ���
	ofstream fout_5("B2.mat");	//���׷�����ʾ���
	if (!(fout_1 && fout_2 && fout_3 && fout_4 && fout_5)){
		cerr << "Create Matrix file failed !" << endl;
		exit(-1);
	}

	fout_1 << setprecision(8);
	fout_2 << setprecision(8);
	fout_3 << setprecision(8);
	fout_4 << setprecision(8);
	fout_5 << setprecision(8);

	//��ʼ״̬����д���ļ�
	double arr_pi[N] = { 0.0 };
	//turingGood(Pi, arr_pi, N);
	turingAdd(Pi, arr_pi, N);
	for (int i = 0; i < N; i++){
		fout_1 << arr_pi[i] << "\t";
	}
	fout_1 << endl;

	//����״̬ת�ƾ���д���ļ�
	double arr_a_1[N] = { 0.0 };
	for (int i = 0; i < N; i++){
		//turingGood(A1[i], arr_a_1, N);
		turingAdd(A1[i], arr_a_1, N);
		for (int j = 0; j < N; j++){
			fout_2 << arr_a_1[j] << "\t";
		}
		fout_2 << endl;
	}

	//����״̬ת�ƾ���д���ļ�
	double arr_a_2[N] = { 0.0 };
	for (int i = 0; i < N; i++){
		for (int j = 0; j < N; j++){
			//turingGood(A2[i][j], arr_a_2, N);
			turingAdd(A2[i][j], arr_a_2, N);
			for (int k = 0; k < N; k++){
				fout_3 << arr_a_2[k] << "\t";
			}
			fout_3 << endl;
		}
	}

	//���׷�����ʾ���д���ļ�
	double arr_b_1[M] = { 0.0 };
	for (int i = 0; i < N; i++){
		//turingGood(B1[i], arr_b_1, M);
		turingAdd(B1[i], arr_b_1, M);
		for (int j = 0; j < M; j++){
			fout_4 << arr_b_1[j] << "\t";
		}
		fout_4 << endl;
	}

	//���׷�����ʾ���д���ļ�
	double arr_b_2[M] = { 0.0 };
	for (int i = 0; i < N; i++){
		for (int j = 0; j < N; j++){
			//turingGood(B2[i][j], arr_b_2, M);
			turingAdd(B2[i][j], arr_b_2, M);
			for (int k = 0; k < M; k++){
				fout_5 << arr_b_2[k] << "\t";
			}
			fout_5 << endl;
		}
	}

	fout_1.close();
	fout_2.close();
	fout_3.close();
	fout_4.close();
	fout_5.close();

	return 0;
}
