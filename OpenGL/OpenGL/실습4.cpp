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
    bool aniPos1 = false;
	bool aniPos2 = false;
    int zigdir = 0;
    float zigtime = 0;
    bool aniSize = false;
	bool aniColor = false;
    bool goingUp = true;
	float initX = 0.0f, initY = 0.0f;
    bool followLeader = false;   // 따라가기 여부
    int followTarget = -1;       // 누구를 따라하는지 (인덱스)
    float followDelay = 0.0f;    // 지연 시간
    float followTimer = 0.0f;    // 누적 시간

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

//--- 타이머 함수 (전체)
void OnTimer(int) {
    const float dt = 0.1f / 60.0f; // 프레임 간격
    int winw = glutGet(GLUT_WINDOW_WIDTH);
    int winh = glutGet(GLUT_WINDOW_HEIGHT);

    for (int i = 0; i < rects.size(); i++) {
        Rect& t = rects[i];

        // =====================
        // 따라하기 처리
        // =====================
        if (t.followLeader) {
            Rect& leader = rects[t.followTarget];
            t.followTimer += dt;

            if (t.followTimer >= t.followDelay) {
                // 리더의 상태를 그대로 복사
                t.x1 = leader.x1;
                t.y1 = leader.y1;
                t.x2 = leader.x2;
                t.y2 = leader.y2;
                t.r = leader.r;
                t.g = leader.g;
                t.b = leader.b;
            }
            continue; // 따라가는 사각형은 독립 애니메이션 건너뜀
        }

        // =====================
        // 원래 애니메이션 처리
        // =====================

        // 위치 변화 1 : 대각선 이동 + 반사
        if (t.aniPos1) {
            t.x1 += t.vx * dt;
            t.y1 += t.vy * dt;
            t.x2 += t.vx * dt;
            t.y2 += t.vy * dt;
            if (t.x1 < 0 || t.x2 > winw) t.vx = -t.vx;
            if (t.y1 < 0 || t.y2 > winh) t.vy = -t.vy;
        }

        // 위치 변화 2 : ㄹ자 이동
        if (t.aniPos2) {
            float speed = 500;
            float move = speed * dt;

            // 현재 진행 방향에 따라 이동
            if (t.zigdir == 0) { // →
                t.x1 += move; t.x2 += move;
            }
            else if (t.zigdir == 1) { // ↑
                t.y1 += move; t.y2 += move;
            }
            else if (t.zigdir == 2) { // ←
                t.x1 -= move; t.x2 -= move;
            }
            else if (t.zigdir == 3) { // ↓
                t.y1 -= move; t.y2 -= move;
            }

            t.zigtime += dt;
            if (t.zigtime > 0.5f) {
                // 일정 시간마다 방향 전환
                if (t.goingUp) {
                    // 올라가는 ㄹ자 : → ↑ ← ↑ 반복
                    if (t.zigdir == 0) t.zigdir = 1;
                    else if (t.zigdir == 1) t.zigdir = 2;
                    else if (t.zigdir == 2) t.zigdir = 1;
                }
                else {
                    // 내려가는 ㄹ자 : → ↓ ← ↓ 반복
                    if (t.zigdir == 0) t.zigdir = 3;
                    else if (t.zigdir == 3) t.zigdir = 2;
                    else if (t.zigdir == 2) t.zigdir = 3;
                }
                t.zigtime = 0;
            }

            float w = t.x2 - t.x1;
            float h = t.y2 - t.y1;

            // --- 천장/바닥 처리 ---
            if (t.y2 >= winh) {
                // 천장에 닿으면 내려가기 시작
                t.y2 = winh; t.y1 = winh - h;
                t.goingUp = false;
                t.zigdir = 3;   // ↓로 전환
                t.zigtime = 0;
            }
            if (t.y1 <= 0) {
                // 바닥에 닿으면 올라가기 시작
                t.y1 = 0; t.y2 = h;
                t.goingUp = true;
                t.zigdir = 1;   // ↑로 전환
                t.zigtime = 0;
            }

            // 좌우 벽 처리 (튀어나가지 않도록 보정)
            if (t.x1 < 0) {
                t.x1 = 0; t.x2 = w;
            }
            if (t.x2 > winw) {
                t.x2 = winw; t.x1 = winw - w;
            }
        }



        // 크기 변화
        if (t.aniSize) {
            float w = (t.x2 - t.x1) + t.dw * dt;
            float h = (t.y2 - t.y1) + t.dh * dt;
            float cx = (t.x1 + t.x2) * 0.5f;
            float cy = (t.y1 + t.y2) * 0.5f;

            if (w < 20 || w > 200) { t.dw = -t.dw; w = max(20.0f, min(200.f, w)); }
            if (h < 20 || h > 200) { t.dh = -t.dh; h = max(20.0f, min(200.f, h)); }

            t.x1 = cx - w * 0.5f;
            t.x2 = cx + w * 0.5f;
            t.y1 = cy - h * 0.5f;
            t.y2 = cy + h * 0.5f;
        }

        // 색상 변화
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
    case '1':for (auto& t : rects) { t.aniPos1 = !t.aniPos1; t.aniPos2 = false; }break;
    case '2':for (auto& t : rects) { t.aniPos2 = !t.aniPos2; t.aniPos1 = false; t.zigdir = 0; t.zigtime = 0; }break;
    case '3':for(auto& t:rects)t.aniSize=!t.aniSize; break;
    case '4':for (auto& t : rects) t.aniColor = !t.aniColor; break;
    case '5': {
        if (rects.empty()) break;
        int leaderIndex = rand() % rects.size();
        Rect& leader = rects[leaderIndex];
        cout << "Leader = " << leaderIndex << endl;

        float delayStep = 0.5f; // 뒤따르는 사각형 간 딜레이

        for (int i = 0; i < rects.size(); i++) {
            if (i == leaderIndex) continue;
            rects[i].aniPos1 = leader.aniPos1;
            rects[i].aniPos2 = leader.aniPos2;
            rects[i].aniSize = leader.aniSize;
            rects[i].aniColor = leader.aniColor;

            // 지그재그 같은 건 시작 위치 리셋
            rects[i].zigdir = leader.zigdir;
            rects[i].zigtime = leader.zigtime;
        }
        break;
    }

    case 's':for (auto& t : rects) { t.aniPos2 = t.aniPos1 = t.aniSize = t.aniColor = false; }; break;
    case 'm':for (auto& t : rects) {
        float w = t.x2 - t.x1;
        float h = t.y2 - t.y1;
        // 중심을 원래 위치로 되돌림
        t.x1 = t.initX - w * 0.5f;
        t.x2 = t.initX + w * 0.5f;
        t.y1 = t.initY - h * 0.5f;
        t.y2 = t.initY + h * 0.5f;
    }; break;
    case 'r':rects.clear(); break;
    case 'q':exit(0); break; //--- 프로그램 종료
    }
    glutPostRedisplay(); //--- 배경색이 바뀔 때마다 출력 콜백 함수를 호출하여 화면을 refresh 한다
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
            // 중심 좌표 저장
            newRect.initX = cx;
            newRect.initY = cy;
            //속도/변화량 초기값
            newRect.vx = (rand() % 2 ? 1.f : -1.f) * (300 + rand() % 300); //-100 ~ +100
            newRect.vy = (rand() % 2 ? 1.f : -1.f) * (300 + rand() % 300); //-100 ~ +100
            newRect.dw = (rand() % 2 ? 1.f : -1.f) * (200 + rand() % 200);  //-50 ~ +50
            newRect.dh = (rand() % 2 ? 1.f : -1.f) * (200 + rand() % 200);  //-50 ~ +50
            newRect.dr = (rand() % 200) / 100.0f - 1.0f; //-1 ~ +1
            newRect.dg = (rand() % 200) / 100.0f - 1.0f; //-1 ~ +1
            newRect.db = (rand() % 200) / 100.0f - 1.0f; //-1 ~ +1

            rects.push_back(newRect);
        }
    }
}

