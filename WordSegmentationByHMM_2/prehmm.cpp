#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <map>
#include "util.h"

using namespace std;


/*
* �������ܣ���ѵ�����ϺͲ��������г��ֵĺ��ֽ��б��룬�����ǵĶ�Ӧ��ϵ�����ļ�
*         ��ʽΪ������-���룬�����0��ʼ
* �������룺infile_1 ѵ�������ļ���
*         infile_2 ���������ļ���
*         outfile  ָ��������ļ���
* �����������Ϊoutfile���ļ�
*/
void makeDB(string infile_1, string infile_2, string outfile){
	//��ȡ�����ļ�
	ifstream fin_1(infile_1.c_str());
	ifstream fin_2(infile_2.c_str());
	if (!(fin_1 && fin_2)){
		cerr << "makeDB : Open input file fail !" << endl;
		exit(-1);
	}
	//������ļ�
	ofstream fout(outfile.c_str());
	if (!fout){
		cerr << "makeDB : Open output file fail !" << endl;
		exit(-1);
	}

	map<string, int> map_cchar;
	int id = -1;
	string line = "";
	string cchar = "";
	//��ȡ�����ļ�����
	while (getline(fin_1, line)){
		line = replace_all(line, "/", "");
		if (line.size() >= 3){
			//���ֶ�ȡ
			for (int i = 0; i < line.size() - 2; i += 3){
				cchar = line.substr(i, 3);
				if (map_cchar.find(cchar) == map_cchar.end()){
					++id;
					map_cchar[cchar] = id;
				}
			}
		}
	}
	while (getline(fin_2, line)){
		line = replace_all(line, "/", "");
		if (line.size() >= 3){
			//���ֶ�ȡ
			for (int i = 0; i < line.size() - 2; i += 3){
				cchar = line.substr(i, 3);
				if (map_cchar.find(cchar) == map_cchar.end()){
					++id;
					map_cchar[cchar] = id;
				}
			}
		}
	}

	//������ļ�
	map<string, int>::iterator iter;
	for (iter = map_cchar.begin(); iter != map_cchar.end(); ++iter){
		//cout << iter -> first << " " << iter -> second << endl;
		fout << iter->first << " " << iter->second << endl;
	}

	fin_1.close();
	fin_2.close();
	fout.close();
}


/*
* �������ܣ���ѵ������ÿ�����ֺ�������Ӧ��BMES״̬
* �������룺infile  ѵ�������ļ���
*         outfile ָ��������ļ���
* �����������Ϊoutfile���ļ�
*/
void makeBMES(string infile, string outfile){

	ifstream fin(infile.c_str());
	ofstream fout(outfile.c_str());
	if (!(fin && fout)){
		cerr << "makeBMES : Open file failed !" << endl;
		exit(-1);
	}

	string word_in = "";
	string word_out = "";
	string line_in = "";
	string line_out = "";

	while (getline(fin, line_in)){
		if (line_in.size() >= 3){
			line_out.clear();
			line_in = replace_all(line_in, "/", " ");
			istringstream strstm(line_in);
			while (strstm >> word_in){
				word_out.clear();
				if (word_in.size() % 3 != 0){
					cout << "���ʲ�����Ҫ��" << word_in << endl;
					continue;
				}
				int num = word_in.size() / 3;	//�����а������ٸ�����
				if (num == 0){
					continue;
				}

				if (num == 1){
					word_out = word_in;
					word_out += "/S";
				}
				else{
					//���Ƶ����еĵ�һ����
					word_out.insert(word_out.size(), word_in, 0, 3);
					word_out += "/B";
					//������Ƶ����м����
					for (int i = 1; i < num - 1; i++){
						word_out.insert(word_out.size(), word_in, 3 * i, 3);
						word_out += "/M";
					}
					//���Ƶ��������ĺ���
					word_out.insert(word_out.size(), word_in, 3 * num - 3, 3);
					word_out += "/E";
				}

				line_out += word_out;
			}

			fout << line_out << endl;
		}
	}

}


/*
* ������
*/
int main(int argc, char *argv[]){
	if (argc < 5){
		cout << "Usage: " << argv[0] << " train_file test_file db_file bmes_file" << endl;
		exit(-1);
	}
	//����DB�ļ�������ѵ�����ϡ��������ϡ�����ļ���
	makeDB(argv[1], argv[2], argv[3]);

	//����BMES�ļ�������ѵ�����ϡ�����ļ���
	makeBMES(argv[1], argv[4]);

}