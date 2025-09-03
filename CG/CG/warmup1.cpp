#include<iostream>
#include<vector>
#include<random>
#include<iomanip>


using namespace std;

random_device random;
mt19937 engine(random());
uniform_int_distribution<int> distribution(1, 9);

//���� ����
int Ecount = 0;
int Fcount = 0;
int Filtercount = 0;
bool filter = false;


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
vector<vector<int>> mulmatrix(const vector<vector<int>>& A, const vector<vector<int>>& B) {
	vector<vector<int>> C(4, vector<int>(4));
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			for (int k = 0; k < 4; k++) {
				C[i][j] += A[i][k] * B[k][j];
			}
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
	//�ּҰ� ã��
	int min = M[0][0];
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (M[i][j] < min)
				min = M[i][j];
		}
	}
	//���
	vector<vector<int>> backup(4, vector<int>(4));
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			backup[i][j] = M[i][j];
		}
	}
	//�ּҰ� ����
	vector<vector<int>> N(4, vector<int>(4));
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			N[i][j] = M[i][j] - min;
		}
	}
	if (Ecount % 2 == 0)
		return backup;
	else
		return N;
}

//�ִ밪 ���ϱ�
vector<vector<int>> maxmatrix(const vector<vector<int>>& M) {
	//�ִ밪 ã��
	int max = M[0][0];
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (M[i][j] > max)
				max = M[i][j];
		}
	}
	//���
	vector<vector<int>> backup(4, vector<int>(4));
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			backup[i][j] = M[i][j];
		}
	}
	//�ִ밪 ���ϱ�
	vector<vector<int>> N(4, vector<int>(4));
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			N[i][j] = M[i][j] + max;
		}
	}
	if (Fcount % 2 == 0)
		return backup;
	else
		return N;
}
//��� ���
vector<vector<int>> filtermatrix(const vector<vector<int>>& M, int n) {
	vector<vector<int>> N(4, vector<int>(4));
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (M[i][j] % n == 0)
				N[i][j] = M[i][j];
			else
				N[i][j] = 0;
		}
	}
	return N;
}

//�޴� ���
char menu() {
	cout << "m: ����� ����\n"
		<< "a: ����� ����\n"
		<< "d: ����� ����\n"
		<< "r: ��Ľ��� ��\n"
		<< "t: ��ġ ���\n"
		<< "e: ����� �� �߿��� �ּҰ��� ã�� �� ���� ����� ��� ���� ����\n"
		<< "f: ����� �� �߿��� �ִ밪�� ã�� �� ���� ����� ��� ���� ���Ѵ�\n"
		<< "���� (1~9): �Է��� ������ ����� ���\n"
		<< "s: ����� ���� ���� �����ϰ� ����\n"
		<< "q: ���α׷� ����\n"
		<< "�޴� �Է�: ";
	char input;
	cin >> input;
	return input;
}

int main() {
	vector<vector<int>> A = randmatrix();
	vector<vector<int>> B = randmatrix();
	printmatrix(A);
	printmatrix(B);

	while (1) {
		char selected = menu();

		if (selected >= '1' && selected <= '9') {
			int n = selected - '0';
			if (!filter) {
				filter = true;
				Filtercount = n;
			}
			else if (filter && Filtercount == n) {
				filter = false;
			}
			else {
				Filtercount = n;
			}

			if (filter) {
				printmatrix(filtermatrix(A, Filtercount));
				printmatrix(filtermatrix(B, Filtercount));
			}
			else {
				printmatrix(A);
				printmatrix(B);
			}
			continue;
		}

		switch (selected) {
		case 'm':
			printmatrix(mulmatrix(A, B));
			break;
		case 'a':
			printmatrix(addmatrix(A, B));
			break;
		case 'd':
			printmatrix(submatrix(A, B));
			break;
		case 'r':
			cout << determinant(A) << "\n";
			cout << "\n";
			cout << determinant(B) << "\n";
			break;
		case 't':
			printmatrix(transmatrix(A));
			cout << "\n";
			printmatrix(transmatrix(B));
			break;
		case 'e':
			Ecount++;
			printmatrix(minmatrix(A));
			cout << "\n";
			printmatrix(minmatrix(B));
			break;
		case 'f':
			Fcount++;
			printmatrix(maxmatrix(A));
			cout << "\n";
			printmatrix(maxmatrix(B));
			break;
		case 's':
			A = randmatrix();
			B = randmatrix();
			printmatrix(A);
			cout << "\n";
			printmatrix(B);
			break;
		case 'q':
			return 0;
			break;
		default:
			cout << "��ȿ���� ���� �Է�\n";
			break;
		}
	}
	return 0;
}
