#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<stack>
#include<sys/stat.h>
#include<gdbm.h>
#include<cstdlib>

using namespace std;

const string DB_FILE_BLOCK = "dict_db";
GDBM_FILE dbm_ptr;
const int SNUM = 4;       //����״̬���ϴ�С
const int ONUM = 4782;    //�۲�ֵ���ϴ�С

/*����HMMģ�Ͳ���*/
double PI[SNUM];
double A1[SNUM][SNUM];
double A2[SNUM][SNUM][SNUM];
double B1[SNUM][ONUM];
double B2[SNUM][SNUM][ONUM];

inline int stateIndex(char state)
{
	switch (state) {
	case 'B':
		return 0;
		break;
	case 'M':
		return 1;
		break;
	case 'E':
		return 2;
		break;
	case 'S':
		return 3;
		break;
	default:
		return -1;
		break;
	}
}

inline int observIndex(string chinese)
{
	//cout << chinese << endl;
	datum key, value;
	key.dptr = const_cast < char *>(chinese.c_str());
	key.dsize = 3;
	value = gdbm_fetch(dbm_ptr, key);
	int index = atoi(value.dptr);
	return index;
}

/*���ļ��ж���HMMģ�Ͳ���*/
void initHMM(string f1, string f2, string f3, string f4, string f5){
	ifstream ifs1(f1.c_str());
	ifstream ifs2(f2.c_str());
	ifstream ifs3(f3.c_str());
	ifstream ifs4(f4.c_str());
	ifstream ifs5(f5.c_str());
	if (!(ifs1 && ifs2 && ifs3 && ifs4 && ifs5)){
		cerr << "Open file failed!" << endl;
		exit(1);
	}

	//��ȡPI
	string line;
	if (getline(ifs1, line)){
		istringstream strstm(line);
		string word;
		for (int i = 0; i<SNUM; ++i){
			strstm >> word;
			PI[i] = atof(word.c_str());
		}
	}
	else{
		cerr << "Read PI failed!" << endl;
		exit(1);
	}

	//��ȡA1
	for (int i = 0; i<SNUM; ++i){
		getline(ifs2, line);
		istringstream strstm(line);
		string word;
		for (int j = 0; j<SNUM; ++j){
			strstm >> word;
			A1[i][j] = atof(word.c_str());
		}
	}
	//��ȡA2
	for (int i = 0; i<SNUM; ++i){
		for (int j = 0; j<SNUM; ++j){
			getline(ifs3, line);
			istringstream strstm(line);
			string word;
			for (int k = 0; k<SNUM; ++k){
				strstm >> word;
				A2[i][j][k] = atof(word.c_str());
			}
		}
	}

	//��ȡB1
	for (int i = 0; i<SNUM; ++i){
		getline(ifs4, line);
		istringstream strstm(line);
		string word;
		for (int j = 0; j<ONUM; ++j){
			strstm >> word;
			B1[i][j] = atof(word.c_str());
		}
	}
	//��ȡB2
	for (int i = 0; i<SNUM; ++i){
		for (int j = 0; j<SNUM; ++j){
			getline(ifs5, line);
			istringstream strstm(line);
			string word;
			for (int k = 0; k<ONUM; ++k){
				strstm >> word;
				B2[i][j][k] = atof(word.c_str());
			}
		}
	}

	ifs1.close();
	ifs2.close();
	ifs3.close();
	ifs4.close();
	ifs5.close();
}

/*Viterbi�㷨���зִ�*/
void viterbi(string sentence, string &result){
	if (sentence.size() == 0)
		return;
	result.clear();
	int time = sentence.size() / 3;     //�۲����еĳ���
	if (time<3){  //�۲�������ֻ��1���ֻ�2���֣�������Ϊ�����1����
		result = sentence.append(" ");
		return;
	}
	double ***Q = new double **[SNUM];    //��̬���벢��ʼ��Q��Path����
	int ***Path = new int**[SNUM];
	for (int i = 0; i<SNUM; ++i){
		Q[i] = new double *[SNUM];
		Path[i] = new int *[SNUM];
		for (int j = 0; j<SNUM; ++j){
			Q[i][j] = new double[ONUM];
			Path[i][j] = new int[ONUM];
			for (int k = 0; k<ONUM; ++k){
				Q[i][j][k] = 0.0;
				Path[i][j][k] = 0;
			}
		}
	}

	//��Q��Path����ĵ�1���渳ֵ
	string chinese1 = sentence.substr(0, 3);
	int o1 = observIndex(chinese1);
	string chinese2 = sentence.substr(3, 3);
	int o2 = observIndex(chinese2);
	for (int i = 0; i<SNUM; ++i){
		for (int j = 0; j<SNUM; ++j){
			Q[i][j][0] = PI[i] * A1[i][j] * B1[i][o1] * B2[i][j][o2];
			Path[i][j][0] = -1;
		}
	}

	//��Q��Path����ĺ������渳ֵ      
	for (int t = 1; t<time - 1; ++t){
		string chinese = sentence.substr(3 * (t + 1), 3);
		int ot = observIndex(chinese);
		for (int j = 0; j<SNUM; ++j){
			for (int k = 0; k<SNUM; ++k){
				double max = -1.0;
				int maxindex = -1;
				for (int i = 0; i<SNUM; ++i){
					double product = Q[i][j][t - 1] * A2[i][j][k];
					if (product>max){
						max = product;
						maxindex = i;
					}
				}
				Q[j][k][t] = max*B2[j][k][ot];
				Path[j][k][t] = maxindex;
			}
		}
	}

	//��Q�������һ������ֵ
	double max = -1.0;
	int maxindexi = -1;
	int maxindexj = -1;
	for (int i = 0; i<SNUM; ++i){
		for (int j = 0; j<SNUM; ++j){
			if (Q[i][j][time - 2]>max){
				max = Q[i][j][time - 2];
				maxindexi = i;
				maxindexj = j;
			}
		}
	}
	//��maxindexj,maxindexi����������Path�����ҳ�����ܵ�״̬����
	stack<int> st;
	st.push(maxindexj);
	st.push(maxindexi);
	for (int t = time - 3; t >= 0; --t){
		int maxindexk = Path[maxindexi][maxindexj][t + 1];
		st.push(maxindexk);
		maxindexj = maxindexi;
		maxindexi = maxindexk;
	}
	//�ͷ���ά����
	for (int i = 0; i<SNUM; ++i){
		for (int j = 0; j<SNUM; ++j){
			delete[]Q[i][j];
			delete[]Path[i][j];
		}
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
	initHMM("PI.mat", "A1.mat", "A2.mat", "B1.mat", "B2.mat");

	ifstream ifs(argv[1]);
	ofstream ofs(argv[2]);
	if (!(ifs&&ofs)){
		cerr << "Open file failed!" << endl;
		return 1;
	}

	string line, line_out;
	int lineno = 0;
	//ѭ����ȡÿһ��
	while (getline(ifs, line)){
		lineno++;
		//cout<<"line="<<line<<endl;
		line_out.clear();
		istringstream strstm(line);
		string sentence;
		string result;
		while (strstm >> sentence){
			//if(sentence.size()<6){
			//  cout<<lineno<<": "<<sentence<<endl;
			//  continue;
			//}
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