#include<iostream>

using namespace std;

const int SIZE = 6;
char board[SIZE][SIZE];

// 메뉴 출력
char printmenu() {
	cout << "r: 게임을 리셋하고 다시 시작\n"
		<< "h: 보드칸의 모든 문자들을 잠시 보여주고 다시 원래대로 출력\n"
		<< "q: 게임 종료\n";
	char input;
	cin >> input;
	return input;
}

//보드 생성
void initboard() {
	//인덱스	초기화
	for (int i = 0; i < SIZE; i++)
		board[i+1][0] = '1'+i;
	for(int i = 0; i < SIZE-1; i++)
		board[0][i+1] = 'A'+i;
	//보드 초기화
	for(int i = 1; i < SIZE; i++)
		for(int j = 1; j < SIZE; j++)
			board[i][j] = '*';

}

//보드 출력
void printboard() {
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			cout << board[i][j] << ' ';
		}
		cout << '\n';
	}
	return;
}

int main() {
	initboard();
	printboard();
	char menu = printmenu();
	
	switch (menu)
	{
	case'r':

		break;
	case'h':

		break;
	case'q':
		return 0;
		break;
	default:
		break;
	}

	return 0;
}