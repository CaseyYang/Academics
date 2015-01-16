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
* 转换类，获取编号
*/
class DB{
private:
	map<string, int> cchar_map;	//汉字-编码映射
	map<int, string> index_map;	//编码-汉字映射
public:
	DB();
	DB(string file);
	string getCchar(int id);		//根据编码获得汉字
	int getObservIndex(string cchar);	//根据汉字获得编码
	int getStateIndex(char state);		//根据状态获得状态编号
	vector<int> makeObservs(string line);	//将输入的句子构造为发射符号序列
};

//无参构造函数
DB::DB(){

}

//有参构造函数
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
		//加入map
		cchar_map[cchar] = id;
		index_map[id] = cchar;
	}
	cout << "cchar_map大小： " << cchar_map.size() << endl;
	cout << "index_map大小： " << index_map.size() << endl;
}

//将状态转换为数字编号
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

//将汉字转换为数字编号
int DB::getObservIndex(string cchar){
	map<string, int>::iterator iter = cchar_map.find(cchar);
	if (iter != cchar_map.end()){
		return iter->second;
	}
	else{
		return -1;
	}
}

//将数字编号转换为汉字
string DB::getCchar(int id){
	map<int, string>::iterator iter = index_map.find(id);
	if (iter != index_map.end()){
		return iter->second;
	}
	else{
		return NULL;
	}
}


//将输入的句子构造为发射符号序列
vector<int> DB::makeObservs(string line){
	vector<int> vec_observ; //输出符号的集合
	string cchar = "";      //存放每个汉字
	string word = "";       //存放一个单词
	int num = 0;            //单词的字数
	int index = -1;         //单词对应的编号

	line = replace_all(line, "/", " ");
	cout << line << endl;
	istringstream strstm(line);
	while (strstm >> word){
		if (word.size() % 3 != 0){
			cout << "单词不符合要求：" << word << endl;
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