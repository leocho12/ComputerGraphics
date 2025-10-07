#include "GLmain.h"

//각종함수들 작성공간

float centerX, centerY;


GLvoid drawScene()
{
	glClearColor(bGCr, bGCg, bGCb, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramID);
	UpdateBuffer();
	glBindVertexArray(vao);

	if (frame)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	for (int i = 0; i < 4; i++)
	{
		glDrawArrays(GL_TRIANGLES, i * 3, 3);
	}
	glutSwapBuffers();
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'p':
		frame = false;
		break;
	case 'l':
		frame = true;
		break;
	case '1':
		
		break;
	case '2':
		
		break;
	case '3':
		
		break;
	case '4':
		
		break;
	case '5':

		break;
	case 'c':
		
		break;
	case 'q':
		glutLeaveMainLoop();
		break;
	}
	glutPostRedisplay();
	
}


GLvoid Mouse(int button, int state, int x, int y)
{
	float fx = 0.0, fy = 0.0;
	convertXY(x, y, fx, fy);
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		
	}
	glutPostRedisplay();
}

void InitBuffer()
{
	glGenVertexArrays(1, &vao); //--- VAO 를 지정하고 할당하기
	glBindVertexArray(vao); //--- VAO를 바인드하기

	glGenBuffers(2, vbo); //--- 2개의 VBO를 지정하고 할당하기
	//--- 1번째 VBO를 활성화하여 바인드하고, 버텍스 속성 (좌표값)을 저장
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	//--- 변수 diamond 에서 버텍스 데이터 값을 버퍼에 복사한다.
	//--- triShape 배열의 사이즈: 9 * float
	glBufferData(GL_ARRAY_BUFFER, 4 * 9 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
	//--- 좌표값을 attribute 인덱스 0번에 명시한다: 버텍스 당 3* float
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//--- attribute 인덱스 0번을 사용가능하게 함
	glEnableVertexAttribArray(0);

	//--- 2번째 VBO를 활성화 하여 바인드 하고, 버텍스 속성 (색상)을 저장
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	//--- 변수 colors에서 버텍스 색상을 복사한다.
	//--- colors 배열의 사이즈: 9 *float
	glBufferData(GL_ARRAY_BUFFER, 4 * 9 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
	//--- 색상값을 attribute 인덱스 1번에 명시한다: 버텍스 당 3*float
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//--- attribute 인덱스 1번을 사용 가능하게 함.
	glEnableVertexAttribArray(1);
}

void UpdateBuffer()
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	for (int i = 0; i < 4; i++)
	{
		glBufferSubData(GL_ARRAY_BUFFER, i * 9 * sizeof(GLfloat), 9 * sizeof(GLfloat), shape[i].shapecoord);
	}

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);

	for (int i = 0; i < 4; i++)
	{
		glBufferSubData(GL_ARRAY_BUFFER, i * 9 * sizeof(GLfloat), 9 * sizeof(GLfloat), shape[i].color);
	}
}

