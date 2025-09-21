#include <iostream>
#include<vector>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

using namespace std;

// �簢�� ����ü
struct Rect {
    float x1, y1, x2, y2; // �簢�� ��ǥ
    float r, g, b;        // ���� ����
    bool isSelected = false; // ���� ����
};

// �簢�� �����̳�
vector<Rect> rects;

//�Լ�����
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid InitRects(int winW, int winH);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Mouse(int button, int state, int x, int y);
GLvoid Motion(int x, int y);



//��������
float Bgcolor[3] = { 1.0f, 1.0f, 1.0f }; //--- ���� ����
int selectedRectIndex = -1; // ���õ� �簢�� �ε���
int prevMouseX = -1, prevMouseY = -1; // ���� ���콺 ��ġ
const float RECTSIZE = 30.0f;
bool isOverlap(const Rect& r1, const Rect& r2) {
    return !(r1.x2 < r2.x1 || r1.x1 > r2.x2 || r1.y2 < r2.y1 || r1.y1 > r2.y2);
}
Rect eraser;
bool hasEraser = false;
float eraserSize = RECTSIZE * 2;
int erasedCount = 0;


//������ ����ϰ� �ݹ��Լ� ����
void main(int argc, char** argv)
{
    //������ ����
    glutInit(&argc, argv); //glut �ʱ�ȭ
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); //���÷��� ��� ����
    glutInitWindowPosition(0, 0); //�������� ��ġ ����
    glutInitWindowSize(800, 600); //�������� ũ�� ����
    glutCreateWindow("Example1"); //������ ����(������ �̸�)
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
    glutMouseFunc(Mouse);     //Ŭ��/���� �̺�Ʈ
    glutMotionFunc(Motion);   //�巡�� �̺�Ʈ
    glutMainLoop(); //�̺�Ʈ ó�� ����


}

//�簢�� �׸��� �Լ�
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

//ȭ�� ���
GLvoid drawScene()
{
    // ����� ���� ����
    glClearColor(Bgcolor[0], Bgcolor[1], Bgcolor[2], 1.0f);//������ ����
    glClear(GL_COLOR_BUFFER_BIT); //������ ������ ��ü�� ĥ�ϱ�

    for (const auto& r : rects) {
        DrawRect(r);
    }

    if(hasEraser)
		DrawRect(eraser);

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
    case 'r':InitRects(800,600); break;
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
	//��Ŭ��-�簢�� ����
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

                // ���찳 �߽� �����ϸ� ũ�� �缳��
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

//���콺 �巡�� ó��
GLvoid Motion(int x, int y) {
    if (hasEraser) {
        int winH = glutGet(GLUT_WINDOW_HEIGHT);
        int mouseY = winH - y; // OpenGL ��ǥ�迡 �°� ��ȯ
        int dx = x - prevMouseX;
        int dy = mouseY - prevMouseY;

        //���찳 �̵�
		eraser.x1 += dx;
		eraser.x2 += dx;
		eraser.y1 += dy;
		eraser.y2 += dy;

        //�����
        for (int i = 0; i < rects.size();) {
            if (isOverlap(eraser, rects[i])) {
                //���� ���� ����
                erasedCount++;

                //���찳 ũ�� ���� 
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