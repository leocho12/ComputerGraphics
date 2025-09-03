#include<iostream>
#include<vector>
#include<random>
#include<iomanip>


using namespace std;

random_device random;
mt19937 engine(random());
uniform_int_distribution<int> distribution(1, 9);


//��� ����
vector<vector<int>> randmatrix() {
	vector<vector<int>> M(4, vector<int>(4));
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			M[i][j] = distribution(engine);
		}
	}
	return M;
}

//��� ���
void printmatrix(const vector<vector<int>>& M) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			cout << setw(3) << M[i][j] << " ";
		}
		cout << "\n";
	}
	cout << "\n";
}

//��� ����
vector<vector<int>> addmatrix(const vector<vector<int>>& A, const vector<vector<int>>& B) {
	vector<vector<int>> C(4, vector<int>(4));
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			C[i][j] = A[i][j] + B[i][j];
		}
	}
	return C;
}
//��� ����
vector<vector<int>> submatrix(const vector<vector<int>>& A, const vector<vector<int>>& B) {
	vector<vector<int>> C(4, vector<int>(4));
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			C[i][j] = A[i][j] - B[i][j];
		}
	}
	return C;
}
//��� ����
vector<vector<int>> addmatrix(const vector<vector<int>>& A, const vector<vector<int>>& B) {
	vector<vector<int>> C(4, vector<int>(4));
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			C[i][j] = A[i][j] * B[i][j];
		}
	}
	return C;
}
//��Ľ� ��
double determinant(const vector<vector<int>>& M) {
	double det = 0;
	det += M[0][0] * (M[1][1] * (M[2][2] * M[3][3] - M[2][3] * M[3][2]) - M[1][2] * (M[2][1] * M[3][3] - M[2][3] * M[3][1]) + M[1][3] * (M[2][1] * M[3][2] - M[2][2] * M[3][1]));
	det -= M[0][1] * (M[1][0] * (M[2][2] * M[3][3] - M[2][3] * M[3][2]) - M[1][2] * (M[2][0] * M[3][3] - M[2][3] * M[3][0]) + M[1][3] * (M[2][0] * M[3][2] - M[2][2] * M[3][0]));
	det += M[0][2] * (M[1][0] * (M[2][1] * M[3][3] - M[2][3] * M[3][1]) - M[1][1] * (M[2][0] * M[3][3] - M[2][3] * M[3][0]) + M[1][3] * (M[2][0] * M[3][1] - M[2][1] * M[3][0]));
	det -= M[0][3] * (M[1][0] * (M[2][1] * M[3][2] - M[2][2] * M[3][1]) - M[1][1] * (M[2][0] * M[3][2] - M[2][2] * M[3][0]) + M[1][2] * (M[2][0] * M[3][1] - M[2][1] * M[3][0]));
	return det;
}

//��ġ ���
vector<vector<int>> transmatrix(const vector<vector<int>>& M) {
	vector<vector<int>> T(4, vector<int>(4));
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			T[i][j] = M[j][i];
		}
	}
	return T;
}

//�ּҰ� ����
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

//�ִ밪 ���ϱ�
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
//��� ���
vector<vector<int>> filtermatrix(const vector<vector<int>>& M, int n) {
	vector<vector<int>> N(4, vector<int>(4));
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {

		}
	}
}


//�ʱ�ȭ
void init() {


}

//�޴� ���
void menu() {
	cout << "m: ����� ����\n"
		<< "a: ����� ����\n"
		<< "d: ����� ����\n"
		<< "r: ��Ľ��� ��\n"
		<< "t: ��ġ ���\n"
		<< "e: ����� �� �߿��� �ּҰ��� ã�� �� ���� ����� ��� ���� ����\n"
		<< "f: ����� �� �߿��� �ִ밪�� ã�� �� ���� ����� ��� ���� ���Ѵ�\n"
		<< "���� (1~9): �Է��� ������ ����� ���\n"
		<< "s: ����� ���� ���� �����ϰ� ����\n"
		<< "q: ���α׷� ����\n";
}

int main() {
	vector<vector<int>> A = randmatrix();
	vector<vector<int>> B = randmatrix();
	printmatrix(A);
	printmatrix(B);
	menu();
	return 0;
}
