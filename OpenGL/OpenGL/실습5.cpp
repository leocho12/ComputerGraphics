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

//함수선언
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid InitRects(int winW, int winH);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Mouse(int button, int state, int x, int y);
GLvoid Motion(int x, int y);



//변수선언
float Bgcolor[3] = { 1.0f, 1.0f, 1.0f }; //--- 배경색 저장
int selectedRectIndex = -1; // 선택된 사각형 인덱스
int prevMouseX = -1, prevMouseY = -1; // 이전 마우스 위치
const float RECTSIZE = 30.0f;
bool isOverlap(const Rect& r1, const Rect& r2) {
    return !(r1.x2 < r2.x1 || r1.x1 > r2.x2 || r1.y2 < r2.y1 || r1.y1 > r2.y2);
}
Rect eraser;
bool hasEraser = false;
float eraserSize = RECTSIZE * 2;
int erasedCount = 0;


//윈도우 출력하고 콜백함수 설정
void main(int argc, char** argv)
{
    //윈도우 생성
    glutInit(&argc, argv); //glut 초기화
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); //디스플레이 모드 설정
    glutInitWindowPosition(0, 0); //윈도우의 위치 지정
    glutInitWindowSize(800, 600); //윈도우의 크기 지정
    glutCreateWindow("Example1"); //윈도우 생성(윈도우 이름)
    //GLEW 초기화
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) //glew 초기화 
    {
        std::cerr << "Unable to initialize GLEW" << std::endl;
        exit(EXIT_FAILURE);
    }
    else
        std::cout << "GLEW Initialized\n";

	InitRects(800, 600);

    glutDisplayFunc(drawScene); //출력 콜백함수의 지정
    glutReshapeFunc(Reshape); //다시 그리기 콜백함수 지정
    glutKeyboardFunc(Keyboard); //키보드 입력 콜백함수 지정
    glutMouseFunc(Mouse);     //클릭/해제 이벤트
    glutMotionFunc(Motion);   //드래그 이벤트
    glutMainLoop(); //이벤트 처리 시작


}

//사각형 그리기 함수
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

//화면 출력
GLvoid drawScene()
{
    // 변경된 배경색 설정
    glClearColor(Bgcolor[0], Bgcolor[1], Bgcolor[2], 1.0f);//바탕색 변경
    glClear(GL_COLOR_BUFFER_BIT); //설정된 색으로 전체를 칠하기

    for (const auto& r : rects) {
        DrawRect(r);
    }

    if(hasEraser)
		DrawRect(eraser);

    glutSwapBuffers(); //화면에 출력
}

//윈도우 크기 변경시 좌표계 재설정
GLvoid Reshape(int w, int h)
{
    glViewport(0, 0, w, h);          // 윈도우 전체를 그리기 영역으로 지정
    glMatrixMode(GL_PROJECTION);     // 투영 행렬 모드로 전환
    glLoadIdentity();                // 이전 설정 초기화
    gluOrtho2D(0, w, 0, h);          // 2D 좌표계 설정 (0~w, 0~h)
    glMatrixMode(GL_MODELVIEW);      // 다시 모델뷰 행렬 모드로 전환
}

//키보드 입력 처리
GLvoid Keyboard(unsigned char key, int x, int y)
{
    switch (key) {
    case 'r':InitRects(800,600); break;
    case 'q':exit(0); break; //프로그램 종료
    }
    glutPostRedisplay(); //배경색이 바뀔 때마다 출력 콜백 함수를 호출하여 화면을 refresh 한다
}


//마우스 클릭
GLvoid Mouse(int button, int state, int x, int y)
{
    int winH = glutGet(GLUT_WINDOW_HEIGHT);
    int mouseY = winH - y; //OpenGL 좌표계로 변환

    //좌클릭-지우개 생성
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if(!hasEraser){
			eraserSize = RECTSIZE * 2;
			eraser.x1 = x - eraserSize / 2;
			eraser.y1 = mouseY - eraserSize / 2;
			eraser.x2 = x + eraserSize / 2;
			eraser.y2 = mouseY + eraserSize / 2;
            eraser.r = 0.0f; 
            eraser.g = 0.0f; 
            eraser.b = 0.0f;
            eraser.isSelected = true;
            hasEraser = true;

			prevMouseX = x;
			prevMouseY = mouseY;

        }
    }
    else if(button == GLUT_LEFT_BUTTON && state == GLUT_UP){
        hasEraser = false;
	}
	//우클릭-사각형 생성
    else if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		if(erasedCount>0)
        {
            Rect newRect;
            newRect.x1 = x - RECTSIZE / 2;
            newRect.y1 = mouseY - RECTSIZE / 2;
            newRect.x2 = x + RECTSIZE/2;
            newRect.y2 = mouseY + RECTSIZE/2;
            newRect.r = (float)(rand() % 100) / 100.0f;
            newRect.g = (float)(rand() % 100) / 100.0f;
            newRect.b = (float)(rand() % 100) / 100.0f;
            newRect.isSelected = false;

            rects.push_back(newRect);
			erasedCount--;

            if (eraserSize > RECTSIZE * 2) {
                eraserSize -= RECTSIZE * 0.5f;

                // 지우개 중심 유지하며 크기 재설정
                float cx = (eraser.x1 + eraser.x2) / 2.0f;
                float cy = (eraser.y1 + eraser.y2) / 2.0f;
                eraser.x1 = cx - eraserSize / 2;
                eraser.y1 = cy - eraserSize / 2;
                eraser.x2 = cx + eraserSize / 2;
                eraser.y2 = cy + eraserSize / 2;
            }
        }
	}
    glutPostRedisplay();
}

GLvoid InitRects(int winW, int winH)
{
	rects.clear();//clear window
    int count = rand() % 21 + 20;//20~40

    for (int i = 0; i < count; i++)
    {
		Rect newRect;

        newRect.x1 = rand() % (winW - (int)RECTSIZE);
        newRect.y1 = rand() % (winH - (int)RECTSIZE);
        newRect.x2 = newRect.x1 + RECTSIZE;
        newRect.y2 = newRect.y1 + RECTSIZE;
        newRect.r = (float)(rand() % 100) / 100.0f;
        newRect.g = (float)(rand() % 100) / 100.0f;
        newRect.b = (float)(rand() % 100) / 100.0f;
        newRect.isSelected = false;
        
        rects.push_back(newRect);
    }
    return GLvoid();
}

//마우스 드래그 처리
GLvoid Motion(int x, int y) {
    if (hasEraser) {
        int winH = glutGet(GLUT_WINDOW_HEIGHT);
        int mouseY = winH - y; // OpenGL 좌표계에 맞게 변환
        int dx = x - prevMouseX;
        int dy = mouseY - prevMouseY;

        //지우개 이동
		eraser.x1 += dx;
		eraser.x2 += dx;
		eraser.y1 += dy;
		eraser.y2 += dy;

        //지우기
        for (int i = 0; i < rects.size();) {
            if (isOverlap(eraser, rects[i])) {
                //지운 개수 증가
                erasedCount++;

                //지우개 크기 증가 
                eraserSize += RECTSIZE * 0.5f;
                float cx = (eraser.x1 + eraser.x2) / 2.0f;
                float cy = (eraser.y1 + eraser.y2) / 2.0f;
                eraser.x1 = cx - eraserSize / 2;
                eraser.y1 = cy - eraserSize / 2;
                eraser.x2 = cx + eraserSize / 2;
                eraser.y2 = cy + eraserSize / 2;

                eraser.r = rects[i].r;
                eraser.g = rects[i].g;
                eraser.b = rects[i].b;

                rects.erase(rects.begin() + i);
            }
            else {
                i++;
            }
        }
        
        prevMouseX = x;
        prevMouseY = mouseY;

        glutPostRedisplay();
    }
}