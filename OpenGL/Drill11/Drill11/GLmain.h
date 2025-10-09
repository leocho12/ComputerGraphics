#pragma once
#include "GLbasic.h"
//class와 main함수 선언공간
class Shape
{
public:
	GLfloat shapecoord[3];
	GLfloat color[3];

	Shape() {
		for (int i = 0; i < 3; i++) {
			Color(color[i]);
			shapecoord[i] = 0.0f;
		}
	}

	void Colors()
	{
		for (int i = 0; i < 3; i++)
		{
			Color(color[i]);
		}
	}
};



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