#include<iostream>
#include<fstream>
#include<vector>
#include<algorithm>
#include<string>
#include<cctype>
#include<windows.h>

using namespace std;

void setColor(int color) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

// 메뉴 출력
char menu() {
	cout << "a: 모든 문장을 대문자로 바꿔 출력 (토글)\n"
		<< "b: 각 줄의 단어의 개수를 출력\n"
		<< "c: 대문자로 시작되는 단어를 색 강조하고 개수 출력 (토글)\n"
		<< "d: 문장 전체 거꾸로 출력 (토글)\n"
		<< "e: 공백을 '/'로 치환 (토글)\n"
		<< "f: 단어들을 거꾸로 출력 (토글)\n"
		<< "g: 문자 치환 (토글)\n"
		<< "h: 숫자 뒤 문자열을 다음 줄로 넘김 (토글)\n"
		<< "i: 알파벳 개수 기준 정렬 (오름->내림->원래)\n"
		<< "J: 단어 강조 검색\n"
		<< "q: 종료\n"
		<< "메뉴 입력: ";
	char input;
	cin >> input;
	return input;
}

int main() {
	cout << "파일명 입력: ";
	string filename;
	cin >> filename;
	ifstream fin(filename);
	if (!fin) {
		cerr << "파일 열기 실패\n";
		return 1;
	}

	vector<string> original;
	string line;
	while (getline(fin, line)) {
		while (line.find("  ") != string::npos)
			line.replace(line.find("  "), 2, " ");
		original.push_back(line);
		if (original.size() >= 10) break;
	}
	fin.close();

	vector<string> lines = original;
	bool upper = false, reverseLine = false, slash = false, reverseWords = false, highlightCaps = false, replaceChar = false, splitByDigit = false;
	char oldChar, newChar;
	int sortState = 0;

	while (true) {
		char input = menu();
		switch (input) {
		case 'q':
			return 0;
		case 'a':
			upper = !upper;
			break;
		case 'b': {
			cout << "\n단어 개수:\n";
			for (auto& s : lines) {
				int wc = count(s.begin(), s.end(), ' ') + 1;
				cout << s << " (" << wc << ")\n";
			}
			continue;
		}
		case 'c':
			highlightCaps = !highlightCaps;
			break;
		case 'd':
			reverseLine = !reverseLine;
			break;
		case 'e':
			slash = !slash;
			break;
		case 'f':
			reverseWords = !reverseWords;
			break;
		case 'g':
			if (!replaceChar) {
				cout << "바꿀 문자 입력: "; cin >> oldChar;
				cout << "새 문자 입력: "; cin >> newChar;
			}
			replaceChar = !replaceChar;
			break;
		case 'h': {
			splitByDigit = !splitByDigit;
			break;
		}
		case 'i':
			sortState = (sortState + 1) % 3;
			break;
		case 'j': {
			string word;
			cout << "찾을 단어 입력: "; cin >> word;
			int count = 0;
			HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
			for (auto& s : lines) {
				string s_lower = s;
				string word_lower = word;
				transform(s_lower.begin(), s_lower.end(), s_lower.begin(), ::tolower);
				transform(word_lower.begin(), word_lower.end(), word_lower.begin(), ::tolower);
				size_t pos = 0;
				while (pos < s.size()) {
					size_t found = s_lower.find(word_lower, pos);
					if (found == string::npos) {
						cout << s.substr(pos);
						break;
					}
					cout << s.substr(pos, found - pos);
					SetConsoleTextAttribute(h, 12);
					cout << s.substr(found, word.length());
					SetConsoleTextAttribute(h, 7);
					pos = found + word.length();
					count++;
				}
				cout << "\n";
			}
			cout << "총 " << count << "개 발견\n";
			continue;
		}
		default:
			cout << "유효하지 않은 입력입니다.\n";
			continue;
		}

		lines = original;
		if (replaceChar) for (auto& s : lines) for (auto& c : s) if (c == oldChar) c = newChar;
		if (upper) for (auto& s : lines) for (auto& c : s) c = toupper(c);
		if (slash) for (auto& s : lines) for (auto& c : s) if (c == ' ') c = '/';
		if (reverseWords) for (auto& s : lines) {
			vector<string> words;
			string word;
			for (char c : s) {
				if (c == ' ' || c == '/') {
					if (!word.empty()) words.push_back(word);
					word = "";
				}
				else word += c;
			}
			if (!word.empty()) words.push_back(word);
			reverse(words.begin(), words.end());
			s = "";
			for (int i = 0; i < words.size(); ++i) {
				if (i > 0) s += slash ? '/' : ' ';
				s += words[i];
			}
		}
		if (reverseLine) for (auto& s : lines) reverse(s.begin(), s.end());
		if (splitByDigit) {
			vector<string> newLines;
			for (auto& s : lines) {
				string current = "";
				for (char c : s) {
					if (isdigit(c)) {
						if (!current.empty()) {
							newLines.push_back(current);
							current.clear();
						}
						newLines.push_back(string(1, c));
					}
					else {
						current += c;
					}
				}
				if (!current.empty()) newLines.push_back(current);
			}
			lines = newLines;
		}
		if (sortState == 1) sort(lines.begin(), lines.end(), [](string a, string b) {
			return count_if(a.begin(), a.end(), ::isalpha) < count_if(b.begin(), b.end(), ::isalpha);
			});
		else if (sortState == 2) sort(lines.begin(), lines.end(), [](string a, string b) {
			return count_if(a.begin(), a.end(), ::isalpha) > count_if(b.begin(), b.end(), ::isalpha);
			});

		cout << "\n출력 결과:\n";
		for (auto& s : lines) {
			if (highlightCaps) {
				string word;
				for (int i = 0; i <= s.size(); ++i) {
					if (i == s.size() || s[i] == ' ' || s[i] == '/') {
						if (!word.empty() && isupper(word[0])) {
							setColor(12); cout << word; setColor(7);
						}
						else cout << word;
						if (i != s.size()) cout << s[i];
						word = "";
					}
					else word += s[i];
				}
				cout << "\n";
			}
			else cout << s << "\n";
		}
		cout << "\n";
	}
	return 0;
}
