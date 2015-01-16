#ifndef _HEADER_H
#define _HEADER_H

#include<vector>
#include<list>
#include<map>

using namespace std;

void goodturing(const int count[], double prob[], int len){
	map<int, list<int> > count_map;     //map可以自动按key排好序
	int N = 0;
	for (int i = 0; i<len; ++i){
		int c = count[i];
		N += c;
		map<int, list<int> >::const_iterator itr;
		itr = count_map.find(c);
		if (itr == count_map.end()){
			list<int> l;
			l.push_back(i);
			count_map[c] = l;
		}
		else{
			count_map[c].push_back(i);
		}
	}

	if (N == 0){
		for (int i = 0; i<len; ++i)
			prob[i] = 0.0;
		return;
	}

	map<int, list<int> >::const_iterator iter = count_map.begin();
	while (iter != count_map.end()){
		double pr;
		int r = iter->first;
		int nr = iter->second.size();
		if (++iter != count_map.end()){
			int r_new = iter->first;
			if (r_new = r + 1){
				int nr_1 = iter->second.size();
				pr = (1.0 + r)*nr_1 / (N*nr);
			}
			else{
				pr = 1.0*r / N;
			}
		}
		else{
			pr = 1.0*r / N;
		}
		list<int> l = (--iter)->second;
		list<int>::const_iterator itr1 = l.begin();
		while (itr1 != l.end()){
			int index = *itr1;
			itr1++;
			prob[index] = pr;
		}
		++iter;
	}

	//概率归一化
	double sum = 0;
	for (int i = 0; i<len; ++i)
		sum += prob[i];
	for (int i = 0; i<len; ++i)
		prob[i] /= sum;
}

#endif