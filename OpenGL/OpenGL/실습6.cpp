#include <iostream>
#include <vector>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

using namespace std;

//�簢�� ����ü
struct Rect {
    float cx,cy; //�簢�� �߽� ��ǥ
    float r, g, b;        //���� ����
	float vx = 0, vy = 0; //�ӵ�
    float size;           //ũ��
	bool AnimationActive = false;//�ִϸ��̼� Ȱ��ȭ ����
};

//Global ����
float Bgcolor[3] = { 1.0f, 1.0f, 1.0f }; //���� ����
int mode = 1;
//�簢�� �����̳�
vector<Rect> rects;

//�Լ�����
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid InitRects(int winW, int winH);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Mouse(int button, int state, int x, int y);



//������ ����ϰ� �ݹ��Լ� ����
void main(int argc, char** argv)
{
    //������ ����
    glutInit(&argc, argv); //glut �ʱ�ȭ
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); //���÷��� ��� ����
    glutInitWindowPosition(0, 0); //�������� ��ġ ����
    glutInitWindowSize(800, 600); //�������� ũ�� ����
    glutCreateWindow("Example6"); //������ ����(������ �̸�)
    //GLEW �ʱ�ȭ
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) //glew �ʱ�ȭ 
    {
        std::cerr << "Unable to initialize GLEW" << std::endl;
        exit(EXIT_FAILURE);
    }
    else
        std::cout << "GLEW Initialized\n";

    InitRects(800, 600);

    glutDisplayFunc(drawScene); //��� �ݹ��Լ��� ����
    glutReshapeFunc(Reshape); //�ٽ� �׸��� �ݹ��Լ� ����
    glutKeyboardFunc(Keyboard); //Ű���� �Է� �ݹ��Լ� ����
    glutMouseFunc(Mouse); //Ŭ��/���� �̺�Ʈ
	glutTimerFunc(16, Timer, 0); //Ÿ�̸� �̺�Ʈ
    glutMainLoop(); //�̺�Ʈ ó�� ����


}

//�簢�� �׸��� �Լ�
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

//ȭ�� ���
GLvoid drawScene()
{
    // ����� ���� ����
    glClearColor(Bgcolor[0], Bgcolor[1], Bgcolor[2], 1.0f);//������ ����
    glClear(GL_COLOR_BUFFER_BIT); //������ ������ ��ü�� ĥ�ϱ�

    for (const auto& r : rects) {
        DrawRect(r);
    }

   

    glutSwapBuffers(); //ȭ�鿡 ���
}

//������ ũ�� ����� ��ǥ�� �缳��
GLvoid Reshape(int w, int h)
{
    glViewport(0, 0, w, h);          // ������ ��ü�� �׸��� �������� ����
    glMatrixMode(GL_PROJECTION);     // ���� ��� ���� ��ȯ
    glLoadIdentity();                // ���� ���� �ʱ�ȭ
    gluOrtho2D(0, w, 0, h);          // 2D ��ǥ�� ���� (0~w, 0~h)
    glMatrixMode(GL_MODELVIEW);      // �ٽ� �𵨺� ��� ���� ��ȯ
}

//Ű���� �Է� ó��
GLvoid Keyboard(unsigned char key, int x, int y)
{
    switch (key) {
    case'1':mode = 1; break;//�¿����
    case'2':mode = 1; break;//�밢��
    case'3':mode = 1; break;//�׷�
    case'4':mode = 1; break;//8����
    case 'r':InitRects(800, 600); break;//����
    case 'q':exit(0); break; //���α׷� ����
    }
    glutPostRedisplay(); //������ �ٲ� ������ ��� �ݹ� �Լ��� ȣ���Ͽ� ȭ���� refresh �Ѵ�
}


//���콺 Ŭ��
GLvoid Mouse(int button, int state, int x, int y)
{
    int winH = glutGet(GLUT_WINDOW_HEIGHT);
    int mouseY = winH - y; //OpenGL ��ǥ��� ��ȯ

    //��Ŭ��-���찳 ����
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
            r.size -= 0.2f;//���
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
