#include<iostream>
#include<vector>
#include<algorithm>

using namespace std;

struct Point {
	int x, y, z;
	bool valid;
};

//���� ����
const int MAX_POINTS = 10;
vector<Point> points(MAX_POINTS);
bool sorted = false;
vector<Point> original;

//����Ʈ ���
void PrintList() {
	cout << "���� ����Ʈ:\n";
	for (int i = 0; i < MAX_POINTS; i++) {
		if (points[i].valid)
			cout << i << ": (" << points[i].x << ", " << points[i].y << " ," << points[i].z << ")\n";
		else
			cout << "����Ʈ �������\n";
		cout << "\n";
	}
	cout << "\n";
}

//����Ʈ �� �� �Է�
void AddTop(int x, int y, int z) {
	for(int i=MAX_POINTS-1;i>0;i--)
		points[i] = points[i - 1];
	points[0] = { x, y, z, true };
	return;
}

//����Ʈ �� �� ����
void RemoveTop() {
	for (int i = 0; i < MAX_POINTS - 1; i++)
		points[i] = points[i + 1];
	points[MAX_POINTS - 1].valid = false;
	return;
}

//����Ʈ �� �Ʒ� �Է�
void AddBottom(int x, int y, int z) {
	for (int i = 0; i < MAX_POINTS; i++) {
		if (!points[i].valid) {
			points[i] = { x, y, z, true };
			return;
		}
	}
	cout << "����Ʈ�� ���� á���ϴ�.\n";
	return;
}

//����Ʈ �� �Ʒ� ����
void RemoveBottom() {
	for (int i = MAX_POINTS - 1; i >= 0; i--) {
		if (points[i].valid) {
			points[i].valid = false;
			return;
		}
	}
	cout << "����Ʈ�� ����ֽ��ϴ�.\n";
	return;
}

//����Ʈ ���� ���
void CountPoints() {
	int count = 0;
	for (const auto& point : points) {
		if (point.valid) count++;
	}
	cout << "����Ʈ�� ����� ���� ����: " << count << "\n";
	return;
}

//�� ��ġ ����
void DecreasePositions() {
	Point temp = points[0];
	for (int i = 0; i < MAX_POINTS - 1; i++) {
		points[i] = points[i + 1];
	}
	points[MAX_POINTS - 1] = temp;
}

//����Ʈ ����
void ClearList() {
	for (auto& point : points) {
		point.valid = false;
	}
	return;
}

//�Ÿ� ���
double DistanceFromOrigin(const Point& p) {
	return sqrt(p.x * p.x + p.y * p.y + p.z * p.z);
}

//�Ÿ� ���� ����
void SortByDistance() {
	if (!sorted) {
		original = points;
		vector<Point> validPoints;
		for (const auto& point : points) {
			if (point.valid) validPoints.push_back(point);
		}
		sort(validPoints.begin(), validPoints.end(), [](const Point& a, const Point& b) {
			return DistanceFromOrigin(a) < DistanceFromOrigin(b);
			});
		for (int i = 0; i < MAX_POINTS; i++) {
			if (i < validPoints.size())
				points[i] = validPoints[i];
			else
				points[i].valid = false;
		}
		sorted = true;
	}
	else {
		points = original;
		sorted = false;
	}
	return;
}

// �޴� ���
char menu() {
	cout << "+ x y z: ����Ʈ�� �� ���� �Է� (x, y, z: ����)\n"
		<< "-: ����Ʈ�� �� ������ ����\n"
		<< "e x y z: ����Ʈ�� �� �Ʒ��� �Է� (��ɾ� +�� �ݴ��� ��ġ, ����Ʈ�� ����� �����Ͱ��� ���� �ö󰣴�.)\n"
		<< "d: ����Ʈ�� �� �Ʒ����� �����Ѵ�. (����Ʈ���� ������ ĭ�� ����ִ�.)\n"
		<< "a: ����Ʈ�� ����� ���� ������ ���\n"
		<< "b: ������ ��ġ�� �� ĭ�� ����\n"
		<< "c: ����Ʈ�� ����. ����Ʈ�� ��� �� �ٽ� �Է��ϸ� 0������ ����\n"
		<< "f: �������� �Ÿ��� �����Ͽ� ������������ �����Ͽ� ����Ѵ�. �ε��� 0������ �� ĭ���� �����Ͽ� ����Ѵ�. �ٽ� ������ ������� ���\n"
		<< "q: ����\n"
		<< "�޴� �Է�: ";
	char input;
	cin >> input;
	return input;
}

int main() {
	while (true) {
		PrintList();
		char choice = menu();
		if (choice == 'q') break;
		switch (choice) {
		case '+': {
			int x, y, z;
			cout << "x y z �Է�: ";
			cin >> x >> y >> z;
			AddTop(x, y, z);
			break;
		}
		case '-':
			RemoveTop();
			break;
		case 'e': {
			int x, y, z;
			cout << "x y z �Է�: ";
			cin >> x >> y >> z;
			AddBottom(x, y, z);
			break;
		}
		case 'd':
			RemoveBottom();
			break;
		case 'a':
			CountPoints();
			break;
		case 'b':
			DecreasePositions();
			break;
		case 'c':
			ClearList();
			break;
		case 'f':
			SortByDistance();
			break;
		default:
			cout << "��ȿ���� ���� �Է�\n";
			break;
		}
	}
	return 0;
}

