#include<iostream>

using namespace std;

const int SIZE = 6;
char board[SIZE][SIZE];

// �޴� ���
char printmenu() {
	cout << "r: ������ �����ϰ� �ٽ� ����\n"
		<< "h: ����ĭ�� ��� ���ڵ��� ��� �����ְ� �ٽ� ������� ���\n"
		<< "q: ���� ����\n";
	char input;
	cin >> input;
	return input;
}

//���� ����
void initboard() {
	//�ε���	�ʱ�ȭ
	for (int i = 0; i < SIZE; i++)
		board[i+1][0] = '1'+i;
	for(int i = 0; i < SIZE-1; i++)
		board[0][i+1] = 'A'+i;
	//���� �ʱ�ȭ
	for(int i = 1; i < SIZE; i++)
		for(int j = 1; j < SIZE; j++)
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