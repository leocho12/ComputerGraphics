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

//인덱스 초기화
void initindex() {
	for (int i = 0; i < SIZE; i++)
		board[i + 1][0] = '1' + i;
	for (int i = 0; i < SIZE - 1; i++)
		board[0][i + 1] = 'a' + i;
}

//문자 초기화
void initchar() {
	vector<char> cards;
	for (char c = 'a'; c < 'a' + 12; c++) {
		cards.push_back(c);
		cards.push_back(c);
	}
	cards.push_back('@');

	random_shuffle(cards.begin(), cards.end());

	//문자 배치
	int index = 0;
	for (int i = 0; i < 5; i++)
		for (int j = 0; j < 5; j++)
			answer[i][j] = cards[index++];

}

//보드 생성
void initboard() {
	initindex();
	initchar();
	//보드 초기화
	for (int i = 1; i < SIZE; i++)
		for (int j = 1; j < SIZE; j++)
			board[i][j] = '*';

	for (int i = 0; i < 5; i++)
		for (int j = 0; j < 5; j++)
			revealed[i][j] = false;
}

//보드 출력
void printboard() {
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			cout << board[i][j] << ' ';
		}
		cout << '\n';
	}
}

//좌표입력
bool inputcoord(int& row, int& col) {
	string input;
	cout << "좌표 입력 (ex: a1): ";
	cin >> input;
	if (input.length() != 2) {
		cout << "잘못된 형식\n";
		return false;
	}

	col = input[0] - 'a';
	row = input[1] - '1';

	if (row < 0 || row >= 5 || col < 0 || col >= 5) {
		cout << "잘못된 좌표\n";
		return false;
	}

	if (revealed[row][col]) {
		cout << "이미 공개된 칸\n";
		return false;
	}
	return true;
}

//힌트
void hint() {
	cout << "힌트 출력\n";

	for (int i = 1; i < SIZE; i++)
		for (int j = 1; j < SIZE; j++)
			board[i][j] = answer[i - 1][j - 1];

	printboard();

	this_thread::sleep_for(chrono::seconds(2));

	//가리기
	for (int i = 1; i < SIZE; i++) {
		for (int j = 1; j < SIZE; j++) {
			if (!revealed[i - 1][j - 1]) {
				board[i][j] = '*';
			}
		}
	}
}


//한 턴
void turn() {
	int row1, col1, row2, col2;

	cout << "첫 번째 좌표 \n";
	if (!inputcoord(row1, col1))return;
	cout << "두 번째 좌표 \n";
	if (!inputcoord(row2, col2))return;

	board[row1 + 1][col1 + 1] = answer[row1][col1];
	board[row2 + 1][col2 + 1] = answer[row2][col2];
	printboard();

	char a = answer[row1][col1];
	char b = answer[row2][col2];

	if (a == b || a == '@' || b == '@') {
		cout << "일치\n";
		revealed[row1][col1] = true;
		revealed[row2][col2] = true;
	}
	else {
		cout << "불일치\n";
		board[row1 + 1][col1 + 1] = '*';
		board[row2 + 1][col2 + 1] = '*';
	}
	this_thread::sleep_for(chrono::seconds(2));
}

// 메뉴 출력
string printmenu() {
	cout << "n: 카드 선택\n" 
		<< "r: 게임을 리셋하고 다시 시작\n"
		<< "h: 보드칸의 모든 문자들을 잠시 보여주고 다시 원래대로 출력\n"
		<< "q: 게임 종료\n";
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
			cout << "종료\n";
			return 0;
		}
		else if(menu=="n") {
			turn();
		}
	}

	return 0;
}