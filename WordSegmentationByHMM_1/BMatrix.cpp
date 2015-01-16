#include<iostream>
#include<cstdlib>
#include<iomanip>
#include<fstream>
#include<sstream>
#include<string>
#include"gt.h"
#include<sys/stat.h>
#include<gdbm.h>

const int SNUM = 4;
const int ONUM = 4782;      //字典数据库中共有ONUM项

int B1[SNUM][ONUM];     //记录一阶状态发射的次数
int B2[SNUM][SNUM][ONUM];   //记录二阶状态发射的次数

GDBM_FILE dbm_ptr;

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

int main(int argc, char *argv[])
{
	if (argc < 2) {
		cout << "usage: " << argv[0] << " inputfile" << endl;
		return 1;
	}
	ifstream ifs(argv[1]);
	if (!ifs) {
		cerr << "open file " << argv[1] << " failed." << endl;
		return 1;
	}
	dbm_ptr = gdbm_open("dict_db", 0, GDBM_READER, S_IRUSR | S_IWUSR, NULL);

	string line;
	//int line_no = 1;
	while (getline(ifs, line)) {
		//cout << line_no++ << endl;
		string::size_type local;
		if ((local = line.find("/")) != string::npos) {
			char s = line.at(local + 1);    //第1个状态
			string chinese = line.substr(local - 3, 3); //第1个观察值
			int j = stateIndex(s);
			int k = observIndex(chinese);
			B1[j][k]++;
			int i;
			while ((local =
				line.find("/", local + 1)) != string::npos) {
				s = line.at(local + 1); //下1个状态
				chinese = line.substr(local - 3, 3);    //下1个观测值
				i = j;
				j = stateIndex(s);
				k = observIndex(chinese);
				B1[j][k]++;
				B2[i][j][k]++;
			}
		}
	}
	ifs.close();
	gdbm_close(dbm_ptr);

	ofstream ofs1("B1.mat");
	ofstream ofs2("B2.mat");
	if (!(ofs1 && ofs2)) {
		cerr << "create outputfile failed." << endl;
		return 1;
	}
	ofs1 << setprecision(8);
	ofs2 << setprecision(8);
	double arr[ONUM] = { 0.0 };
	for (int i = 0; i < SNUM; ++i) {
		goodturing(B1[i], arr, ONUM);
		for (int j = 0; j < ONUM; ++j)
			ofs1 << arr[j] << "\t";
		ofs1 << endl;
	}
	for (int i = 0; i < SNUM; ++i) {
		for (int j = 0; j < SNUM; ++j) {
			goodturing(B2[i][j], arr, ONUM);
			for (int k = 0; k < ONUM; ++k)
				ofs2 << arr[k] << "\t";
			ofs2 << endl;
		}
	}
	ofs1.close();
	ofs2.close();

	return 0;
}