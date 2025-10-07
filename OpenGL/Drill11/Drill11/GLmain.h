#pragma once
#include "GLbasic.h"
//class와 main함수 선언공간
class Shape
{
public:
	GLfloat shapecoord[3][3];
	float size;
	int Way;
	int theta;
	float r;
	bool var;
	float rotate;
	float speed;
	float spiralOffset;
	GLfloat color[3][3];
	Shape() : size{ 1.0 }, Way{ 0 }, theta{ 0 }, r{ 0.1 }, var{ false }, rotate{ 0.0 }, speed{ 0.1 }
	{
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				shapecoord[i][j] = 0.5;
			}
		}
		for (int i = 0; i < 3; i++)
		{
			Color(color[0][i]);
		}
		for (int i = 1; i < 3; i++)
		{
			color[i][0] = color[0][0];
			color[i][1] = color[0][1];
			color[i][2] = color[0][2];
		}
	};

	void Colors()
	{
		for (int i = 0; i < 3; i++)
		{
			Color(color[0][i]);
		}
		for (int i = 1; i < 3; i++)
		{
			color[i][0] = color[0][0];
			color[i][1] = color[0][1];
			color[i][2] = color[0][2];
		}
	}
};

Shape shape[4];

void main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("Example11");

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized\n";

	make_shaderProgram();
	InitBuffer();
	glutDisplayFunc(drawScene);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(Mouse);
	glutReshapeFunc(Reshape);
	glutMainLoop();
}