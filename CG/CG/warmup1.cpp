#include<iostream>
#include<vector>
#include<random>
#include<iomanip>


using namespace std;

random_device random;
mt19937 engine(random());
uniform_int_distribution<int> distribution(1, 9);


//행렬 생성
vector<vector<int>> randmatrix() {
	vector<vector<int>> M(4, vector<int>(4));
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			M[i][j] = distribution(engine);
		}
	}
	return M;
}

//행렬 출력
void printmatrix(const vector<vector<int>>& M) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			cout << setw(3) << M[i][j] << " ";
		}
		cout << "\n";
	}
	cout << "\n";
}

//행렬 덧셈
vector<vector<int>> addmatrix(const vector<vector<int>>& A, const vector<vector<int>>& B) {
	vector<vector<int>> C(4, vector<int>(4));
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			C[i][j] = A[i][j] + B[i][j];
		}
	}
	return C;
}
//행렬 뺄셈
vector<vector<int>> submatrix(const vector<vector<int>>& A, const vector<vector<int>>& B) {
	vector<vector<int>> C(4, vector<int>(4));
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			C[i][j] = A[i][j] - B[i][j];
		}
	}
	return C;
}
//행렬 곱셈
vector<vector<int>> addmatrix(const vector<vector<int>>& A, const vector<vector<int>>& B) {
	vector<vector<int>> C(4, vector<int>(4));
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			C[i][j] = A[i][j] * B[i][j];
		}
	}
	return C;
}
//행렬식 값
double determinant(const vector<vector<int>>& M) {
	double det = 0;
	det += M[0][0] * (M[1][1] * (M[2][2] * M[3][3] - M[2][3] * M[3][2]) - M[1][2] * (M[2][1] * M[3][3] - M[2][3] * M[3][1]) + M[1][3] * (M[2][1] * M[3][2] - M[2][2] * M[3][1]));
	det -= M[0][1] * (M[1][0] * (M[2][2] * M[3][3] - M[2][3] * M[3][2]) - M[1][2] * (M[2][0] * M[3][3] - M[2][3] * M[3][0]) + M[1][3] * (M[2][0] * M[3][2] - M[2][2] * M[3][0]));
	det += M[0][2] * (M[1][0] * (M[2][1] * M[3][3] - M[2][3] * M[3][1]) - M[1][1] * (M[2][0] * M[3][3] - M[2][3] * M[3][0]) + M[1][3] * (M[2][0] * M[3][1] - M[2][1] * M[3][0]));
	det -= M[0][3] * (M[1][0] * (M[2][1] * M[3][2] - M[2][2] * M[3][1]) - M[1][1] * (M[2][0] * M[3][2] - M[2][2] * M[3][0]) + M[1][2] * (M[2][0] * M[3][1] - M[2][1] * M[3][0]));
	return det;
}

//전치 행렬
vector<vector<int>> transmatrix(const vector<vector<int>>& M) {
	vector<vector<int>> T(4, vector<int>(4));
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			T[i][j] = M[j][i];
		}
	}
	return T;
}

//최소값 빼기
vector<vector<int>> minmatrix(const vector<vector<int>>& M) {
	int min = M[0][0];
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (M[i][j] < min)
				min = M[i][j];
		}
	}
	vector<vector<int>> N(4, vector<int>(4));
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			N[i][j] = M[i][j] - min;
		}
	}
	return N;
}

//최대값 더하기
vector<vector<int>> maxmatrix(const vector<vector<int>>& M) {
	int max = M[0][0];
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (M[i][j] > max)
				max = M[i][j];
		}
	}
	vector<vector<int>> N(4, vector<int>(4));
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			N[i][j] = M[i][j] + max;
		}
	}
	return N;
}
//배수 출력
vector<vector<int>> filtermatrix(const vector<vector<int>>& M, int n) {
	vector<vector<int>> N(4, vector<int>(4));
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {

		}
	}
}


//초기화
void init() {


}

//메뉴 출력
void menu() {
	cout << "m: 행렬의 곱셈\n"
		<< "a: 행렬의 덧셈\n"
		<< "d: 행렬의 뺄셈\n"
		<< "r: 행렬식의 값\n"
		<< "t: 전치 행렬\n"
		<< "e: 행렬의 값 중에서 최소값을 찾아 그 값을 행렬의 모든 값에 뺀다\n"
		<< "f: 행렬의 값 중에서 최대값을 찾아 그 값을 행렬의 모든 값에 더한다\n"
		<< "숫자 (1~9): 입력한 숫자의 배수만 출력\n"
		<< "s: 행렬의 값을 새로 랜덤하게 설정\n"
		<< "q: 프로그램 종료\n";
}

int main() {
	vector<vector<int>> A = randmatrix();
	vector<vector<int>> B = randmatrix();
	printmatrix(A);
	printmatrix(B);
	menu();
	return 0;
}
