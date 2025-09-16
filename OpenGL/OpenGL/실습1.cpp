#include <iostream>
#include <gl/glew.h> //--- �ʿ��� ������� include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Timer(int value);

float Bgcolor[3] = { 1.0f, 1.0f, 1.0f }; //--- ���� ����
bool TimerOn = false;

void main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ����
{
	//--- ������ �����ϱ�
	glutInit(&argc, argv); //--- glut �ʱ�ȭ
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); //--- ���÷��� ��� ����
	glutInitWindowPosition(0, 0); //--- �������� ��ġ ����
	glutInitWindowSize(800, 600); //--- �������� ũ�� ����
	glutCreateWindow("Example1"); //--- ������ ����(������ �̸�)
	//--- GLEW �ʱ�ȭ�ϱ�
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
GLvoid drawScene() //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
{
	//--- ����� ���� ����
	glClearColor(Bgcolor[0], Bgcolor[1], Bgcolor[2], 1.0f);//--- �������� ����
	glClear(GL_COLOR_BUFFER_BIT); //--- ������ ������ ��ü�� ĥ�ϱ�
	glutSwapBuffers(); //--- ȭ�鿡 ����ϱ�
}
GLvoid Reshape(int w, int h) //--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ�
{
	glViewport(0, 0, w, h);
}
GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'c': Bgcolor[0] = 0.0f; Bgcolor[1] = 1.0f; Bgcolor[2] = 1.0f; break; //--- ������ û�ϻ����� ����
	case 'm': Bgcolor[0] = 1.0f; Bgcolor[1] = 0.0f; Bgcolor[2] = 1.0f; break; //--- ������ ��ȫ������ ����
	case 'y': Bgcolor[0] = 1.0f; Bgcolor[1] = 1.0f; Bgcolor[2] = 0.0f; break; //--- ������ Ȳ������ ����
	case 'a':Bgcolor[0] = (float)rand() / RAND_MAX; Bgcolor[1] = (float)rand() / RAND_MAX; Bgcolor[2] = (float)rand() / RAND_MAX; break; //--- ������ ���������� ����
	case 'w':Bgcolor[0] = 1.0f; Bgcolor[1] = 1.0f; Bgcolor[2] = 1.0f; break; //--- ������ ������� ����
	case 'k':Bgcolor[0] = 0.0f; Bgcolor[1] = 0.0f; Bgcolor[2] = 0.0f; break; //--- ������ ������� ����
	case 't':TimerOn = true; glutTimerFunc(500, Timer, 1); break; //--- Ÿ�̸� ������ Ư���ð�����	 ������ �������� ����
	case 's':TimerOn = false; break; //--- Ÿ�̸� ����
	case 'q':exit(0); break; //--- ���α׷� ����
	}
	glutPostRedisplay(); //--- ������ �ٲ� ������ ��� �ݹ� �Լ��� ȣ���Ͽ� ȭ���� refresh �Ѵ�
}
GLvoid Timer(int value)
{
	if (TimerOn) {
		// ������ ����
		Bgcolor[0] = (float)rand() / RAND_MAX;
		Bgcolor[1] = (float)rand() / RAND_MAX;
		Bgcolor[2] = (float)rand() / RAND_MAX;

		glutPostRedisplay();
		glutTimerFunc(500, Timer, 1); // �ٽ� Ÿ�̸� ���
	}
}