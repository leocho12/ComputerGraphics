#include <iostream>
#include <vector>
#include <algorithm>
#include <Windows.h>
#include <random>
#include <conio.h>

using namespace std;

const int BOARD_SIZE = 30;
char board[BOARD_SIZE][BOARD_SIZE];
int playerX = 0, playerY = 0;

// 보드 생성
void CreateBoard() {
    vector<char> boards(BOARD_SIZE * BOARD_SIZE, '0');
    for (int i = 0; i < 4; ++i) {
        boards[i] = 'X';
    }

    random_device rd;
    mt19937 gen(rd());
    shuffle(boards.begin(), boards.end(), gen);

    
    while (boards[0] == 'X') {
        shuffle(boards.begin(), boards.end(), gen);
    }

    int index = 0;
    for (int i = 0; i < BOARD_SIZE; i++)
        for (int j = 0; j < BOARD_SIZE; j++)
            board[i][j] = boards[index++];

    playerX = 0;
    playerY = 0;
}

// 보드 출력
void PrintBoard() {
    system("cls");
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == 'X') {
                SetConsoleTextAttribute(h, 12);
            }
            else if (i == playerX && j == playerY) {
                SetConsoleTextAttribute(h, 10);
                cout << "* ";
				continue;
            }
            else if (board[i][j] >= '1' && board[i][j] <= '9') {
                SetConsoleTextAttribute(h, 11);
            }
            else if (board[i][j] >= 'A') {
                SetConsoleTextAttribute(h, 9);
            }
            else {
                SetConsoleTextAttribute(h, 8);
            }
            cout << board[i][j] << ' ';
        }
        cout << '\n';
    }

    SetConsoleTextAttribute(h, 7);
    cout << "\nwasd: 이동, Enter: 리셋, ESC: 종료\n";
}

// 이동 처리
void MovePlayer(int dx, int dy) {
    int nx = playerX + dx;
    int ny = playerY + dy;

    if (nx < 0 || ny < 0 || nx >= BOARD_SIZE || ny >= BOARD_SIZE) return;

    if (board[nx][ny] == 'X') return;

   
    char& cell = board[playerX][playerY];
    if (cell == '0') {
        board[playerX][playerY] = '1';
    }
    else if (cell >= '1' && cell <= '8') {
        board[playerX][playerY] += 1;
    }
    else if (cell == '9') {
        board[playerX][playerY] = 'A';
    }
    else if (cell >= 'A' && cell < 'Z') {
        board[playerX][playerY] += 1;
    }

    playerX = nx;
    playerY = ny;
}

// 메인 함수
int main() {
    CreateBoard();
    PrintBoard();

    while (true) {
        int key = _getch();

        if (key == 27) break;

        if (key == 'w') MovePlayer(-1, 0);
        else if (key == 's') MovePlayer(1, 0);
        else if (key == 'a') MovePlayer(0, -1);
        else if (key == 'd') MovePlayer(0, 1);
        else if (key == 13) CreateBoard();

        PrintBoard();
    }

    return 0;
}
