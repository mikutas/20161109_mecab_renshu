#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <mecab.h>

using namespace std;

vector<string> split(string s, string c) {
	vector<string> ret;
	for (int i = 0, n; i <= s.length(); i = n + 1) {
		n = s.find_first_of(c, i);
		if (n == string::npos)n = s.length();
		string tmp = s.substr(i, n - i);
		ret.push_back(tmp);
	}
	return ret;
}

int main() {
	cout << "How many writers?" << endl;
	int writers;
	cin >> writers;
	cout << "I got it." << endl;

	// 作家ごとのTerm Frequency
	vector<multimap<double, string>> tf_map_vec;
	// Document Frequency
	map<string, int> doc_freq;

	for (int i = 0; i < writers; i++) {
		int total_words = 0;
		map<string, int> word_count;
		multimap<double, string> term_freq;
		map<string, int>::iterator it;

		// 文書の読み込み
		string input_file;
		string str;
		cout << "Drop file here and press ENTER." << endl;
		cin >> input_file;
		while (input_file != "x") {
			ifstream ifs(input_file);
			if (ifs.fail()) {
				cout << "Cannot open file." << endl;
				exit(1);
			}
			char c;
			while (ifs.get(c)) {
				str.push_back(c);
			}
			cout << "next?" << endl;
			cin >> input_file;
		}

		// 形態素解析
		MeCab::Tagger *tagger = MeCab::createTagger("");
		const MeCab::Node *node = tagger->parseToNode(str.c_str());

		// 単語出現数とdfのカウント
		for (node = node->next; node->next; node = node->next) {
			vector<string> strvec = split(node->feature, ",");
			total_words++;
			if (strvec[0] == "名詞" /*| strvec[0] == "動詞"*/) {
				//cout << "counting..." << endl;
				string noun = strvec[6];
				it = doc_freq.find(noun);
				if (it != doc_freq.end()) {
					if (it->second < i) it->second++;
				}
				else {
					doc_freq.insert(pair<string, int>(noun, 1));
				}
				it = word_count.find(noun);
				if (it != word_count.end()) {
					it->second += 1;
				}
				else {
					word_count.insert(pair<string, int>(noun, 1));
				}
			}
		}
		delete tagger;
		
		// tfの計算
		for (it = word_count.begin(); it != word_count.end(); it++) {
			term_freq.insert(pair<double, string>((double)(it->second) / total_words, it->first));
		}
		tf_map_vec.push_back(term_freq);
	}

	// tf-idfの計算
	vector<multimap<double, string>> tf_idf_vec;
	for (int i = 0; i < writers; i++) {
		multimap<double, string> tf_idf;
		map<double, string>::iterator it;
		for (it = tf_map_vec.at(i).begin(); it != tf_map_vec.at(i).end(); it++) {
			double idf = log(writers / (double)(doc_freq.find(it->second)->second));
			double tf_idf_val = (it->first) * idf;
			tf_idf.insert(pair<double, string>(tf_idf_val, it->second));
		}
		tf_idf_vec.push_back(tf_idf);
	}

	// 作家ごとにtf-idfトップいくつかを表示
	cout << "How many words?" << endl;
	int display_words;
	cin >> display_words;
	for (int i = 0; i < writers; i++) {
		cout << "Writer no." << i + 1 << endl;
		multimap<double, string>::reverse_iterator rit;
		int j = 0;
		for (rit = tf_idf_vec.at(i).rbegin(); j < min(tf_idf_vec.at(i).size(), display_words); rit++) {
			cout << rit->second << "\t" << rit->first << endl;
			j++;
		}
		cout << endl;
	}
	return 0;
}