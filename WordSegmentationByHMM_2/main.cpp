#include <cstdlib>
#include <vector>
#include <iomanip>
#include <map>
#include <algorithm>
#include <sys/time.h>
#include <sys/stat.h>
#include "util.h"
#include "viterbi.cpp"
using namespace std;

const long MaxCount = 50000;	//��Ҫ�зֵ�����������������ֵ�����ļ���
//ʵ�ʵľ�����������ʵ�ʾ�������Ϊ׼��

//��ȡ��ǰʱ��(ms)
long getCurrentTime(){
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

//��ȡ�ļ���С
unsigned long getFileSize(string file_path){
	unsigned long filesize = -1;
	struct stat statbuff;
	if (stat(file_path.c_str(), &statbuff) < 0){
		return filesize;
	}
	else{
		filesize = statbuff.st_size;
	}
	return filesize;
}


/*
* �������ܣ������зֱ�ǵ�λ��
* �������룺1.strline_inδ�����зֵĺ����ַ���
2.strline_right�����зֺ�ĺ����ַ���
* ���������vecetor�����д����strline_in����Щλ�÷����˷ִʱ��
*         ע�⣺vector�в���������ǵ�λ�ã����ǰ���λ��0��
*/
vector<int> getPos(string strline_right, string strline_in){
	int pos_1 = 0;
	int pos_2 = -1;
	int pos_3 = 0;
	string word = "";
	vector<int> vec;

	int length = strline_right.length();
	while (pos_2 < length){
		//ǰ��ķִʱ��
		pos_1 = pos_2;

		//����ķִʱ��
		pos_2 = strline_right.find('/', pos_1 + 1);

		if (pos_2 > pos_1){
			//�������ִʱ��֮��ĵ���ȡ��
			word = strline_right.substr(pos_1 + 1, pos_2 - pos_1 - 1);
			//���ݵ���ȥ���������в�����ֵ�λ��
			pos_3 = strline_in.find(word, pos_3);
			//��λ�ô�������
			vec.push_back(pos_3);
			pos_3 = pos_3 + word.size();
		}
		else{
			break;
		}
	}

	return vec;
}


/*
* ��ȡ��׼�зֺͳ����зֵĽ��
*/
string getString(string word, int pos, vector<int> vec_right){
	char ss[1000];
	int i = 0;
	int k = 0;
	if (vec_right.size() == 0){
		return word;
	}
	while (vec_right[i] < pos){
		i++;
	}
	for (int j = 0; j < word.size(); j++){
		if (j == vec_right[i] - pos){
			if (j != 0){
				ss[k] = '/';
				++k;
			}
			++i;
		}
		ss[k] = word[j];
		++k;
	}
	ss[k] = '\0';
	string word_str = ss;

	return word_str;
}

/*
* �������ܣ���ȡ���������зֵĽ��ͳ��
* �������룺1.vec_right ��ȷ�ķִʱ��λ�ü���
*           2.vec_out   �����зֵõ��ķִʱ��λ�ü���
* �������������һ��veceor������4��Ԫ�أ��ֱ�Ϊ��
*          �з���ȷ����������塢δ��¼�ʡ����������������
*
*/
vector<int> getCount_2(string strline, vector<int> vec_right, vector<int> vec_out, vector<string> &vec_err){
	vector<int> vec(4, 0);	//��ż�����
	//����map
	map<int, int> map_result;
	for (int i = 0; i < vec_right.size(); i++){
		map_result[vec_right[i]] += 1;
	}
	for (int i = 0; i < vec_out.size(); i++){
		map_result[vec_out[i]] += 2;
	}

	//ͳ��map�е���Ϣ
	//��value=1��ֻ��vec_right��
	//��value=2��ֻ��vec_out��
	//��value=3����vec_right��vec_out�ж���
	map<int, int>::iterator p_pre, p_cur;
	int count_value_1 = 0;
	int count_value_2 = 0;
	int count_value_3 = 0;
	p_pre = map_result.begin();
	p_cur = map_result.begin();
	while (p_cur != map_result.end()){
		while (p_cur != map_result.end() && p_cur->second == 3){
			p_pre = p_cur;
			++count_value_3;	//�з���ȷ����Ŀ
			++p_cur;		//����������
		}

		while (p_cur != map_result.end() && p_cur->second != 3){
			if (p_cur->second == 1){
				++count_value_1;
			}
			else if (p_cur->second == 2){
				++count_value_2;
			}
			++p_cur;
		}

		//ȷ���зִ�����ַ���
		if (p_cur == map_result.end() && p_cur == (++p_pre)){
			continue;
		}
		int pos_1 = p_pre->first;
		int pos_2 = p_cur->first;
		string word = strline.substr(pos_1, pos_2 - pos_1);	//�зִ���ĵ���
		string word_right = getString(word, pos_1, vec_right);	//��ȷ���зַ�ʽ
		string word_out = getString(word, pos_1, vec_out);	//�õ����зַ�ʽ

		string str_err = "";
		//��ͬ�Ĵ�������		
		if (count_value_1 > 0 && count_value_2 == 0){
			str_err = "  ��������壺 " + word + "    ��ȷ�з֣� " + word_right + "    �����з֣� " + word_out;
			vec_err.push_back(str_err);
			cout << str_err << endl;
			vec[1] += count_value_1;
		}
		else if (count_value_1 == 0 && count_value_2 > 0){
			str_err = "  δ��¼��� " + word + "    ��ȷ�з֣� " + word_right + "    �����з֣� " + word_out;
			vec_err.push_back(str_err);
			cout << str_err << endl;
			vec[2] += count_value_2;
		}
		else if (count_value_1 > 0 && count_value_2 > 0){
			str_err = "  ���������壺 " + word + "    ��ȷ�з֣� " + word_right + "    �����з֣� " + word_out;
			vec_err.push_back(str_err);
			cout << str_err << endl;
			vec[3] += count_value_2;
		}

		//��������λ
		count_value_1 = 0;
		count_value_2 = 0;
	}

	vec[0] += count_value_3;

	return vec;
}


/*
* �����������зִʲ�ͳ�Ʒִʽ��
*
*/
int main(int argc, char *argv[]){
	if (argc < 3){
		cout << "Usage: " << argv[0] << " test_file result_file" << endl;
		exit(-1);
	}

	long time_1 = getCurrentTime();

	string strline_right;	//�������ϣ�������׼�ִʽ��
	string strline_in;	//ȥ���ִʱ�ǵ����ϣ������ִʵ����룩
	string strline_out_1;	//������Ʒ�ģ�ͣ����ף��ִ���ϵ�����
	string strline_out_2;	//������Ʒ�ģ�ͣ����ף��ִ���ϵ�����

	ifstream fin(argv[1]);	//�������ļ�
	if (!fin){
		cout << "Unable to open input file !" << argv[1] << endl;
		exit(-1);
	}

	ofstream fout(argv[2]);	//ȷ������ļ�
	if (!fout){
		cout << "Unable to open output file !" << endl;
		exit(-1);
	}

	long count = 0;			//���ӱ��
	long count_1 = 0;		//������Ʒ�ģ�ͣ����ף��з���ȫ��ȷ�ľ�������
	long count_2 = 0;		//������Ʒ�ģ�ͣ����ף��з���ȫ��ȷ�ľ�������
	long count_right_all = 0;	//׼ȷ���з�����
	//����
	long count_out_1_all = 0;	//������Ʒ�ģ���з�����
	long count_out_1_right_all = 0;	//������Ʒ�ģ���з���ȷ����
	long count_out_1_fail_1_all = 0;//������Ʒ�ģ�ͣ���������壩
	long count_out_1_fail_2_all = 0;//������Ʒ�ģ�ͣ�δ��¼���
	long count_out_1_fail_3_all = 0;//������Ʒ�ģ�ͣ����������壩
	//����
	long count_out_2_all = 0;	//������Ʒ�ģ���з�����
	long count_out_2_right_all = 0;	//������Ʒ�ģ���з���ȷ����
	long count_out_2_fail_1_all = 0;//������Ʒ�ģ�ͣ���������壩
	long count_out_2_fail_2_all = 0;//������Ʒ�ģ�ͣ�δ��¼���
	long count_out_2_fail_3_all = 0;//������Ʒ�ģ�ͣ����������壩


	vector<string> vec_err_1;	//������Ʒ�ģ�ͣ����ף��зִ���Ĵ�
	vector<string> vec_err_2;	//������Ʒ�ģ�ͣ����ף��зִ���Ĵ�

	while (getline(fin, strline_right, '\n') && count < MaxCount){
		if (strline_right.length() > 1){

			//ȥ���ִʱ��
			strline_in = strline_right;
			strline_in = replace_all(strline_in, "/", "");

			//������Ʒ�ģ�ͷִ�
			strline_out_1 = strline_right;
			istringstream strstm(strline_in);
			string sentence;
			string result_1;
			string result_2;
			string line_out_1;
			string line_out_2;
			while (strstm >> sentence){
				//�����з�
				result_1 = viterbiTwo(sentence);
				line_out_1 += result_1;
				//�����з�
				result_2 = viterbiThree(sentence);
				line_out_2 += result_2;
			}
			strline_out_1 = line_out_1;
			strline_out_2 = line_out_2;

			//����ִʽ��
			count++;
			cout << "----------------------------------------------" << endl;
			cout << "���ӱ�ţ�" << count << endl;
			cout << endl;
			cout << "���ִʵľ��ӳ���: " << strline_in.length() << "  ���ӣ�" << endl;
			cout << strline_in << endl;
			cout << endl;
			cout << "��׼�ȶԽ������: " << strline_right.length() << "  ���ӣ�" << endl;
			cout << strline_right << endl;
			cout << endl;
			cout << "������Ʒ�ģ�ͣ����ף��ִʳ���: " << strline_out_1.length() << "  ���ӣ�" << endl;
			cout << strline_out_1 << endl;
			cout << endl;
			cout << "������Ʒ�ģ�ͣ����ף��ִʳ���: " << strline_out_2.length() << "  ���ӣ�" << endl;
			cout << strline_out_2 << endl;
			cout << endl;


			//����ִʽ�����������б�ʾ
			vector<int> vec_right = getPos(strline_right, strline_in);
			vector<int> vec_out_1 = getPos(strline_out_1, strline_in);
			vector<int> vec_out_2 = getPos(strline_out_2, strline_in);

			cout << "��׼�����" << endl;
			for (int i = 0; i < vec_right.size(); i++){
				cout << setw(4) << vec_right[i];
			}
			cout << endl;

			cout << "������Ʒ�ģ�ͣ����ף��ִʽ����" << endl;
			for (int i = 0; i < vec_out_1.size(); i++){
				cout << setw(4) << vec_out_1[i];
			}
			cout << endl;

			cout << "������Ʒ�ģ�ͣ����ף��ִʽ����" << endl;
			for (int i = 0; i < vec_out_2.size(); i++){
				cout << setw(4) << vec_out_2[i];
			}
			cout << endl;


			//���ƥ��Ĵ����б�
			cout << endl;
			if (vec_right == vec_out_1){
				cout << "������Ʒ�ģ�ͣ����ף��ִ���ȫ��ȷ��" << endl;
				count_1++;
			}
			else{
				cout << "������Ʒ�ģ�ͣ����ף��ִʴ����б�" << endl;
			}
			vector<int> vec_count_1 = getCount_2(strline_in, vec_right, vec_out_1, vec_err_1);

			cout << endl;
			if (vec_right == vec_out_2){
				cout << "������Ʒ�ģ�ͣ����ף��ִ���ȫ��ȷ��" << endl;
				count_2++;
			}
			else{
				cout << "������Ʒ�ģ�ͣ����ף��ִʴ����б�" << endl;
			}
			vector<int> vec_count_2 = getCount_2(strline_in, vec_right, vec_out_2, vec_err_2);

			//׼ȷ���з�����
			int count_right = vec_right.size();
			//�зֵõ�������
			int count_out_1 = vec_out_1.size();
			int count_out_2 = vec_out_2.size();
			//�з���ȷ������
			int count_out_1_right = vec_count_1[0];
			cout << "�зֵõ���" << count_out_1 << endl;
			cout << "�з���ȷ��" << count_out_1_right << endl;

			cout << "������Ʒ�ģ�ͣ����ף���" << endl;
			cout << "  ��������壺" << vec_count_1[1] << endl;
			cout << "  δ��¼���" << vec_count_1[2] << endl;
			cout << "  ���������壺" << vec_count_1[3] << endl;

			int count_out_2_right = vec_count_2[0];
			cout << "�зֵõ���" << count_out_2 << endl;
			cout << "�з���ȷ��" << count_out_2_right << endl;

			cout << "������Ʒ�ģ�ͣ����ף���" << endl;
			cout << "  ��������壺" << vec_count_2[1] << endl;
			cout << "  δ��¼���" << vec_count_2[2] << endl;
			cout << "  ���������壺" << vec_count_2[3] << endl;

			count_right_all += count_right;

			count_out_1_all += count_out_1;
			count_out_1_right_all += count_out_1_right;
			count_out_1_fail_1_all += vec_count_1[1];
			count_out_1_fail_2_all += vec_count_1[2];
			count_out_1_fail_3_all += vec_count_1[3];

			count_out_2_all += count_out_2;
			count_out_2_right_all += count_out_2_right;
			count_out_2_fail_1_all += vec_count_2[1];
			count_out_2_fail_2_all += vec_count_2[2];
			count_out_2_fail_3_all += vec_count_2[3];

		}
	}

	long time_2 = getCurrentTime();
	unsigned long file_size = getFileSize("test.txt");


	//��ӡ������з�����	
	cout << endl;
	cout << "---------------------------------" << endl;
	cout << "���������������򣩣�" << endl;


	//�Դ����з����ݽ������򲢵��ظ���
	sort(vec_err_1.begin(), vec_err_1.end());
	sort(vec_err_2.begin(), vec_err_2.end());

	vector<string>::iterator end_unique_1 = unique(vec_err_1.begin(), vec_err_1.end());
	vector<string>::iterator end_unique_2 = unique(vec_err_2.begin(), vec_err_2.end());


	int num_1 = end_unique_1 - vec_err_1.begin();
	int num_2 = end_unique_2 - vec_err_2.begin();


	cout << "----------------------------------" << endl;
	cout << "������Ʒ�ģ�ͣ����ף��зִ���������" << num_1 << endl;
	for (int i = 0; i < num_1; i++){
		cout << vec_err_1[i] << endl;
	}
	cout << endl;

	cout << "----------------------------------" << endl;
	cout << "������Ʒ�ģ�ͣ����ף��зִ���������" << num_2 << endl;
	for (int i = 0; i < num_2; i++){
		cout << vec_err_2[i] << endl;
	}
	cout << endl;


	//����׼ȷ�ʺ��ٻ���
	double kk_1 = (double)count_out_1_right_all / count_out_1_all;	//������Ʒ�ģ�ͣ����ף�׼ȷ��
	double kk_2 = (double)count_out_1_right_all / count_right_all;	//������Ʒ�ģ�ͣ����ף��ٻ���
	double kk_3 = (double)count_out_2_right_all / count_out_2_all;	//������Ʒ�ģ�ͣ����ף�׼ȷ��
	double kk_4 = (double)count_out_2_right_all / count_right_all;	//������Ʒ�ģ�ͣ����ף��ٻ���


	//����������
	cout << endl;
	cout << "---------------------------------" << endl;
	cout << "�ִ�����ʱ�䣺" << time_2 - time_1 << "ms" << endl;
	cout << "�����ļ���С��" << file_size / 1024 << " KB" << endl;
	cout << "�ִ��ٶ�Ϊ��  " << (double)file_size * 1000 / ((time_2 - time_1) * 1024) << " KB/s" << endl;



	cout << endl;
	cout << "����������" << count << endl;

	cout << "������Ʒ�ģ�ͣ����ף��з���ȫ��ȷ�ľ�����Ŀ�� " << count_1 << "\t �� " << (double)count_1 * 100 / count << " % ��" << endl;
	cout << "������Ʒ�ģ�ͣ����ף��з���ȫ��ȷ�ľ�����Ŀ�� " << count_2 << "\t �� " << (double)count_2 * 100 / count << " % ��" << endl;

	cout << endl;

	cout << "׼ȷ���з�������" << count_right_all << endl;		//׼ȷ���з�����
	cout << "������Ʒ�ģ�ͣ����ף��з�������" << count_out_1_all << endl;		//������Ʒ�ģ���з�����	
	cout << "������Ʒ�ģ�ͣ����ף��з�������" << count_out_2_all << endl;		//������Ʒ�ģ���з�����	
	cout << "������Ʒ�ģ�ͣ����ף��з���ȷ������" << count_out_1_right_all << endl;	//������Ʒ�ģ���з���ȷ����
	cout << "������Ʒ�ģ�ͣ����ף��з���ȷ������" << count_out_2_right_all << endl;	//������Ʒ�ģ���з���ȷ����


	cout << endl;
	cout << "������Ʒ�ģ�ͣ����ף���" << endl;
	long count_out_1_fail_all = count_out_1_fail_1_all + count_out_1_fail_2_all + count_out_1_fail_3_all;
	cout << "  ��������壺" << count_out_1_fail_1_all << "\t ( " << (double)count_out_1_fail_1_all * 100 / count_out_1_fail_all << " % )" << endl;
	cout << "  δ��¼���" << count_out_1_fail_2_all << "\t ( " << (double)count_out_1_fail_2_all * 100 / count_out_1_fail_all << " % )" << endl;
	cout << "  ���������壺" << count_out_1_fail_3_all << "\t ( " << (double)count_out_1_fail_3_all * 100 / count_out_1_fail_all << " % )" << endl;
	cout << endl;
	cout << "������Ʒ�ģ�ͣ����ף���" << endl;
	long count_out_2_fail_all = count_out_2_fail_1_all + count_out_2_fail_2_all + count_out_2_fail_3_all;
	cout << "  ��������壺" << count_out_2_fail_1_all << "\t ( " << (double)count_out_2_fail_1_all * 100 / count_out_2_fail_all << " % )" << endl;
	cout << "  δ��¼���" << count_out_2_fail_2_all << "\t ( " << (double)count_out_2_fail_2_all * 100 / count_out_2_fail_all << " % )" << endl;
	cout << "  ���������壺" << count_out_2_fail_3_all << "\t ( " << (double)count_out_2_fail_3_all * 100 / count_out_2_fail_all << " % )" << endl;

	cout << endl;
	cout << "ͳ�ƽ����" << endl;
	cout << "������Ʒ�ģ�ͣ����ף�    ׼ȷ�ʣ�" << kk_1 * 100 << "%  \t�ٻ��ʣ�" << kk_2 * 100 << "%" << endl;
	cout << "������Ʒ�ģ�ͣ����ף�    ׼ȷ�ʣ�" << kk_3 * 100 << "%  \t�ٻ��ʣ�" << kk_4 * 100 << "%" << endl;


	return 0;
}
