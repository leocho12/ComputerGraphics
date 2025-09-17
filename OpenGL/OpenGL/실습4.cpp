#include <iostream>
#include<vector>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

using namespace std;

// 사각형 구조체
struct Rect {
    float x1, y1, x2, y2; // 사각형 좌표
    float r, g, b;        // 색상
	//애니메이션 관련
	float vx = 0.0f, vy = 0.0f; // 속도 변화
	float dw = 0.0f, dh = 0.0f; // 크기 변화
	float dr = 0.f, dg = 0.f, db = 0.f; // 색상 변화
    bool aniPos = false;
    bool aniSize = false;
	bool aniColor = false;
};

// 사각형 컨테이너
vector<Rect> rects;

//--- 함수선언
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid AddRandomRect(int winW, int winH);
GLvoid Mouse(int button, int state, int x, int y);
void OnTimer(int);


//--- 변수선언
float Bgcolor[3] = { 1.0f, 1.0f, 1.0f }; //--- 배경색 저장



//--- 윈도우 출력하고 콜백함수 설정
void main(int argc, char** argv)
{
    //--- 윈도우 생성
    glutInit(&argc, argv); //--- glut 초기화
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); //--- 디스플레이 모드 설정
    glutInitWindowPosition(0, 0); //--- 윈도우의 위치 지정
    glutInitWindowSize(800, 600); //--- 윈도우의 크기 지정
    glutCreateWindow("Example4"); //--- 윈도우 생성(윈도우 이름)
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

	glutTimerFunc(10, OnTimer, 0); //타이머 이벤트

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

//--- 타이머 함수
void OnTimer(int) {
	const float dt = 0.1f / 60.0f; // 프레임 간격
	int winw = glutGet(GLUT_WINDOW_WIDTH);
	int winh = glutGet(GLUT_WINDOW_HEIGHT);

    for (auto& t : rects) {
        //위치
        if (t.aniPos) {
            t.x1 += t.vx * dt;
            t.y1 += t.vy * dt;
            t.x2 += t.vx * dt;
            t.y2 += t.vy * dt;
            //벽 반사
            if (t.x1 < 0 || t.x2 > winw) t.vx = -t.vx;
			if (t.y1 < 0 || t.y2 > winh) t.vy = -t.vy;
        }
		//크기
        if (t.aniSize) {
			float w = (t.x2 - t.x1) + t.dw * dt;
			float h = (t.y2 - t.y1) + t.dh * dt;
			float cx = (t.x1 + t.x2) * 0.5f;//중심점 계산
			float cy = (t.y1 + t.y2) * 0.5f;

            if (w < 20 || w > 200) { t.dw = -t.dw; w = max(20.0f, min(200.f, w)); }
			if (h < 20 || h > 200) { t.dh = -t.dh; h = max(20.0f, min(200.f, h)); }

			t.x1 = cx - w * 0.5f;
			t.x2 = cx + w * 0.5f;
			t.y1 = cy - h * 0.5f;
			t.y2 = cy + h * 0.5f;

        }
		//색상
        if (t.aniColor) {
            t.r += t.dr * dt;
            t.g += t.dg * dt;
            t.b += t.db * dt;

            auto bounce = [](float& c, float& dc) {
                if (c < 0 || c > 1) { dc = -dc; c = max(0.0f, min(1.0f, c)); }
                };
            bounce(t.r, t.dr);
            bounce(t.g, t.dg);
            bounce(t.b, t.db);
        }
    }
	glutPostRedisplay();
	glutTimerFunc(10, OnTimer, 0);
}

//--- 화면 출력
GLvoid drawScene()
{
    //--- 변경된 배경색 설정
    glClearColor(Bgcolor[0], Bgcolor[1], Bgcolor[2], 1.0f);//--- 바탕색을 변경
    glClear(GL_COLOR_BUFFER_BIT); //--- 설정된 색으로 전체를 칠하기

    for (const auto& r : rects) {
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
    case '1':for (auto& t : rects) t.aniPos = !t.aniPos; break;
    case '2':for (auto& t : rects) t.aniPos = !t.aniPos; break;
    case '3':for(auto& t:rects)t.aniSize=!t.aniSize; break;
    case '4':for (auto& t : rects) t.aniColor = !t.aniColor; break;
    case '5':; break;
    case 's':for (auto& t : rects) { t.aniPos = t.aniSize = t.aniColor = false; }; break;
    case 'm':; break;
    case 'r':rects.clear(); break;
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

    //--- 5개 개수제한
    if (rects.size() < 5)
        rects.push_back(newRect);
}

//--- 마우스 클릭
GLvoid Mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        int winH = glutGet(GLUT_WINDOW_HEIGHT);
        float cx = (float)x;
        float cy = (float)(winH - y); //OpenGL 좌표계로 변환

        if (rects.size() < 5) {
            Rect newRect{};
            float w = 50 + rand() % 70; //사각형 너비
            float h = 50 + rand() % 70; //사각형 높이
            //사각형 색상
            newRect.r = (rand() % 100) / 100.0f;
            newRect.g = (rand() % 100) / 100.0f;
            newRect.b = (rand() % 100) / 100.0f;
            //사각형 위치
            newRect.x1 = cx - w * 0.5f;
            newRect.y1 = cy - h * 0.5f;
            newRect.x2 = newRect.x1 + w;
            newRect.y2 = newRect.y1 + h;
            //속도/변화량 초기化
            newRect.vx = (rand() % 200) - 100.0f; //-100 ~ +100
            newRect.vy = (rand() % 200) - 100.0f; //-100 ~ +100
            newRect.dw = (rand() % 100) - 50.0f;  //-50 ~ +50
            newRect.dh = (rand() % 100) - 50.0f;  //-50 ~ +50
            newRect.dr = (rand() % 200) / 100.0f - 1.0f; //-1 ~ +1
            newRect.dg = (rand() % 200) / 100.0f - 1.0f; //-1 ~ +1
            newRect.db = (rand() % 200) / 100.0f - 1.0f; //-1 ~ +1

            rects.push_back(newRect);
        }
    }
}

