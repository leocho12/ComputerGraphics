#include<iostream>
#include<vector>
#include<algorithm>

using namespace std;

struct Point {
	int x, y, z;
	bool valid;
};

//변수 설정
const int MAX_POINTS = 10;
vector<Point> points(MAX_POINTS);
bool sorted = false;
vector<Point> original;

//리스트 출력
void PrintList() {
	cout << "현재 리스트:\n";
	for (int i = 0; i < MAX_POINTS; i++) {
		if (points[i].valid)
			cout << i << ": (" << points[i].x << ", " << points[i].y << " ," << points[i].z << ")\n";
		else
			cout << "리스트 비어있음\n";
		cout << "\n";
	}
	cout << "\n";
}

//리스트 맨 위 입력
void AddTop(int x, int y, int z) {
	for(int i=MAX_POINTS-1;i>0;i--)
		points[i] = points[i - 1];
	points[0] = { x, y, z, true };
	return;
}

//리스트 맨 위 삭제
void RemoveTop() {
	for (int i = 0; i < MAX_POINTS - 1; i++)
		points[i] = points[i + 1];
	points[MAX_POINTS - 1].valid = false;
	return;
}

//리스트 맨 아래 입력
void AddBottom(int x, int y, int z) {
	for (int i = 0; i < MAX_POINTS; i++) {
		if (!points[i].valid) {
			points[i] = { x, y, z, true };
			return;
		}
	}
	cout << "리스트가 가득 찼습니다.\n";
	return;
}

//리스트 맨 아래 삭제
void RemoveBottom() {
	for (int i = MAX_POINTS - 1; i >= 0; i--) {
		if (points[i].valid) {
			points[i].valid = false;
			return;
		}
	}
	cout << "리스트가 비어있습니다.\n";
	return;
}

//리스트 개수 출력
void CountPoints() {
	int count = 0;
	for (const auto& point : points) {
		if (point.valid) count++;
	}
	cout << "리스트에 저장된 점의 개수: " << count << "\n";
	return;
}

//점 위치 감소
void DecreasePositions() {
	Point temp = points[0];
	for (int i = 0; i < MAX_POINTS - 1; i++) {
		points[i] = points[i + 1];
	}
	points[MAX_POINTS - 1] = temp;
}

//리스트 비우기
void ClearList() {
	for (auto& point : points) {
		point.valid = false;
	}
	return;
}

//거리 계산
double DistanceFromOrigin(const Point& p) {
	return sqrt(p.x * p.x + p.y * p.y + p.z * p.z);
}

//거리 기준 정렬
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

// 메뉴 출력
char menu() {
	cout << "+ x y z: 리스트의 맨 위에 입력 (x, y, z: 숫자)\n"
		<< "-: 리스트의 맨 위에서 삭제\n"
		<< "e x y z: 리스트의 맨 아래에 입력 (명령어 +와 반대의 위치, 리스트에 저장된 데이터값이 위로 올라간다.)\n"
		<< "d: 리스트의 맨 아래에서 삭제한다. (리스트에서 삭제된 칸이 비어있다.)\n"
		<< "a: 리스트에 저장된 점의 개수를 출력\n"
		<< "b: 점들의 위치를 한 칸씩 감소\n"
		<< "c: 리스트를 비운다. 리스트를 비운 후 다시 입력하면 0번부터 저장\n"
		<< "f: 원점과의 거리를 정렬하여 오름차순으로 정렬하여 출력한다. 인덱스 0번부터 빈 칸없이 저장하여 출력한다. 다시 누르면 원래대로 출력\n"
		<< "q: 종료\n"
		<< "메뉴 입력: ";
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
			cout << "x y z 입력: ";
			cin >> x >> y >> z;
			AddTop(x, y, z);
			break;
		}
		case '-':
			RemoveTop();
			break;
		case 'e': {
			int x, y, z;
			cout << "x y z 입력: ";
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
			cout << "유효하지 않은 입력\n";
			break;
		}
	}
	return 0;
}

