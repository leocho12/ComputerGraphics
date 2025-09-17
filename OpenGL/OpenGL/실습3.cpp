#include <iostream>
#include<vector>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

using namespace std;

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid AddRandomRect(int winW, int winH);

// �簢�� ����ü
struct Rect {
	float x1, y1, x2, y2; // �簢�� ��ǥ
	float r, g, b;        // ���� ����
	bool isSelected = false; // ���� ����
};

// �簢�� �����̳�
vector<Rect> rects;

float Bgcolor[3] = { 1.0f, 1.0f, 1.0f }; //--- ���� ����


void main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ����
{
	//--- ������ ����
	glutInit(&argc, argv); //--- glut �ʱ�ȭ
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); //--- ���÷��� ��� ����
	glutInitWindowPosition(0, 0); //--- �������� ��ġ ����
	glutInitWindowSize(800, 600); //--- �������� ũ�� ����
	glutCreateWindow("Example1"); //--- ������ ����(������ �̸�)
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
	glutMainLoop(); //--- �̺�Ʈ ó�� ����
}

void DrawRect(const Rect& rect) //--- �簢�� �׸��� �Լ�
{
	glColor3f(rect.r, rect.g, rect.b);
	glBegin(GL_QUADS);
	glVertex2f(rect.x1, rect.y1);
	glVertex2f(rect.x2, rect.y1);
	glVertex2f(rect.x2, rect.y2);
	glVertex2f(rect.x1, rect.y2);
	glEnd();
}

GLvoid drawScene() //--- ȭ�� ���
{
	//--- ����� ���� ����
	glClearColor(Bgcolor[0],Bgcolor[1],Bgcolor[2], 1.0f);//--- �������� ����
	glClear(GL_COLOR_BUFFER_BIT); //--- ������ ������ ��ü�� ĥ�ϱ�
	
	for(const auto& r : rects) {
		DrawRect(r);
	}

	glutSwapBuffers(); //--- ȭ�鿡 ���
}

GLvoid Reshape(int w, int h)//--- ������ ũ�� ����� ��ǥ�� �缳��
{
	glViewport(0, 0, w, h);          // ������ ��ü�� �׸��� �������� ����
	glMatrixMode(GL_PROJECTION);     // ���� ��� ���� ��ȯ
	glLoadIdentity();                // ���� ���� �ʱ�ȭ
	gluOrtho2D(0, w, 0, h);          // 2D ��ǥ�� ���� (0~w, 0~h)
	glMatrixMode(GL_MODELVIEW);      // �ٽ� �𵨺� ��� ���� ��ȯ
}


GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'a':AddRandomRect(800,600);
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

	newRect.isSelected = false;

	//--- 30�� ��������
	if(rects.size()<30)
		rects.push_back(newRect);

	return GLvoid();
}
