#include <iostream>
#include <gl/glew.h> //--- 필요한 헤더파일 include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Timer(int value);

float Bgcolor[3] = { 1.0f, 1.0f, 1.0f }; //--- 배경색 저장
bool TimerOn = false;

void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv); //--- glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); //--- 디스플레이 모드 설정
	glutInitWindowPosition(0, 0); //--- 윈도우의 위치 지정
	glutInitWindowSize(800, 600); //--- 윈도우의 크기 지정
	glutCreateWindow("Example1"); //--- 윈도우 생성(윈도우 이름)
	//--- GLEW 초기화하기
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
GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수
{
	//--- 변경된 배경색 설정
	glClearColor(Bgcolor[0], Bgcolor[1], Bgcolor[2], 1.0f);//--- 바탕색을 변경
	glClear(GL_COLOR_BUFFER_BIT); //--- 설정된 색으로 전체를 칠하기
	glutSwapBuffers(); //--- 화면에 출력하기
}
GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수
{
	glViewport(0, 0, w, h);
}
GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'c': Bgcolor[0] = 0.0f; Bgcolor[1] = 1.0f; Bgcolor[2] = 1.0f; break; //--- 배경색을 청록색으로 설정
	case 'm': Bgcolor[0] = 1.0f; Bgcolor[1] = 0.0f; Bgcolor[2] = 1.0f; break; //--- 배경색을 자홍색으로 설정
	case 'y': Bgcolor[0] = 1.0f; Bgcolor[1] = 1.0f; Bgcolor[2] = 0.0f; break; //--- 배경색을 황색으로 설정
	case 'a':Bgcolor[0] = (float)rand() / RAND_MAX; Bgcolor[1] = (float)rand() / RAND_MAX; Bgcolor[2] = (float)rand() / RAND_MAX; break; //--- 배경색을 랜덤색으로 설정
	case 'w':Bgcolor[0] = 1.0f; Bgcolor[1] = 1.0f; Bgcolor[2] = 1.0f; break; //--- 배경색을 백색으로 설정
	case 'k':Bgcolor[0] = 0.0f; Bgcolor[1] = 0.0f; Bgcolor[2] = 0.0f; break; //--- 배경색을 흑색으로 설정
	case 't':TimerOn = true; glutTimerFunc(500, Timer, 1); break; //--- 타이머 설정해 특정시간마다	 배경색을 랜덤으로 설정
	case 's':TimerOn = false; break; //--- 타이머 종료
	case 'q':exit(0); break; //--- 프로그램 종료
	}
	glutPostRedisplay(); //--- 배경색이 바뀔 때마다 출력 콜백 함수를 호출하여 화면을 refresh 한다
}
GLvoid Timer(int value)
{
	if (TimerOn) {
		// 랜덤색 갱신
		Bgcolor[0] = (float)rand() / RAND_MAX;
		Bgcolor[1] = (float)rand() / RAND_MAX;
		Bgcolor[2] = (float)rand() / RAND_MAX;

		glutPostRedisplay();
		glutTimerFunc(500, Timer, 1); // 다시 타이머 등록
	}
}