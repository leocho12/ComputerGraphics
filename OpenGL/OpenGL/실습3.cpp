#include <iostream>
#include<vector>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

using namespace std;

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);

// 사각형 구조체
struct Rect {
	float x1, y1, x2, y2; // 사각형 좌표
	float r, g, b;        // 색상 저장
	bool isSelected = false; // 선택 여부
};

// 사각형 컨테이너
vector<Rect> rects;

float Bgcolor[3] = { 1.0f, 1.0f, 1.0f }; //--- 배경색 저장


void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
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
	glutMainLoop(); //--- 이벤트 처리 시작
}

void DrawRect(const Rect& rect) //--- 사각형 그리기 함수
{
	glColor3f(rect.r, rect.g, rect.b);
	glBegin(GL_QUADS);
	glVertex2f(rect.x1, rect.y1);
	glVertex2f(rect.x2, rect.y1);
	glVertex2f(rect.x2, rect.y2);
	glVertex2f(rect.x1, rect.y2);
	glEnd();
}

GLvoid drawScene() //--- 화면 출력
{
	//--- 변경된 배경색 설정
	glClearColor(Bgcolor[0],Bgcolor[1],Bgcolor[2], 1.0f);//--- 바탕색을 변경
	glClear(GL_COLOR_BUFFER_BIT); //--- 설정된 색으로 전체를 칠하기
	
	for(const auto& r : rects) {
		DrawRect(r);
	}

	glutSwapBuffers(); //--- 화면에 출력
}

GLvoid Reshape(int w, int h)//--- 윈도우 크기 변경시 좌표계 재설정
{
	glViewport(0, 0, w, h);          // 윈도우 전체를 그리기 영역으로 지정
	glMatrixMode(GL_PROJECTION);     // 투영 행렬 모드로 전환
	glLoadIdentity();                // 이전 설정 초기화
	gluOrtho2D(0, w, 0, h);          // 2D 좌표계 설정 (0~w, 0~h)
	glMatrixMode(GL_MODELVIEW);      // 다시 모델뷰 행렬 모드로 전환
}


GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'a':;
	case 'q':exit(0); break; //--- 프로그램 종료
	}
	glutPostRedisplay(); //--- 배경색이 바뀔 때마다 출력 콜백 함수를 호출하여 화면을 refresh 한다
}
