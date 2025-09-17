#include <iostream>
#include<vector>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

using namespace std;

// 사각형 구조체
struct Rect {
	float x1, y1, x2, y2; // 사각형 좌표
	float r, g, b;        // 색상 저장
	bool isSelected = false; // 선택 여부
};

// 사각형 컨테이너
vector<Rect> rects;

//--- 함수선언
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid AddRandomRect(int winW, int winH);
GLvoid Mouse(int button, int state, int x, int y);
GLvoid Motion(int x, int y);
Rect mergeRect(const Rect& r1, const Rect& r2);
vector<Rect> splitRect(const Rect& r);


//--- 변수선언
float Bgcolor[3] = { 1.0f, 1.0f, 1.0f }; //--- 배경색 저장
int selectedRectIndex = -1; // 선택된 사각형 인덱스
int prevMouseX = -1, prevMouseY = -1; // 이전 마우스 위치
bool isOverlap(const Rect& r1, const Rect& r2) {
	return !(r1.x2 < r2.x1 || r1.x1 > r2.x2 || r1.y2 < r2.y1 || r1.y1 > r2.y2);
}


//--- 윈도우 출력하고 콜백함수 설정
void main(int argc, char** argv) 
{
	//--- 윈도우 생성
	glutInit(&argc, argv); //--- glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); //--- 디스플레이 모드 설정
	glutInitWindowPosition(0, 0); //--- 윈도우의 위치 지정
	glutInitWindowSize(800, 600); //--- 윈도우의 크기 지정
	glutCreateWindow("Example1"); //--- 윈도우 생성(윈도우 이름)
	//--- GLEW 초기화
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) //--- glew 초기화 
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized\n";
	glutDisplayFunc(drawScene); //--- 출력 콜백함수의 지정
	glutReshapeFunc(Reshape); //--- 다시 그리기 콜백함수 지정
	glutKeyboardFunc(Keyboard); //--- 키보드 입력 콜백함수 지정
	glutMouseFunc(Mouse);     // 클릭/해제 이벤트
	glutMotionFunc(Motion);   // 드래그 이벤트
	glutMainLoop(); //--- 이벤트 처리 시작
	

}

//--- 사각형 그리기 함수
void DrawRect(const Rect& rect) 
{
	glColor3f(rect.r, rect.g, rect.b);
	glBegin(GL_QUADS);
	glVertex2f(rect.x1, rect.y1);
	glVertex2f(rect.x2, rect.y1);
	glVertex2f(rect.x2, rect.y2);
	glVertex2f(rect.x1, rect.y2);
	glEnd();
}

//--- 화면 출력
GLvoid drawScene() 
{
	//--- 변경된 배경색 설정
	glClearColor(Bgcolor[0],Bgcolor[1],Bgcolor[2], 1.0f);//--- 바탕색을 변경
	glClear(GL_COLOR_BUFFER_BIT); //--- 설정된 색으로 전체를 칠하기
	
	for(const auto& r : rects) {
		DrawRect(r);
	}

	glutSwapBuffers(); //--- 화면에 출력
}

//--- 윈도우 크기 변경시 좌표계 재설정
GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);          // 윈도우 전체를 그리기 영역으로 지정
	glMatrixMode(GL_PROJECTION);     // 투영 행렬 모드로 전환
	glLoadIdentity();                // 이전 설정 초기화
	gluOrtho2D(0, w, 0, h);          // 2D 좌표계 설정 (0~w, 0~h)
	glMatrixMode(GL_MODELVIEW);      // 다시 모델뷰 행렬 모드로 전환
}

//--- 키보드 입력 처리
GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'a':AddRandomRect(800, 600); break;
	case 'q':exit(0); break; //--- 프로그램 종료
	}
	glutPostRedisplay(); //--- 배경색이 바뀔 때마다 출력 콜백 함수를 호출하여 화면을 refresh 한다
}

GLvoid AddRandomRect(int winW, int winH)
{
	Rect newRect;

	float w = rand() % 100 + 20; //--- 사각형의 너비
	float h = rand() % 100 + 20; //--- 사각형의 높이

	//--- 위치
	newRect.x1 = rand() % (winW - (int)w);
	newRect.y1 = rand() % (winH - (int)h);
	newRect.x2 = newRect.x1 + w;
	newRect.y2 = newRect.y1 + h;

	//--- 색상
	newRect.r = (float)(rand() % 100) / 100.0f;
	newRect.g = (float)(rand() % 100) / 100.0f;
	newRect.b = (float)(rand() % 100) / 100.0f;

	newRect.isSelected = false;

	//--- 30개 개수제한
	if(rects.size()<30)
		rects.push_back(newRect);
}

//--- 마우스 클릭
GLvoid Mouse(int button, int state, int x, int y)
{
	int winH = glutGet(GLUT_WINDOW_HEIGHT);
	int mouseY = winH - y; // OpenGL 좌표계에 맞게 변환

	if(button== GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		// 사각형 선택
		selectedRectIndex = -1; // 초기화
		for(int i = 0; i < rects.size(); i++) {
			const Rect& r = rects[i];
			if(x >= r.x1 && x <= r.x2 && mouseY >= r.y1 && mouseY <= r.y2) {
				selectedRectIndex = i;
				rects[i].isSelected = true;
				prevMouseX = x;
				prevMouseY = mouseY;
			} else {
				rects[i].isSelected = false;
			}
		}
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		if (selectedRectIndex != -1) {
			Rect moved = rects[selectedRectIndex];

			// 다른 사각형들과 겹치는지 확인
			for (int i = 0; i < rects.size(); i++) {
				if (i == selectedRectIndex) continue;

				if (isOverlap(moved, rects[i])) {
					// 합치기
					Rect merged = mergeRect(moved, rects[i]);

					// 기존 두 사각형 제거
					if (i > selectedRectIndex) {
						rects.erase(rects.begin() + i);
						rects.erase(rects.begin() + selectedRectIndex);
					}
					else {
						rects.erase(rects.begin() + selectedRectIndex);
						rects.erase(rects.begin() + i);
					}

					// 새 사각형 추가
					rects.push_back(merged);
					break;
				}
			}
		}

		// 드래그 종료 처리
		selectedRectIndex = -1;
		prevMouseX = -1;
		prevMouseY = -1;
		for (auto& r : rects) r.isSelected = false;
	}

	glutPostRedisplay();
}

//--- 마우스 드래그 처리
GLvoid Motion(int x, int y) {
	if(selectedRectIndex != -1) {
		int winH = glutGet(GLUT_WINDOW_HEIGHT);
		int mouseY = winH - y; // OpenGL 좌표계에 맞게 변환
		int dx = x - prevMouseX;
		int dy = mouseY - prevMouseY;
		Rect& r = rects[selectedRectIndex];
		r.x1 += dx;
		r.x2 += dx;
		r.y1 += dy;
		r.y2 += dy;
		prevMouseX = x;
		prevMouseY = mouseY;
		glutPostRedisplay();
	}
}

Rect mergeRect(const Rect& r1, const Rect& r2)
{
	Rect merged;

	merged.x1 = min(r1.x1, r2.x1);
	merged.y1 = min(r1.y1, r2.y1);
	merged.x2 = max(r1.x2, r2.x2);
	merged.y2 = max(r1.y2, r2.y2);

	merged.r = (float)(rand() % 100) / 100.0f;
	merged.g = (float)(rand() % 100) / 100.0f;
	merged.b = (float)(rand() % 100) / 100.0f;
	merged.isSelected = false;

	return merged;
}

vector<Rect> splitRect(const Rect& r)
{
	vector<Rect> result;
	bool splitvertically = rand() % 2;

	if (splitvertically) {
		float midX = (r.x1 + r.x2) / 2.0f;

		Rect r1 = { r.x1, r.y1, midX, r.y2, (float)(rand() % 100) / 100.0f, (float)(rand() % 100) / 100.0f, (float)(rand() % 100) / 100.0f, false };
		Rect r2 = { midX, r.y1, r.x2, r.y2, (float)(rand() % 100) / 100.0f, (float)(rand() % 100) / 100.0f, (float)(rand() % 100) / 100.0f, false };
		result.push_back(r1);
		result.push_back(r2);

	}
	else {
		float midY = (r.y1 + r.y2) / 2.0f;

		Rect r3 = { r.x1, r.y1, r.x2, midY, (float)(rand() % 100) / 100.0f, (float)(rand() % 100) / 100.0f, (float)(rand() % 100) / 100.0f, false };
		Rect r4 = { r.x1, midY, r.x2, r.y2, (float)(rand() % 100) / 100.0f, (float)(rand() % 100) / 100.0f, (float)(rand() % 100) / 100.0f, false };
		result.push_back(r3);
		result.push_back(r4);
	}
	return result;
}
