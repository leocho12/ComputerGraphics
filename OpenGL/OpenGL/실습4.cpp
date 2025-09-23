#include <iostream>
#include<vector>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

using namespace std;

// �簢�� ����ü
struct Rect {
    float x1, y1, x2, y2; // �簢�� ��ǥ
    float r, g, b;        // ����
	//�ִϸ��̼� ����
	float vx = 0.0f, vy = 0.0f; // �ӵ� ��ȭ
	float dw = 0.0f, dh = 0.0f; // ũ�� ��ȭ
	float dr = 0.f, dg = 0.f, db = 0.f; // ���� ��ȭ
    bool aniPos1 = false;
	bool aniPos2 = false;
    int zigdir = 0;
    float zigtime = 0;
    bool aniSize = false;
	bool aniColor = false;
    bool goingUp = true;
	float initX = 0.0f, initY = 0.0f;
    bool followLeader = false;   // ���󰡱� ����
    int followTarget = -1;       // ������ �����ϴ��� (�ε���)
    float followDelay = 0.0f;    // ���� �ð�
    float followTimer = 0.0f;    // ���� �ð�

};

// �簢�� �����̳�
vector<Rect> rects;

//--- �Լ�����
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid AddRandomRect(int winW, int winH);
GLvoid Mouse(int button, int state, int x, int y);
void OnTimer(int);


//--- ��������
float Bgcolor[3] = { 1.0f, 1.0f, 1.0f }; //--- ���� ����



//--- ������ ����ϰ� �ݹ��Լ� ����
void main(int argc, char** argv)
{
    //--- ������ ����
    glutInit(&argc, argv); //--- glut �ʱ�ȭ
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); //--- ���÷��� ��� ����
    glutInitWindowPosition(0, 0); //--- �������� ��ġ ����
    glutInitWindowSize(800, 600); //--- �������� ũ�� ����
    glutCreateWindow("Example4"); //--- ������ ����(������ �̸�)
    //--- GLEW �ʱ�ȭ
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) //--- glew �ʱ�ȭ 
    {
        std::cerr << "Unable to initialize GLEW" << std::endl;
        exit(EXIT_FAILURE);
    }
    else
        std::cout << "GLEW Initialized\n";
    glutDisplayFunc(drawScene); //--- ��� �ݹ��Լ��� ����
    glutReshapeFunc(Reshape); //--- �ٽ� �׸��� �ݹ��Լ� ����
    glutKeyboardFunc(Keyboard); //--- Ű���� �Է� �ݹ��Լ� ����
    glutMouseFunc(Mouse);     // Ŭ��/���� �̺�Ʈ

	glutTimerFunc(10, OnTimer, 0); //Ÿ�̸� �̺�Ʈ

    glutMainLoop(); //--- �̺�Ʈ ó�� ����


}

//--- �簢�� �׸��� �Լ�
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

//--- Ÿ�̸� �Լ� (��ü)
void OnTimer(int) {
    const float dt = 0.1f / 60.0f; // ������ ����
    int winw = glutGet(GLUT_WINDOW_WIDTH);
    int winh = glutGet(GLUT_WINDOW_HEIGHT);

    for (int i = 0; i < rects.size(); i++) {
        Rect& t = rects[i];

        // =====================
        // �����ϱ� ó��
        // =====================
        if (t.followLeader) {
            Rect& leader = rects[t.followTarget];
            t.followTimer += dt;

            if (t.followTimer >= t.followDelay) {
                // ������ ���¸� �״�� ����
                t.x1 = leader.x1;
                t.y1 = leader.y1;
                t.x2 = leader.x2;
                t.y2 = leader.y2;
                t.r = leader.r;
                t.g = leader.g;
                t.b = leader.b;
            }
            continue; // ���󰡴� �簢���� ���� �ִϸ��̼� �ǳʶ�
        }

        // =====================
        // ���� �ִϸ��̼� ó��
        // =====================

        // ��ġ ��ȭ 1 : �밢�� �̵� + �ݻ�
        if (t.aniPos1) {
            t.x1 += t.vx * dt;
            t.y1 += t.vy * dt;
            t.x2 += t.vx * dt;
            t.y2 += t.vy * dt;
            if (t.x1 < 0 || t.x2 > winw) t.vx = -t.vx;
            if (t.y1 < 0 || t.y2 > winh) t.vy = -t.vy;
        }

        // ��ġ ��ȭ 2 : ���� �̵�
        if (t.aniPos2) {
            float speed = 500;
            float move = speed * dt;

            // ���� ���� ���⿡ ���� �̵�
            if (t.zigdir == 0) { // ��
                t.x1 += move; t.x2 += move;
            }
            else if (t.zigdir == 1) { // ��
                t.y1 += move; t.y2 += move;
            }
            else if (t.zigdir == 2) { // ��
                t.x1 -= move; t.x2 -= move;
            }
            else if (t.zigdir == 3) { // ��
                t.y1 -= move; t.y2 -= move;
            }

            t.zigtime += dt;
            if (t.zigtime > 0.5f) {
                // ���� �ð����� ���� ��ȯ
                if (t.goingUp) {
                    // �ö󰡴� ���� : �� �� �� �� �ݺ�
                    if (t.zigdir == 0) t.zigdir = 1;
                    else if (t.zigdir == 1) t.zigdir = 2;
                    else if (t.zigdir == 2) t.zigdir = 1;
                }
                else {
                    // �������� ���� : �� �� �� �� �ݺ�
                    if (t.zigdir == 0) t.zigdir = 3;
                    else if (t.zigdir == 3) t.zigdir = 2;
                    else if (t.zigdir == 2) t.zigdir = 3;
                }
                t.zigtime = 0;
            }

            float w = t.x2 - t.x1;
            float h = t.y2 - t.y1;

            // --- õ��/�ٴ� ó�� ---
            if (t.y2 >= winh) {
                // õ�忡 ������ �������� ����
                t.y2 = winh; t.y1 = winh - h;
                t.goingUp = false;
                t.zigdir = 3;   // ��� ��ȯ
                t.zigtime = 0;
            }
            if (t.y1 <= 0) {
                // �ٴڿ� ������ �ö󰡱� ����
                t.y1 = 0; t.y2 = h;
                t.goingUp = true;
                t.zigdir = 1;   // ��� ��ȯ
                t.zigtime = 0;
            }

            // �¿� �� ó�� (Ƣ����� �ʵ��� ����)
            if (t.x1 < 0) {
                t.x1 = 0; t.x2 = w;
            }
            if (t.x2 > winw) {
                t.x2 = winw; t.x1 = winw - w;
            }
        }



        // ũ�� ��ȭ
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

        // ���� ��ȭ
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


//--- ȭ�� ���
GLvoid drawScene()
{
    //--- ����� ���� ����
    glClearColor(Bgcolor[0], Bgcolor[1], Bgcolor[2], 1.0f);//--- �������� ����
    glClear(GL_COLOR_BUFFER_BIT); //--- ������ ������ ��ü�� ĥ�ϱ�

    for (const auto& r : rects) {
        DrawRect(r);
    }

    glutSwapBuffers(); //--- ȭ�鿡 ���
}

//--- ������ ũ�� ����� ��ǥ�� �缳��
GLvoid Reshape(int w, int h)
{
    glViewport(0, 0, w, h);          // ������ ��ü�� �׸��� �������� ����
    glMatrixMode(GL_PROJECTION);     // ���� ��� ���� ��ȯ
    glLoadIdentity();                // ���� ���� �ʱ�ȭ
    gluOrtho2D(0, w, 0, h);          // 2D ��ǥ�� ���� (0~w, 0~h)
    glMatrixMode(GL_MODELVIEW);      // �ٽ� �𵨺� ��� ���� ��ȯ
}

//--- Ű���� �Է� ó��
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

        float delayStep = 0.5f; // �ڵ����� �簢�� �� ������

        for (int i = 0; i < rects.size(); i++) {
            if (i == leaderIndex) continue;
            rects[i].aniPos1 = leader.aniPos1;
            rects[i].aniPos2 = leader.aniPos2;
            rects[i].aniSize = leader.aniSize;
            rects[i].aniColor = leader.aniColor;

            // ������� ���� �� ���� ��ġ ����
            rects[i].zigdir = leader.zigdir;
            rects[i].zigtime = leader.zigtime;
        }
        break;
    }

    case 's':for (auto& t : rects) { t.aniPos2 = t.aniPos1 = t.aniSize = t.aniColor = false; }; break;
    case 'm':for (auto& t : rects) {
        float w = t.x2 - t.x1;
        float h = t.y2 - t.y1;
        // �߽��� ���� ��ġ�� �ǵ���
        t.x1 = t.initX - w * 0.5f;
        t.x2 = t.initX + w * 0.5f;
        t.y1 = t.initY - h * 0.5f;
        t.y2 = t.initY + h * 0.5f;
    }; break;
    case 'r':rects.clear(); break;
    case 'q':exit(0); break; //--- ���α׷� ����
    }
    glutPostRedisplay(); //--- ������ �ٲ� ������ ��� �ݹ� �Լ��� ȣ���Ͽ� ȭ���� refresh �Ѵ�
}

//--- ���콺 Ŭ��
GLvoid Mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        int winH = glutGet(GLUT_WINDOW_HEIGHT);
        float cx = (float)x;
        float cy = (float)(winH - y); //OpenGL ��ǥ��� ��ȯ

        if (rects.size() < 5) {
            Rect newRect{};
            float w = 50 + rand() % 70; //�簢�� �ʺ�
            float h = 50 + rand() % 70; //�簢�� ����
            //�簢�� ����
            newRect.r = (rand() % 100) / 100.0f;
            newRect.g = (rand() % 100) / 100.0f;
            newRect.b = (rand() % 100) / 100.0f;
            //�簢�� ��ġ
            newRect.x1 = cx - w * 0.5f;
            newRect.y1 = cy - h * 0.5f;
            newRect.x2 = newRect.x1 + w;
            newRect.y2 = newRect.y1 + h;
            // �߽� ��ǥ ����
            newRect.initX = cx;
            newRect.initY = cy;
            //�ӵ�/��ȭ�� �ʱⰪ
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

