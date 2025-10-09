#pragma once
#include "GLbasic.h"
//class�� main�Լ� �������
bool start = false;
bool animating = true;
int selectedIndex = -1;
float lastMouseX = 0.0f, lastMouseY = 0.0f;

class Shape
{
public:
	GLfloat shapecoord[5][3];
	GLfloat color[5][3];
	int shapetype;
    bool done;
    bool Process[6];
	bool isPoint; // 점 애니메이션 여부
	float vx, vy; // 점 속도
    bool isAnimated; // 결합 후 애니메이션 여부
	Shape() : shapetype{ 0 }, done{ true } {
		for (int i = 0; i < 6; i++) {
			Color(color[0][i]);
			Process[i] = true;
		}
		for (int j = 0; j < 5; j++) {
			for (int k = 0; k < 3; k++) {
				shapecoord[j][k] = 0.0f;
			}
			color[j][0] = color[0][0];
			color[j][1] = color[0][1];
			color[j][2] = color[0][2];
		}
		isPoint = false;
		vx = 0.01f; vy = 0.015f;
        isAnimated = false;
	};

	void Colors()
	{
		for (int i = 0; i < 3; i++)
		{
			Color(color[0][i]);
		}
		for (int j = 0; j < 5; j++) {
			color[j][0] = color[0][0];
			color[j][1] = color[0][1];
			color[j][2] = color[0][2];
		}
	}
};

Shape shape[MAX_SHAPES];
int shapeCount = 0;

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("Example12");

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized\n";

	make_shaderProgram();
	ResetShapes();
	InitBuffer();
	glutDisplayFunc(drawScene);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutReshapeFunc(Reshape);
	glutMainLoop();
	return 0;
}