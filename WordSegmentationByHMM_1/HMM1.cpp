//ԭ�������ӣ�http://www.cnblogs.com/zhangchaoyang/articles/2571110.html

#include<sys/stat.h>
#include<ctype.h>
#include<gdbm.h>
#include<iostream>
#include<sstream>
#include<fstream>
#include<string>
#include<cstring>
#include<cstdlib>
#include<stack>
using namespace std;

const string DB_FILE_BLOCK = "dict_db";
const int WORDS_NUM = 4782;
GDBM_FILE dbm_ptr;

double PI[4];       //��ʼ״̬���ʾ���
double A[4][4];     //״̬ת�ƾ���
double B[4][WORDS_NUM];     //�������

/*���ļ��ж���HMMģ�Ͳ���*/
void initHMM(string f1, string f2, string f3){
	ifstream ifs1(f1.c_str());
	ifstream ifs2(f2.c_str());
	ifstream ifs3(f3.c_str());
	if (!(ifs1 && ifs2 && ifs3)){
		cerr << "Open file failed!" << endl;
		exit(1);
	}

	//��ȡPI
	string line;
	if (getline(ifs1, line)){
		istringstream strstm(line);
		string word;
		for (int i = 0; i<4; ++i){
			strstm >> word;
			PI[i] = atof(word.c_str());
		}
	}
	else{
		cerr << "Read PI failed!" << endl;
		exit(1);
	}

	//��ȡA
	for (int i = 0; i<4; ++i){
		getline(ifs2, line);
		istringstream strstm(line);
		string word;
		for (int j = 0; j<4; ++j){
			strstm >> word;
			A[i][j] = atof(word.c_str());
		}
	}

	//��ȡB
	for (int i = 0; i<4; ++i){
		getline(ifs3, line);
		istringstream strstm(line);
		string word;
		for (int j = 0; j<WORDS_NUM; ++j){
			strstm >> word;
			B[i][j] = atof(word.c_str());
		}
	}

	ifs1.close();
	ifs2.close();
	ifs3.close();
}

/*Viterbi�㷨���зִ�*/
void viterbi(string sentence, string &result){
	if (sentence.size() == 0)
		return;
	result.clear();
	int row = sentence.size() / 3;      //�۲����еĳ���
	double **Q = new double*[row];    //��ʼ��Q����
	for (int i = 0; i<row; ++i)
		Q[i] = new double[4]();
	int **Path = new int*[row];   //��ʼ��Path����
	for (int i = 0; i<row; ++i)
		Path[i] = new int[4]();

	//��Q��Path����ĵ�1�и�ֵ
	datum key, data;
	char chinese[3] = { 0 };
	char *bp = const_cast<char*>(sentence.c_str());
	strncpy(chinese, bp, 3);      //��ȡ�����еĵ�1������   
	key.dptr = chinese;
	key.dsize = 3;
	data = gdbm_fetch(dbm_ptr, key);       //�����ݿ��л�ȡ���ֶ�Ӧ��index����index��Ӧ����������
	int colindex = atoi(data.dptr);
	for (int i = 0; i<4; ++i){
		Path[0][i] = -1;
		Q[0][i] = PI[i] * B[i][colindex];
	}

	//��Q��Path����ĺ����и�ֵ
	for (int i = 1; i<row; ++i){
		bp = const_cast<char*>(sentence.c_str() + i * 3);
		strncpy(chinese, bp, 3);  //��ȡ�����е���һ������
		key.dptr = chinese;
		data = gdbm_fetch(dbm_ptr, key);
		colindex = atoi(data.dptr);
		for (int j = 0; j<4; ++j){
			double max = -1.0;
			int maxindex = -1;
			for (int k = 0; k<4; ++k){
				double product = Q[i - 1][k] * A[k][j];
				if (product>max){
					max = product;
					maxindex = k;
				}
			}
			Q[i][j] = max*B[j][colindex];
			Path[i][j] = maxindex;
		}
	}

	//��Q�������һ�е����ֵ
	double max = -1.0;
	int maxindex = -1;
	for (int i = 0; i<4; ++i){
		if (Q[row - 1][i]>max){
			max = Q[row - 1][i];
			maxindex = i;
		}
	}
	//��maxindex����������Path�����ҳ�����ܵ�״̬����
	stack<int> st;
	st.push(maxindex);
	for (int i = row - 1; i>0; --i){
		maxindex = Path[i][maxindex];
		st.push(maxindex);
	}
	//�ͷŶ�ά����
	for (int i = 0; i<row; ++i){
		delete[]Q[i];
		delete[]Path[i];
	}
	delete[]Q;
	delete[]Path;

	//���ݱ�Ǻõ�״̬���зִ�
	int pos = 0;
	//cout<<sentence<<endl;
	while (!st.empty()){
		int mark = st.top();
		st.pop();
		result.insert(result.size(), sentence, pos, 3);
		if (mark == 2 || mark == 3){     //״̬��E��S
			result.append("  ");
		}
		pos += 3;
	}
	result.append("\t");
}

int main(int argc, char *argv[]){
	if (argc<3){
		cout << "Usage: " << argv[0] << " inputfile outputfile" << endl;
		return 1;
	}

	dbm_ptr = gdbm_open(DB_FILE_BLOCK.c_str(), 0, GDBM_READER, S_IRUSR | S_IWUSR, NULL);
	initHMM("PI.mat", "A1.mat", "B1.mat");

	ifstream ifs(argv[1]);
	ofstream ofs(argv[2]);
	if (!(ifs&&ofs)){
		cerr << "Open file failed!" << endl;
		return 1;
	}

	string line, line_out;
	//ѭ����ȡÿһ��
	while (getline(ifs, line)){
		//cout<<"line="<<line<<endl;
		line_out.clear();
		istringstream strstm(line);
		string sentence;
		string result;
		while (strstm >> sentence){
			//cout<<"sentence="<<sentence<<endl;
			viterbi(sentence, result);
			line_out += result;
		}
		ofs << line_out << endl;
	}
	ifs.close();
	ofs.close();
	gdbm_close(dbm_ptr);
	return 0;
}