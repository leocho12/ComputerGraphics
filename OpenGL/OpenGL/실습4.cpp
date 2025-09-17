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
    bool aniPos = false;
    bool aniSize = false;
	bool aniColor = false;
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

//--- Ÿ�̸� �Լ�
void OnTimer(int) {
	const float dt = 0.1f / 60.0f; // ������ ����
	int winw = glutGet(GLUT_WINDOW_WIDTH);
	int winh = glutGet(GLUT_WINDOW_HEIGHT);

    for (auto& t : rects) {
        //��ġ
        if (t.aniPos) {
            t.x1 += t.vx * dt;
            t.y1 += t.vy * dt;
            t.x2 += t.vx * dt;
            t.y2 += t.vy * dt;
            //�� �ݻ�
            if (t.x1 < 0 || t.x2 > winw) t.vx = -t.vx;
			if (t.y1 < 0 || t.y2 > winh) t.vy = -t.vy;
        }
		//ũ��
        if (t.aniSize) {
			float w = (t.x2 - t.x1) + t.dw * dt;
			float h = (t.y2 - t.y1) + t.dh * dt;
			float cx = (t.x1 + t.x2) * 0.5f;//�߽��� ���
			float cy = (t.y1 + t.y2) * 0.5f;

            if (w < 20 || w > 200) { t.dw = -t.dw; w = max(20.0f, min(200.f, w)); }
			if (h < 20 || h > 200) { t.dh = -t.dh; h = max(20.0f, min(200.f, h)); }

			t.x1 = cx - w * 0.5f;
			t.x2 = cx + w * 0.5f;
			t.y1 = cy - h * 0.5f;
			t.y2 = cy + h * 0.5f;

        }
		//����
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
    case '1':for (auto& t : rects) t.aniPos = !t.aniPos; break;
    case '2':for (auto& t : rects) t.aniPos = !t.aniPos; break;
    case '3':for(auto& t:rects)t.aniSize=!t.aniSize; break;
    case '4':for (auto& t : rects) t.aniColor = !t.aniColor; break;
    case '5':; break;
    case 's':for (auto& t : rects) { t.aniPos = t.aniSize = t.aniColor = false; }; break;
    case 'm':; break;
    case 'r':rects.clear(); break;
    case 'q':exit(0); break; //--- ���α׷� ����
    }
    glutPostRedisplay(); //--- ������ �ٲ� ������ ��� �ݹ� �Լ��� ȣ���Ͽ� ȭ���� refresh �Ѵ�
}

GLvoid AddRandomRect(int winW, int winH)
{
    Rect newRect;

    float w = rand() % 100 + 20; //--- �簢���� �ʺ�
    float h = rand() % 100 + 20; //--- �簢���� ����

    //--- ��ġ
    newRect.x1 = rand() % (winW - (int)w);
    newRect.y1 = rand() % (winH - (int)h);
    newRect.x2 = newRect.x1 + w;
    newRect.y2 = newRect.y1 + h;

    //--- ����
    newRect.r = (float)(rand() % 100) / 100.0f;
    newRect.g = (float)(rand() % 100) / 100.0f;
    newRect.b = (float)(rand() % 100) / 100.0f;

    //--- 5�� ��������
    if (rects.size() < 5)
        rects.push_back(newRect);
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
            //�ӵ�/��ȭ�� �ʱ���
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

