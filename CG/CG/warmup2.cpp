#include<iostream>
#include<fstream>
#include<vector>
#include<algorithm>
#include<string>

using namespace std;

//�޴� ���
char menu() {
	cout << "a: ��� ������ �빮�ڷ� �ٲ� ���\n"
		<< "b: �� ���� �ܾ��� ������ ���\n"
		<< "c: �빮�ڷ� ���۵Ǵ� �ܾ ã�� �� �ܾ �ٸ� ������ ����ϰ�, �� �� �ִ����� ����Ͽ� ���\n"
		<< "d: ���� �� �� ��ü�� �Ųٷ� ���\n"
		<< "e: ��� ���鿡 ��/�� ���� ����\n"
		<< "f: ������ �������� (/�� ���ԵǾ� �ִٸ� /�� �������� ���) ��� �ܾ���� �Ųٷ� ���\n"
		<< "g: ���� ������ Ư�� ���ڸ� �ٸ� ���ڷ� �ٲٱ�\n"
		<< "h: ���忡 �ִ� ���ڸ� ã�� ���� �ڿ� ���� ������ ���� �ٷ� �ѱ��\n"
		<< "i: �� ������ ���ĺ��� ������ ���� ������������ �����Ͽ� ������� ����Ѵ�. �ٽ� ������ ������������ ���\n"
		<< "J: ��ɾ�� �ܾ �Է��ϸ�, �Է� ���� ������� ��� ����ϸ鼭 �� �ܾ ã�� �ٸ� ������ ����ϰ�, �� �� �ִ����� ����Ͽ� ���\n"
		<< "q: ���α׷� ����\n"
		<< "�޴� �Է�: ";
	char input;
	cin >> input;
	return input;
}

int main() {
	cout << "���ϸ� �Է�: ";
	string Filename;
	if (!(cin >> Filename)) {
		cerr << "�Է� ����\n";
		return 1;
	}

	ifstream fin(Filename);
	if (!fin) {
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
			cout << "��� ������ �빮�ڷ� �ٲ� ���\n";
			for (int i = 0; i < line.size(); i++) {
				if (line[i] >= 'a' && line[i] <= 'z')
					line[i] = line[i] - ('a' - 'A');
			}
			break;
		}
		case'b': {
			cout << "�� ���� �ܾ��� ������ ���\n";
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
			cout << "�ܾ� ����: " << wordcount << "\n";
			break;
		}
		case'c':
			cout << ": �빮�ڷ� ���۵Ǵ� �ܾ ã�� �� �ܾ �ٸ� ������ ����ϰ�, �� �� �ִ����� ����Ͽ� ���\n";

			break;
		case'd':
			cout << ": ���� �� �� ��ü�� �Ųٷ� ���\n";
			reverse(line.begin(), line.end());
			cout << line << "\n";
			break;
		case 'e':
			cout << ": ��� ���鿡 ��/�� ���� ����\n";
			for (int i = 0; i < line.size(); i++) {
				if (line[i] == ' ')
					line[i] = '/';
			}
			cout << line << "\n";
			break;
		case 'f':
			cout << ": ������ �������� ��� �ܾ���� �Ųٷ� ���\n";
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
			cout << ": ���� ������ Ư�� ���ڸ� �ٸ� ���ڷ� �ٲٱ�\n";
			char oldchar, newchar;
			cout << "�ٲ� ���� �Է�: ";
			cin >> oldchar;
			cout << "�� ���� �Է�: ";
			cin >> newchar;
			for (int i = 0; i < line.size(); i++) {
				if (line[i] == oldchar)
					line[i] = newchar;
			}
			cout << line << "\n";
			break;
		case'h':
			cout << "���忡 �ִ� ���ڸ� ã�� ���� �ڿ� ���� ������ ���� �ٷ� �ѱ��\n";
			for (int i = 0; i < line.size(); i++) {
				if (line[i] >= '0' && line[i] <= '9') {
					cout << line.substr(0, i + 1) << "\n";
					line = line.substr(i + 1);
					i = 0;
				}
			}
			break;
		case'i':
			cout << ": �� ������ ���ĺ��� ������ ���� ������������ �����Ͽ� ������� ����Ѵ�. �ٽ� ������ ������������ ���\n";
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