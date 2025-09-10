#include<iostream>

using namespace std;

const int SIZE = 30;
char board[SIZE][SIZE];

//보드 생성
void initboard() {
	//보드 초기화
	for (int i = 1; i < SIZE; i++)
		for (int j = 1; j < SIZE; j++)
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
}

// 메뉴 출력
char menu() {
	cout << "x/X: x축 우측/좌측으로 한 칸 이동\n"
		<< "y/Y: y축 아래쪽/위쪽으로 한 칸 이동\n"
		<< "s/S: x와 y 모두 한 칸 축소/확대 \n"
		<< "i/I: x축 한 칸 확대/축소\n"
		<< "j/J: y축 한 칸 확대/축소\n"
		<< "a/A: x축 한 칸 확대, y축 한 칸 축소 / x축 한 칸 축소, y축 한 칸 확대\n"
		<< "b/B: 사각형의 면적 출력\n"
		<< "c: 보드칸을 x축, y축 모두 한 칸씩 늘린다. 최대 10칸\n"
		<< "d: 보드칸을 x축, y축 모두 한 칸씩 줄인다. 최대 20칸\n"
		<< "r: 모든 것을 리셋 하고 다시 좌표 값을 입력\n"
		<< "q: 종료\n"
		<< "메뉴 입력: ";
	char input;
	cin >> input;
	return input;
}

int main() {

	char menuinput = menu();

	switch (menuinput) {
	case'x':

		break;
	case'y':

		break;
	case's':

		break;
	case'i':

		break;
	case'j':

		break;
	case'a':

		break;
	case'b':

		break;
	case'X':

		break;
	case'Y':

		break;
	case'S':

		break;
	case'I':

		break;
	case'J':

		break;
	case'A':

		break;
	case'B':

		break;
	case'c':

		break;
	case'd':

		break;
	case'r':

		break;
	case'q':

		break;
	}

	return 0;
}