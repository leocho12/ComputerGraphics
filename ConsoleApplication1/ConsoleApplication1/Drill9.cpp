//--- �ʿ��� ������� ����
#define _CRT_SECURE_NO_WARNINGS //--- ���α׷� �� �տ� ������ ��
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <vector>

using namespace std;
//--- �Ʒ� 5�� �Լ��� ����� ���� �Լ� ��
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
GLvoid drawScene();
GLvoid Reshape(int w, int h);
//�߰� �Լ�
GLvoid keyboard(unsigned char key, int x, int y);
GLvoid mouse(int button, int state, int x, int y);
//--- �ʿ��� ���� ����
GLint width, height;
GLuint shaderProgramID; //--- ���̴� ���α׷� �̸�
GLuint vertexShader; //--- ���ؽ� ���̴� ��ü
GLuint fragmentShader; //--- �����׸�Ʈ ���̴� ��ü
GLuint VAO;//VBO��� �뵵
GLuint VBO;//���� �����͸� GPU�� �Ѱ��� ���۰���(������/��ư �ּ� ��������) 

enum Shapes { Point, Line, Tri, Rect };
struct Shape {
	Shapes type;
	float x, y;//�߽���ǥ
	float size;//ũ��
	float r, g, b;//����
};
vector<Shape> shapes;
int selected = -1;//���� ���ÿ���
Shapes Mode = Point;//���� ����

char* filetobuf(const char* file)
{
	FILE* fptr;
	long length;
	char* buf;
	fptr = fopen(file, "rb"); // Open file for reading
	if (!fptr) // Return NULL on failure
		return NULL;
	fseek(fptr, 0, SEEK_END); // Seek to the end of the file
	length = ftell(fptr); // Find out how many bytes into the file we are
	buf = (char*)malloc(length + 1); // Allocate a buffer for the entire length of the file and a null terminator
	fseek(fptr, 0, SEEK_SET); // Go back to the beginning of the file
	fread(buf, length, 1, fptr); // Read the contents of the file in to the buffer
	fclose(fptr); // Close the file
	buf[length] = 0; // Null terminator
	return buf; // Return the buffer
}

//--- ���� �Լ�
void main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ����
{
	width = 500;
	height = 500;
	//--- ������ �����ϱ�
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(width, height);
	glutCreateWindow("Example1");
	//--- GLEW �ʱ�ȭ�ϱ�
	glewExperimental = GL_TRUE;
	glewInit();
	//--- ���̴� �о�ͼ� ���̴� ���α׷� �����: ����� �����Լ� ȣ��
	make_vertexShaders(); //--- ���ؽ� ���̴� �����
	make_fragmentShaders(); //--- �����׸�Ʈ ���̴� �����
	shaderProgramID = make_shaderProgram();
	//--- ���̴� ���α׷� �����
	glutDisplayFunc(drawScene); //--- ��� �ݹ� �Լ�
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMainLoop();
}

//--- ���ؽ� ���̴� ��ü �����
void make_vertexShaders()
{
	GLchar* vertexSource;
	//--- ���ؽ� ���̴� �о� �����ϰ� ������ �ϱ�
	//--- filetobuf: ��������� �Լ��� �ؽ�Ʈ�� �о ���ڿ��� �����ϴ� �Լ�
	vertexSource = filetobuf("vertex.glsl");
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{

		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);

		std::cerr << "ERROR: vertex shader ������ ����\n" << errorLog << std::endl;

		return;
	}
}
//--- �����׸�Ʈ ���̴� ��ü �����
void make_fragmentShaders()
{
	GLchar* fragmentSource;
	//--- �����׸�Ʈ ���̴� �о� �����ϰ� �������ϱ�
	fragmentSource = filetobuf("fragment.glsl"); // �����׼��̴� �о����
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: frag_shader ������ ����\n" << errorLog << std::endl;
		return;
	}
}
//--- ���̴� ���α׷� ����� ���̴� ��ü ��ũ�ϱ�
GLuint make_shaderProgram()
{
	GLint result;
	GLchar* errorLog = NULL;
	GLuint shaderID;
	shaderID = glCreateProgram(); //--- ���̴� ���α׷� �����
	glAttachShader(shaderID, vertexShader); //--- ���̴� ���α׷��� ���ؽ� ���̴� ���̱�
	glAttachShader(shaderID, fragmentShader); //--- ���̴� ���α׷��� �����׸�Ʈ ���̴� ���̱�
	glLinkProgram(shaderID); //--- ���̴� ���α׷� ��ũ�ϱ�
	glDeleteShader(vertexShader); //--- ���̴� ��ü�� ���̴� ���α׷��� ��ũ��������, ���̴� ��ü ��ü�� ���� ����
	glDeleteShader(fragmentShader);
	glGetProgramiv(shaderID, GL_LINK_STATUS, &result); // ---���̴��� �� ����Ǿ����� üũ�ϱ�
	if (!result) {
		glGetProgramInfoLog(shaderID, 512, NULL, errorLog);
		std::cerr << "ERROR: shader program ���� ����\n" << errorLog << std::endl;
		return false;
	}
	glUseProgram(shaderID); //--- ������� ���̴� ���α׷� ����ϱ�
	//--- ���� ���� ���̴����α׷� ���� �� �ְ�, �� �� �Ѱ��� ���α׷��� ����Ϸ���
	//--- glUseProgram �Լ��� ȣ���Ͽ� ��� �� Ư�� ���α׷��� �����Ѵ�.
	//--- ����ϱ� ������ ȣ���� �� �ִ�.
	return shaderID;
}
//--- ��� �ݹ� �Լ�
GLvoid drawScene() //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //--- ���� ����
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(shaderProgramID);

	for (auto& s : shapes) {
		glColor3f(s.r, s.g, s.b);

		switch (s.type) {
		case Point:
			glPointSize(5.0f);
			glBegin(GL_POINTS);
			glVertex2f(s.x, s.y);
			glEnd();
			break;
		case Line:
			glBegin(GL_LINES);
			glVertex2f(s.x - s.size, s.y);
			glVertex2f(s.x + s.size, s.y);
			glEnd();
			break;
		case Tri:
			glBegin(GL_TRIANGLES);
			glVertex2f(s.x, s.y + s.size);
			glVertex2f(s.x - s.size, s.y - s.size);
			glVertex2f(s.x + s.size, s.y - s.size);
			glEnd();
			break;
		case Rect:
			glBegin(GL_TRIANGLES);
			// ù ��° �ﰢ�� (�»�, ����, ���)
			glVertex2f(s.x - s.size, s.y + s.size); // �»�
			glVertex2f(s.x - s.size, s.y - s.size); // ����
			glVertex2f(s.x + s.size, s.y + s.size); // ���

			// �� ��° �ﰢ�� (���, ����, ����)
			glVertex2f(s.x + s.size, s.y + s.size); // ���
			glVertex2f(s.x - s.size, s.y - s.size); // ����
			glVertex2f(s.x + s.size, s.y - s.size); // ����
			glEnd();
			break;
		}
	}

	glutSwapBuffers(); // ȭ�鿡 ����ϱ�
}
GLvoid keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'p':Mode = Point; break;
	case 'l':Mode = Line; break;
	case 't':Mode = Tri; break;
	case 'r':Mode = Rect; break;
	case'c':
		shapes.clear();
		selected = -1;
		break;
	case 'w': if (selected != -1)shapes[selected].y += 0.05f; break;
	case 's': if (selected != -1)shapes[selected].y -= 0.05f; break;
	case 'a': if (selected != -1)shapes[selected].x -= 0.05f; break;
	case 'd': if (selected != -1)shapes[selected].x += 0.05f; break;
	case 'u': //�»�
		if (selected != -1) {
			shapes[selected].x -= 0.05f;
			shapes[selected].y += 0.05f;
		}
		break;
	case 'j': //����
		if (selected != -1) {
			shapes[selected].x -= 0.05f;
			shapes[selected].y -= 0.05f;
		}
		break;
	case 'k': //����
		if (selected != -1) {
			shapes[selected].x += 0.05f;
			shapes[selected].y -= 0.05f;
		}
		break;
	case 'i': //���
		if (selected != -1) {
			shapes[selected].x += 0.05f;
			shapes[selected].y += 0.05f;
		}
		break;
	}
	glutPostRedisplay();
}

GLvoid mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		//ȭ�� ��ǥ->OpenGL ��ǥ ��ȯ
		float nx = (float)x / width * 2.0f - 1.0f;
		float ny = 1.0f - (float)y / height * 2.0f;

		selected = -1;
		for (int i = 0; i < shapes.size(); i++) {
			float dx = nx - shapes[i].x;
			float dy = ny - shapes[i].y;
			if (dx * dx + dy * dy < shapes[i].size * shapes[i].size) {
				selected = i;
				break;
			}
		}
		if (selected == -1 && shapes.size() < 10) {
			shapes.push_back({ Mode,nx,ny,0.1f,(float)rand() / RAND_MAX,(float)rand() / RAND_MAX,(float)rand() / RAND_MAX });
		}
	}
	glutPostRedisplay();
}

//--- �ٽñ׸��� �ݹ� �Լ�
GLvoid Reshape(int w, int h) //--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ�
{
	glViewport(0, 0, w, h);
}