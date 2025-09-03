#include<iostream>
#include<fstream>
#include<vector>
#include<algorithm>
#include<string>

using namespace std;

int main() {
	cout << "파일명 입력: ";
	string Filename;
	if(!(cin >> Filename)) {
		cerr << "입력 없음\n";
		return 1;
	}

	ifstream fin(Filename);
	if(!fin) {
		cerr << "파일 열기 실패\n";
		return 1;
	}

	vector<string> lines;
	lines.reserve(10);
	string line;
	while (getline(fin, line)) {
		lines.push_back(line);
		if (lines.size() >= 10)break;
	}
	fin.close();

	cout << "읽은 문자열 출력\n";
	for(int i=0;i<lines.size(); i++) {
		cout << lines[i] << "\n";
	}
	return 0;
}