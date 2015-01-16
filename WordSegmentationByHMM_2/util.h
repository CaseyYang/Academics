#ifndef UTIL_H
#define UTIL_H

#include <string>

using namespace std;

/*
* �������ܣ����ַ����е������ض��Ӵ��û�Ϊ�µ��ַ���
* �������룺str     ��Ҫ���в������ַ���
*         old_str �ɵ��ַ���
*         new_str �µ��ַ���
* ����������û���ϵ��ַ���
*/
string& replace_all(string &str, string old_str, string new_str){
	while (1){
		string::size_type pos(0);
		if ((pos = str.find(old_str)) != string::npos){
			str.replace(pos, old_str.length(), new_str);
		}
		else{
			break;
		}
	}
	return str;
}

#endif