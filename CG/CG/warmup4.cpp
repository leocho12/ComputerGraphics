#include<iostream>
#include<vector>
#include<algorithm>
#include<cstdlib>
#include<chrono>
#include<thread>

using namespace std;

const int SIZE = 6;
char board[SIZE][SIZE];
char answer[5][5];
bool revealed[5][5];

//�ε��� �ʱ�ȭ
void initindex() {
	for (int i = 0; i < SIZE; i++)
		board[i + 1][0] = '1' + i;
	for (int i = 0; i < SIZE - 1; i++)
		board[0][i + 1] = 'a' + i;
}

//���� �ʱ�ȭ
void initchar() {
	vector<char> cards;
	for (char c = 'a'; c < 'a' + 12; c++) {
		cards.push_back(c);
		cards.push_back(c);
	}
	cards.push_back('@');

	random_shuffle(cards.begin(), cards.end());

	//���� ��ġ
	int index = 0;
	for (int i = 0; i < 5; i++)
		for (int j = 0; j < 5; j++)
			answer[i][j] = cards[index++];

}

//���� ����
void initboard() {
	initindex();
	initchar();
	//���� �ʱ�ȭ
	for (int i = 1; i < SIZE; i++)
		for (int j = 1; j < SIZE; j++)
			board[i][j] = '*';

	for (int i = 0; i < 5; i++)
		for (int j = 0; j < 5; j++)
			revealed[i][j] = false;
}

//���� ���
void printboard() {
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			cout << board[i][j] << ' ';
		}
		cout << '\n';
	}
}

//��ǥ�Է�
bool inputcoord(int& row, int& col) {
	string input;
	cout << "��ǥ �Է� (ex: a1): ";
	cin >> input;
	if (input.length() != 2) {
		cout << "�߸��� ����\n";
		return false;
	}

	col = input[0] - 'a';
	row = input[1] - '1';

	if (row < 0 || row >= 5 || col < 0 || col >= 5) {
		cout << "�߸��� ��ǥ\n";
		return false;
	}

	if (revealed[row][col]) {
		cout << "�̹� ������ ĭ\n";
		return false;
	}
	return true;
}

//��Ʈ
void hint() {
	cout << "��Ʈ ���\n";

	for (int i = 1; i < SIZE; i++)
		for (int j = 1; j < SIZE; j++)
			board[i][j] = answer[i - 1][j - 1];

	printboard();

	this_thread::sleep_for(chrono::seconds(2));

	//������
	for (int i = 1; i < SIZE; i++) {
		for (int j = 1; j < SIZE; j++) {
			if (!revealed[i - 1][j - 1]) {
				board[i][j] = '*';
			}
		}
	}
}


//�� ��
void turn() {
	int row1, col1, row2, col2;

	cout << "ù ��° ��ǥ \n";
	if (!inputcoord(row1, col1))return;
	cout << "�� ��° ��ǥ \n";
	if (!inputcoord(row2, col2))return;

	board[row1 + 1][col1 + 1] = answer[row1][col1];
	board[row2 + 1][col2 + 1] = answer[row2][col2];
	printboard();

	char a = answer[row1][col1];
	char b = answer[row2][col2];

	if (a == b || a == '@' || b == '@') {
		cout << "��ġ\n";
		revealed[row1][col1] = true;
		revealed[row2][col2] = true;
	}
	else {
		cout << "����ġ\n";
		board[row1 + 1][col1 + 1] = '*';
		board[row2 + 1][col2 + 1] = '*';
	}
	this_thread::sleep_for(chrono::seconds(2));
}

// �޴� ���
string printmenu() {
	cout << "n: ī�� ����\n" 
		<< "r: ������ �����ϰ� �ٽ� ����\n"
		<< "h: ����ĭ�� ��� ���ڵ��� ��� �����ְ� �ٽ� ������� ���\n"
		<< "q: ���� ����\n";
	string input;
	cin >> input;
	return input;
}

int main() {
	srand((unsigned int)time(0));
	initboard();
	while (true) {
		system("cls");
		printboard();
		string menu = printmenu();

		if (menu == "r")
			initboard();
		else if (menu == "h")
			hint();
		else if (menu == "q") {
			cout << "����\n";
			return 0;
		}
		else if(menu=="n") {
			turn();
		}
	}

	return 0;
}