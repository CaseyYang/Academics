#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<iomanip>

using namespace std;

const int SNUM = 4;       //SNUM������״̬
const char state[SNUM] = { 'B', 'M', 'E', 'S' };
int A1[SNUM][SNUM];     //��¼һ��Markov״̬ת�ƵĴ���
int A2[SNUM][SNUM][SNUM];   //��¼����Markov״̬ת�ƵĴ���
int PI[SNUM];       //��¼����״̬���ֵĴ���

inline int stateIndex(char state){
	switch (state){
	case 'B':return 0;
		break;
	case 'M':return 1;
		break;
	case 'E':return 2;
		break;
	case 'S':return 3;
		break;
	default:return -1;
		break;
	}
}
//��������״ֻ̬��4�֣�ѵ�������㹻�࣬���п��ܵ�״̬ת����ѵ�������ж�����֣����Բ�ʹ���κ�ƽ���㷨
inline void noneturing(const int count[], double prob[], int len){
	double total = 0.0;
	for (int i = 0; i<len; ++i)
		total += count[i];
	if (total == 0.0){
		for (int i = 0; i<len; ++i)
			prob[i] = 0.0;
	}
	else{
		for (int i = 0; i<len; ++i)
			prob[i] = count[i] / total;
	}
}

int main(int argc, char *argv[]){
	if (argc<2){
		cout << "usage: " << argv[1] << " BMES_marked_file" << endl;
		return 1;
	}
	ifstream ifs(argv[1]);
	if (!ifs){
		cerr << "open inputfile " << argv[1] << " failed." << endl;
		return 1;
	}

	string line;
	int lineno = 0;
	while (getline(ifs, line)){
		lineno++;
		char state;       //״̬
		int i, j, k;
		string::size_type local;
		if ((local = line.find("/", local + 1)) != string::npos){     //��ȡ���ӵĵ�1��״̬
			state = line.at(local + 1);
			//cout<<state<<endl;
			i = stateIndex(state);
			PI[i]++;
			if ((local = line.find("/", local + 1)) != string::npos){       //��ȡ���ӵĵ�2��״̬
				state = line.at(local + 1);
				//cout<<state<<endl;
				j = stateIndex(state);
				PI[j]++;
				A1[i][j]++;
				while ((local = line.find("/", local + 1)) != string::npos){       //�����ȡ�����еĺ���״̬
					state = line.at(local + 1);
					//cout<<state<<endl;
					k = stateIndex(state);
					PI[k]++;
					A1[j][k]++;
					A2[i][j][k]++;
					i = j;
					j = k;
				}
			}
		}
	}
	ifs.close();

	/*    for(int i=0;i<SNUM;++i){
	for(int j=0;j<SNUM;++j){
	cout<<A1[i][j]<<"\t";
	}
	cout<<endl;
	}
	*/
	ofstream ofs1("A1.mat");
	ofstream ofs2("A2.mat");
	ofstream ofs3("PI.mat");
	if (!(ofs1 && ofs2 && ofs3)){
		cerr << "create matrix file failed." << endl;
		return 1;
	}
	ofs1 << setprecision(8);
	ofs2 << setprecision(8);
	ofs3 << setprecision(8);

	double PImatrix[SNUM];
	noneturing(PI, PImatrix, SNUM);
	for (int i = 0; i<SNUM; ++i){
		ofs3 << PImatrix[i] << "\t";
	}
	ofs3 << endl;

	for (int i = 0; i<SNUM; ++i){
		double arr[SNUM];
		noneturing(A1[i], arr, SNUM);
		for (int j = 0; j<SNUM; ++j){
			ofs1 << arr[j] << "\t";
		}
		ofs1 << endl;
	}

	for (int i = 0; i<SNUM; ++i){
		for (int j = 0; j<SNUM; ++j){
			double arr[SNUM];
			noneturing(A2[i][j], arr, SNUM);
			for (int k = 0; k<SNUM; ++k){
				ofs2 << arr[k] << "\t";
			}
			ofs2 << endl;
		}
	}

	ofs1.close();
	ofs2.close();
	ofs3.close();

	return 0;
}