#include<iostream>

using namespace std;

const int SIZE = 30;
char board[SIZE][SIZE];

//���� ����
void initboard() {
	//���� �ʱ�ȭ
	for (int i = 1; i < SIZE; i++)
		for (int j = 1; j < SIZE; j++)
			board[i][j] = '*';

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

// �޴� ���
char menu() {
	cout << "x/X: x�� ����/�������� �� ĭ �̵�\n"
		<< "y/Y: y�� �Ʒ���/�������� �� ĭ �̵�\n"
		<< "s/S: x�� y ��� �� ĭ ���/Ȯ�� \n"
		<< "i/I: x�� �� ĭ Ȯ��/���\n"
		<< "j/J: y�� �� ĭ Ȯ��/���\n"
		<< "a/A: x�� �� ĭ Ȯ��, y�� �� ĭ ��� / x�� �� ĭ ���, y�� �� ĭ Ȯ��\n"
		<< "b/B: �簢���� ���� ���\n"
		<< "c: ����ĭ�� x��, y�� ��� �� ĭ�� �ø���. �ִ� 10ĭ\n"
		<< "d: ����ĭ�� x��, y�� ��� �� ĭ�� ���δ�. �ִ� 20ĭ\n"
		<< "r: ��� ���� ���� �ϰ� �ٽ� ��ǥ ���� �Է�\n"
		<< "q: ����\n"
		<< "�޴� �Է�: ";
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