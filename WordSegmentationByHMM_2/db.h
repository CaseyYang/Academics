#ifndef DB_H
#define DB_H

#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <cstdlib>
#include "util.h"

using namespace std;

/*
* ת���࣬��ȡ���
*/
class DB{
private:
	map<string, int> cchar_map;	//����-����ӳ��
	map<int, string> index_map;	//����-����ӳ��
public:
	DB();
	DB(string file);
	string getCchar(int id);		//���ݱ����ú���
	int getObservIndex(string cchar);	//���ݺ��ֻ�ñ���
	int getStateIndex(char state);		//����״̬���״̬���
	vector<int> makeObservs(string line);	//������ľ��ӹ���Ϊ�����������
};

//�޲ι��캯��
DB::DB(){

}

//�вι��캯��
DB::DB(string file){
	ifstream fin(file.c_str());
	if (!fin){
		cout << "Open input file fail ! Can't init Trans !" << endl;
		exit(-1);
	}
	string line = "";
	string word = "";
	string cchar = "";
	int id = 0;
	while (getline(fin, line)){
		istringstream strstm(line);
		strstm >> word;
		cchar = word;
		strstm >> word;
		id = atoi(word.c_str());
		//����map
		cchar_map[cchar] = id;
		index_map[id] = cchar;
	}
	cout << "cchar_map��С�� " << cchar_map.size() << endl;
	cout << "index_map��С�� " << index_map.size() << endl;
}

//��״̬ת��Ϊ���ֱ��
int DB::getStateIndex(char state){
	switch (state){
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

//������ת��Ϊ���ֱ��
int DB::getObservIndex(string cchar){
	map<string, int>::iterator iter = cchar_map.find(cchar);
	if (iter != cchar_map.end()){
		return iter->second;
	}
	else{
		return -1;
	}
}

//�����ֱ��ת��Ϊ����
string DB::getCchar(int id){
	map<int, string>::iterator iter = index_map.find(id);
	if (iter != index_map.end()){
		return iter->second;
	}
	else{
		return NULL;
	}
}


//������ľ��ӹ���Ϊ�����������
vector<int> DB::makeObservs(string line){
	vector<int> vec_observ; //������ŵļ���
	string cchar = "";      //���ÿ������
	string word = "";       //���һ������
	int num = 0;            //���ʵ�����
	int index = -1;         //���ʶ�Ӧ�ı��

	line = replace_all(line, "/", " ");
	cout << line << endl;
	istringstream strstm(line);
	while (strstm >> word){
		if (word.size() % 3 != 0){
			cout << "���ʲ�����Ҫ��" << word << endl;
			continue;
		}
		num = word.size() / 3;
		if (num == 0){
			continue;
		}
		else{
			for (int i = 0; i < num; i++){
				cchar = word.substr(3 * i, 3);
				index = getObservIndex(cchar);
				vec_observ.push_back(index);
				//cout << "cchar = " << cchar << "   index = " << index << endl;
			}
		}
	}

	return vec_observ;
}

#endif