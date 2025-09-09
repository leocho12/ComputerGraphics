#include <iostream>
#include <vector>
#include <algorithm>
#include <Windows.h>
#include <random>
#include <conio.h> // _getch()

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

    // 시작 위치에 X가 안 오게
    while (boards[0] == 'X') {
        shuffle(boards.begin(), boards.end(), gen);
    }

    int index = 0;
    for (int i = 0; i < BOARD_SIZE; i++)
        for (int j = 0; j < BOARD_SIZE; j++)
            board[i][j] = boards[index++];

    // 초기 위치 표시
    playerX = 0;
    playerY = 0;
}

// 보드 출력
void PrintBoard() {
    system("cls");  // 콘솔 초기화
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == 'X') {
                SetConsoleTextAttribute(h, 12);  // Red
            }
            else if (i == playerX && j == playerY) {
                SetConsoleTextAttribute(h, 10);  // Green
                cout << "* ";
				continue;
            }
            else if (board[i][j] >= '1' && board[i][j] <= '9') {
                SetConsoleTextAttribute(h, 7);   // Light gray
            }
            else {
                SetConsoleTextAttribute(h, 8);   // Dark gray for '0'
            }
            cout << board[i][j] << ' ';
        }
        cout << '\n';
    }

    SetConsoleTextAttribute(h, 7); // Reset color
    cout << "\nwasd로 이동, Enter로 리셋, ESC로 종료\n";
}

// 이동 처리 함수
void MovePlayer(int dx, int dy) {
    int nx = playerX + dx;
    int ny = playerY + dy;

    // 경계 확인
    if (nx < 0 || ny < 0 || nx >= BOARD_SIZE || ny >= BOARD_SIZE) return;

    // 장애물이면 이동 안 됨
    if (board[nx][ny] == 'X') return;

   
    char& cell = board[playerX][playerY];
    if (cell == '0') {
        board[playerX][playerY] = '1';
    }
    else if (cell >= '1' && cell <= '8') {
        board[playerX][playerY] += 1; // '1'→'2'→'3'...
    }
    else if (cell == '9') {
        board[playerX][playerY] = 'A'; // 10번째 이상: A, B, ...
    }
    else if (cell >= 'A' && cell < 'Z') {
        board[playerX][playerY] += 1;
    }

    // 새로운 위치로 이동
    playerX = nx;
    playerY = ny;
}

// 메인 함수
int main() {
    CreateBoard();
    PrintBoard();

    while (true) {
        int key = _getch();

        if (key == 27) break; // ESC

        if (key == 'w') MovePlayer(-1, 0);
        else if (key == 's') MovePlayer(1, 0);
        else if (key == 'a') MovePlayer(0, -1);
        else if (key == 'd') MovePlayer(0, 1);
        else if (key == 13) CreateBoard();  // Enter key (리셋)

        PrintBoard();
    }

    return 0;
}
