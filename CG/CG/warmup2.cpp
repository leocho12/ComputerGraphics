#include<iostream>
#include<fstream>
#include<vector>
#include<algorithm>
#include<string>

using namespace std;

//메뉴 출력
char menu() {
	cout << "a: 모든 문장을 대문자로 바꿔 출력\n"
		<< "b: 각 줄의 단어의 개수를 출력\n"
		<< "c: 대문자로 시작되는 단어를 찾아 그 단어를 다른 색으로 출력하고, 몇 개 있는지를 계산하여 출력\n"
		<< "d: 문장 한 줄 전체를 거꾸로 출력\n"
		<< "e: 모든 공백에 “/” 문자 삽입\n"
		<< "f: 공백을 기준으로 (/가 삽입되어 있다면 /를 공백으로 취급) 모든 단어들을 거꾸로 출력\n"
		<< "g: 문자 내부의 특정 문자를 다른 문자로 바꾸기\n"
		<< "h: 문장에 있는 숫자를 찾아 숫자 뒤에 오는 문장을 다음 줄로 넘긴다\n"
		<< "i: 한 문장의 알파벳의 개수에 따라 오름차순으로 정렬하여 문장들을 출력한다. 다시 누르면 내림차순으로 출력\n"
		<< "J: 명령어와 단어를 입력하면, 입력 받은 문장들을 모두 출력하면서 그 단어를 찾아 다른 색으로 출력하고, 몇 개 있는지를 계산하여 출력\n"
		<< "q: 프로그램 종료\n"
		<< "메뉴 입력: ";
	char input;
	cin >> input;
	return input;
}

int main() {
	cout << "파일명 입력: ";
	string Filename;
	if (!(cin >> Filename)) {
		cerr << "입력 없음\n";
		return 1;
	}

	ifstream fin(Filename);
	if (!fin) {
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
	for (int i = 0; i < lines.size(); i++) {
		cout << lines[i] << "\n";
	}
	return 0;

	while (1) {
		menu();
		char input;
		cin >> input;

		switch (input) {
		case'a': {
			cout << "모든 문장을 대문자로 바꿔 출력\n";
			for (int i = 0; i < line.size(); i++) {
				if (line[i] >= 'a' && line[i] <= 'z')
					line[i] = line[i] - ('a' - 'A');
			}
			break;
		}
		case'b': {
			cout << "각 줄의 단어의 개수를 출력\n";
			int wordcount = 0;
			bool inword = false;
			for (int i = 0; i < line.size(); i++) {
				if (line[i] != ' ' && !inword) {
					inword = true;
					wordcount++;
				}
				else if (line[i] == ' ' && inword) {
					inword = false;
				}
			}
			cout << "단어 개수: " << wordcount << "\n";
			break;
		}
		case'c':
			cout << ": 대문자로 시작되는 단어를 찾아 그 단어를 다른 색으로 출력하고, 몇 개 있는지를 계산하여 출력\n";

			break;
		case'd':
			cout << ": 문장 한 줄 전체를 거꾸로 출력\n";
			reverse(line.begin(), line.end());
			cout << line << "\n";
			break;
		case 'e':
			cout << ": 모든 공백에 “/” 문자 삽입\n";
			for (int i = 0; i < line.size(); i++) {
				if (line[i] == ' ')
					line[i] = '/';
			}
			cout << line << "\n";
			break;
		case 'f':
			cout << ": 공백을 기준으로 모든 단어들을 거꾸로 출력\n";
			for (int i = 0; i < line.size(); i++) {
				if (line[i] == ' ' || line[i] == '/') {
					reverse(line.begin(), line.begin() + i);
					cout << line.substr(0, i) << ' ';
					line = line.substr(i + 1);
					i = 0;
				}
			}
			break;
		case 'g':
			cout << ": 문자 내부의 특정 문자를 다른 문자로 바꾸기\n";
			char oldchar, newchar;
			cout << "바꿀 문자 입력: ";
			cin >> oldchar;
			cout << "새 문자 입력: ";
			cin >> newchar;
			for (int i = 0; i < line.size(); i++) {
				if (line[i] == oldchar)
					line[i] = newchar;
			}
			cout << line << "\n";
			break;
		case'h':
			cout << "문장에 있는 숫자를 찾아 숫자 뒤에 오는 문장을 다음 줄로 넘긴다\n";
			for (int i = 0; i < line.size(); i++) {
				if (line[i] >= '0' && line[i] <= '9') {
					cout << line.substr(0, i + 1) << "\n";
					line = line.substr(i + 1);
					i = 0;
				}
			}
			break;
		case'i':
			cout << ": 한 문장의 알파벳의 개수에 따라 오름차순으로 정렬하여 문장들을 출력한다. 다시 누르면 내림차순으로 출력\n";
			static bool ascending = true;
			if (ascending) {
				sort(lines.begin(), lines.end(), [](const string& a, const string& b) {
					int countA = count_if(a.begin(), a.end(), [](char c) { return isalpha(c); });
					int countB = count_if(b.begin(), b.end(), [](char c) { return isalpha(c); });
					return countA < countB;
					});
			}
			else {
				sort(lines.begin(), lines.end(), [](const string& a, const string& b) {
					int countA = count_if(a.begin(), a.end(), [](char c) { return isalpha(c); });
					int countB = count_if(b.begin(), b.end(), [](char c) { return isalpha(c); });
					return countA > countB;
					});
			}
			break;

		}
	}