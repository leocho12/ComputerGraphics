#include<iostream>
#include<fstream>
#include<vector>
#include<algorithm>
#include<string>

using namespace std;

int main() {
	cout << "���ϸ� �Է�: ";
	string Filename;
	if(!(cin >> Filename)) {
		cerr << "�Է� ����\n";
		return 1;
	}

	ifstream fin(Filename);
	if(!fin) {
		cerr << "���� ���� ����\n";
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

	cout << "���� ���ڿ� ���\n";
	for(int i=0;i<lines.size(); i++) {
		cout << lines[i] << "\n";
	}
	return 0;
}