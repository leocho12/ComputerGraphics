#include "GLmain.h"

//각종함수들 작성공간



//전	역변수
Shape shape1[220];
Shape shape2[220];
Shape shape3[220];
Shape shape4[220];
Shape shape5[220];

float r = 0.1f;//반지름
int count = 0;//스파이럴 개수
int shapeCount = 0;
int theta = 0;
float fx = 0.0f, fy = 0.0f;
float xx[4], yy[4];
bool outwardspiral = true;
bool frame = false;

GLvoid drawScene()
{
	glClearColor(bGCr, bGCg, bGCb, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramID);
	UpdateBuffer();
	glBindVertexArray(vao);

	glPointSize(5.0);

	for (int s = 0; s < shapeCount; s++) {
		int base = s * 220;              // 이 스파이럴의 시작 버텍스 인덱스
		int n = std::min(count, 220); // 그릴 버텍스 개수(안전)

		if (n <= 0) continue;

		if (frame)
			glDrawArrays(GL_LINE_STRIP, base, n); // 한 번에 n개
		else
			glDrawArrays(GL_POINTS, base, n); // 한 번에 n개
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
		shapeCount = 1;
		break;
	case '2':
		shapeCount = 2;
		break;
	case '3':
		shapeCount = 3;
		break;
	case '4':
		shapeCount = 4;
		break;
	case '5':
		shapeCount = 5;
		break;
	case 'c':
		shapeCount = 0;
		bGCr = bGCg = bGCb = 1.0f;
		break;
	case 'q':
		glutLeaveMainLoop();
		break;
	}
	glutPostRedisplay();
	
}


GLvoid Mouse(int button, int state, int x, int y)
{
	convertXY(x, y, fx, fy);
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		for (int i = 0; i < 220; i++) {
			shape1[i].shapecoord[0] = shape1[i].shapecoord[1] = 0.0f;
			shape2[i].shapecoord[0] = shape2[i].shapecoord[1] = 0.0f;
			shape3[i].shapecoord[0] = shape3[i].shapecoord[1] = 0.0f;
			shape4[i].shapecoord[0] = shape4[i].shapecoord[1] = 0.0f;
			shape5[i].shapecoord[0] = shape5[i].shapecoord[1] = 0.0f;
		}
		Color(bGCr);
		Color(bGCg);
		Color(bGCb);
		r = 0.01f;
		count = 0;
		theta = 0;
		outwardspiral = true;
		frame = false;
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<int> random(200, 600);
		std::uniform_int_distribution<int> random2(100, 500);
		if (shapeCount > 1) {
			int num = shapeCount - 1;
			for(int i=0;i<shapeCount-1;i++)
				convertXY(random(gen), random2(gen), xx[i], yy[i]);
			for (int i = num; i < 4; i++)
				xx[i] = yy[i] = 0.0f;
		}
		glutTimerFunc(10, TimerFunction, 1);
	}
	printf("shapeCount = %d, coords: ", shapeCount);
	for (int i = 0; i < shapeCount - 1; i++)
		printf("(%f, %f) ", xx[i], yy[i]);
	printf("\n");

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
	glBufferData(GL_ARRAY_BUFFER, 1100 * 3 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
	//--- 좌표값을 attribute 인덱스 0번에 명시한다: 버텍스 당 3* float
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//--- attribute 인덱스 0번을 사용가능하게 함
	glEnableVertexAttribArray(0);

	//--- 2번째 VBO를 활성화 하여 바인드 하고, 버텍스 속성 (색상)을 저장
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	//--- 변수 colors에서 버텍스 색상을 복사한다.
	//--- colors 배열의 사이즈: 9 *float
	glBufferData(GL_ARRAY_BUFFER, 1100 * 3 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
	//--- 색상값을 attribute 인덱스 1번에 명시한다: 버텍스 당 3*float
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//--- attribute 인덱스 1번을 사용 가능하게 함.
	glEnableVertexAttribArray(1);
}

void UpdateBuffer()
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	for (int i = 0; i < 1100; i++)
	{
		if (i < 220)
		{
			glBufferSubData(GL_ARRAY_BUFFER, i * 3 * sizeof(GLfloat), 3 * sizeof(GLfloat), shape1[i].shapecoord);
		}
		else if (220 <= i && i < 440)
		{
			glBufferSubData(GL_ARRAY_BUFFER, i * 3 * sizeof(GLfloat), 3 * sizeof(GLfloat), shape2[i - 220].shapecoord);
		}
		else if (440 <= i && i < 660)
		{
			glBufferSubData(GL_ARRAY_BUFFER, i * 3 * sizeof(GLfloat), 3 * sizeof(GLfloat), shape3[i - 440].shapecoord);
		}
		else if (660 <= i && i < 880)
		{
			glBufferSubData(GL_ARRAY_BUFFER, i * 3 * sizeof(GLfloat), 3 * sizeof(GLfloat), shape4[i - 660].shapecoord);
		}
		else if (880 <= i && i < 1100)
		{
			glBufferSubData(GL_ARRAY_BUFFER, i * 3 * sizeof(GLfloat), 3 * sizeof(GLfloat), shape5[i - 880].shapecoord);
		}
	}
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	for (int i = 0; i < 1100; i++)
	{
		if (i < 220)
		{
			glBufferSubData(GL_ARRAY_BUFFER, i * 3 * sizeof(GLfloat), 3 * sizeof(GLfloat), shape1[i].color);
		}
		else if (220 <= i && i < 440)
		{
			glBufferSubData(GL_ARRAY_BUFFER, i * 3 * sizeof(GLfloat), 3 * sizeof(GLfloat), shape2[i - 220].color);
		}
		else if (440 <= i && i < 660)
		{
			glBufferSubData(GL_ARRAY_BUFFER, i * 3 * sizeof(GLfloat), 3 * sizeof(GLfloat), shape3[i - 440].color);
		}
		else if (660 <= i && i < 880)
		{
			glBufferSubData(GL_ARRAY_BUFFER, i * 3 * sizeof(GLfloat), 3 * sizeof(GLfloat), shape4[i - 660].color);
		}
		else if (880 <= i && i < 1100)
		{
			glBufferSubData(GL_ARRAY_BUFFER, i * 3 * sizeof(GLfloat), 3 * sizeof(GLfloat), shape5[i - 880].color);
		}
	}
}

GLvoid TimerFunction(int value)
{
	// 원 스파이럴의 좌표 계산

	if (outwardspiral)
	{
		shape1[count].shapecoord[0] = fx + r * cos(theta * PI / 180);
		shape1[count].shapecoord[1] = fy + r * sin(theta * PI / 180);
		if (shapeCount > 1)
		{
			shape2[count].shapecoord[0] = xx[0] + r * cos(theta * PI / 180);
			shape2[count].shapecoord[1] = yy[0] + r * sin(theta * PI / 180);
		}
		if (shapeCount > 2)
		{
			shape3[count].shapecoord[0] = xx[1] + r * cos(theta * PI / 180);
			shape3[count].shapecoord[1] = yy[1] + r * sin(theta * PI / 180);
		}
		if (shapeCount > 3)
		{
			shape4[count].shapecoord[0] = xx[2] + r * cos(theta * PI / 180);
			shape4[count].shapecoord[1] = yy[2] + r * sin(theta * PI / 180);
		}
		if (shapeCount > 4)
		{
			shape5[count].shapecoord[0] = xx[3] + r * cos(theta * PI / 180);
			shape5[count].shapecoord[1] = yy[3] + r * sin(theta * PI / 180);
		}
		theta += 10; // 각도 증가 (시계 방향)
		r += 0.001; // 반지름 증가
		if (theta >= 1080)
		{ // 2바퀴 후에
			fx += r * 2;
			for (int i = 0; i < 4; i++)
			{
				xx[i] += r * 2;
			}
			outwardspiral = false; // 다음 단계로 전환
			theta = 1080; // 반대 방향 시작 각도
		}
	}
	else
	{
		shape1[count].shapecoord[0] = fx - r * cos(theta * PI / 180);
		shape1[count].shapecoord[1] = fy - r * sin(theta * PI / 180);
		if (shapeCount > 1)
		{
			shape2[count].shapecoord[0] = xx[0] - r * cos(theta * PI / 180);
			shape2[count].shapecoord[1] = yy[0] - r * sin(theta * PI / 180);
		}
		if (shapeCount > 2)
		{
			shape3[count].shapecoord[0] = xx[1] - r * cos(theta * PI / 180);
			shape3[count].shapecoord[1] = yy[1] - r * sin(theta * PI / 180);
		}
		if (shapeCount > 3)
		{
			shape4[count].shapecoord[0] = xx[2] - r * cos(theta * PI / 180);
			shape4[count].shapecoord[1] = yy[2] - r * sin(theta * PI / 180);
		}
		if (shapeCount > 4)
		{
			shape5[count].shapecoord[0] = xx[3] - r * cos(theta * PI / 180);
			shape5[count].shapecoord[1] = yy[3] - r * sin(theta * PI / 180);
		}
		r -= 0.001;
		theta -= 10; // 각도 감소 (반시계 방향)
	}

	glutPostRedisplay(); // 화면 새로 고침
	count++;
	if (count < 220)
	{
		glutTimerFunc(10, TimerFunction, 1);
	}
}