#include <iostream>
#include <vector>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

using namespace std;

//사각형 구조체
struct Rect {
    float cx,cy; //사각형 중심 좌표
    float r, g, b;        //색상 저장
	float vx = 0, vy = 0; //속도
    float size;           //크기
	bool AnimationActive = false;//애니메이션 활성화 여부
};

//Global 변수
float Bgcolor[3] = { 1.0f, 1.0f, 1.0f }; //배경색 저장
int mode = 1;
//사각형 컨테이너
vector<Rect> rects;

//함수선언
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid InitRects(int winW, int winH);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Mouse(int button, int state, int x, int y);



//윈도우 출력하고 콜백함수 설정
void main(int argc, char** argv)
{
    //윈도우 생성
    glutInit(&argc, argv); //glut 초기화
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); //디스플레이 모드 설정
    glutInitWindowPosition(0, 0); //윈도우의 위치 지정
    glutInitWindowSize(800, 600); //윈도우의 크기 지정
    glutCreateWindow("Example6"); //윈도우 생성(윈도우 이름)
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
    glutMouseFunc(Mouse); //클릭/해제 이벤트
	glutTimerFunc(16, Timer, 0); //타이머 이벤트
    glutMainLoop(); //이벤트 처리 시작


}

//사각형 그리기 함수
void DrawRect(const Rect& rect)
{
    glColor3f(rect.r, rect.g, rect.b);
	float half = rect.size / 2.0f;
    glBegin(GL_QUADS);
    glVertex2f(rect.cx - half, rect.cy - half);
    glVertex2f(rect.cx + half, rect.cy - half);
    glVertex2f(rect.cx + half, rect.cy + half);
    glVertex2f(rect.cx - half, rect.cy + half);
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
    case'1':mode = 1; break;//좌우상하
    case'2':mode = 1; break;//대각선
    case'3':mode = 1; break;//그룹
    case'4':mode = 1; break;//8방향
    case 'r':InitRects(800, 600); break;//종료
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
        for (int i = 0; i < rects.size(); i++) {
			Rect& r = rects[i];
			float half = r.size / 2.0f;
            if (mouseY >= r.cy - half && mouseY <= r.cy + half && x >= r.cx - half && x <= r.cx + half) {
                if (mode == 1) {

                }
                else if(mode == 2) {
                }
                else if (mode == 3) {
                }
                else if (mode == 4) {
				}
                break;
            }
        }
        
    }
    
    glutPostRedisplay();
}

GLvoid Timer(int value) {
    for (int i = 0; i < rects.size(); i++) {
        Rect& r = rects[i];
        if (r.AnimationActive) {
            r.cx += r.vx;
            r.cy += r.vy;
            r.size -= 0.2f;//축소
            if (r.size < 5) {
                rects.erase(rects.begin() + i);
                continue;
            }
        }
        i++;
    }
    glutPostRedisplay();
    glutTimerFunc(16, Timer, 0);
}

GLvoid InitRects(int winW, int winH)
{
    rects.clear();//clear window
    int count = rand() % 6 + 5;//5~10

    for (int i = 0; i < count; i++)
    {
        Rect newRect;

        newRect.cx= rand() % winW;
        newRect.cy = rand() % winH;
		newRect.size = (float)(rand() % 30 + 20); //20~50
        newRect.r = (float)(rand() % 100) / 100.0f;
        newRect.g = (float)(rand() % 100) / 100.0f;
        newRect.b = (float)(rand() % 100) / 100.0f;
        newRect.AnimationActive = false;
        newRect.vx = 0;
        newRect.vy = 0;

        rects.push_back(newRect);
    }
    return GLvoid();
}
