#include "GLmain.h"

int donecount = 0;

// ---- 유틸리티 ----
static int getVertexCount(int type)
{
    return (type == 1) ? 2 : (type == 2) ? 3 : (type == 3) ? 4 : 5;
}

static int getLogicalVertexCount(int idx)
{
    if (shape[idx].isPoint) return 1; // 점은 그려질 때 삼각형이더라도 논리적으로 1로 계산
    return getVertexCount(shape[idx].shapetype);
}

static void getCenter(int idx, float& cx, float& cy)
{
    int vc = getVertexCount(shape[idx].shapetype);
    cx = 0.0f; cy = 0.0f;
    for (int v = 0; v < vc; ++v) { cx += shape[idx].shapecoord[v][0]; cy += shape[idx].shapecoord[v][1]; }
    cx /= vc; cy /= vc;
}

static void getBounds(int idx, float& minx, float& miny, float& maxx, float& maxy)
{
    int vc = getVertexCount(shape[idx].shapetype);
    minx =  2.0f; miny =  2.0f; maxx = -2.0f; maxy = -2.0f;
    for (int v = 0; v < vc; ++v) {
        float x = shape[idx].shapecoord[v][0];
        float y = shape[idx].shapecoord[v][1];
        if (x < minx) minx = x; if (x > maxx) maxx = x;
        if (y < miny) miny = y; if (y > maxy) maxy = y;
    }
}

static bool boundsOverlap(int a, int b)
{
    float aminx, aminy, amaxx, amaxy, bminx, bminy, bmaxx, bmaxy;
    getBounds(a, aminx, aminy, amaxx, amaxy);
    getBounds(b, bminx, bminy, bmaxx, bmaxy);
    return !(amaxx < bminx || bmaxx < aminx || amaxy < bminy || bmaxy < aminy);
}

static void toPoint(int idx)
{
    float cx, cy; getCenter(idx, cx, cy);
    float d = 0.06f;
    // 점을 작은 삼각형으로 표현 (타입 2)
    shape[idx].shapecoord[0][0] = cx;        shape[idx].shapecoord[0][1] = cy + d; shape[idx].shapecoord[0][2] = 0.0f;
    shape[idx].shapecoord[1][0] = cx - d;    shape[idx].shapecoord[1][1] = cy - d; shape[idx].shapecoord[1][2] = 0.0f;
    shape[idx].shapecoord[2][0] = cx + d;    shape[idx].shapecoord[2][1] = cy - d; shape[idx].shapecoord[2][2] = 0.0f;
    shape[idx].shapetype = 2;
    shape[idx].isPoint = true;
    shape[idx].isAnimated = true;
}

static void combineShapes(int a, int b)
{
    int av = getLogicalVertexCount(a);
    int bv = getLogicalVertexCount(b);
    int nv = av + bv;
    if (nv > 5) {
        toPoint(a);
    } else {
        // 간단하게: a 뒤에 b를 이어붙임. 점은 1개 좌표만 추가
        if (shape[b].isPoint) {
            if (av < 5) {
                shape[a].shapecoord[av][0] = shape[b].shapecoord[0][0];
                shape[a].shapecoord[av][1] = shape[b].shapecoord[0][1];
                shape[a].shapecoord[av][2] = 0.0f;
            }
        } else {
            int bgeom = getVertexCount(shape[b].shapetype);
            for (int i = 0; i < bgeom && av + i < 5; ++i) {
                shape[a].shapecoord[av + i][0] = shape[b].shapecoord[i][0];
                shape[a].shapecoord[av + i][1] = shape[b].shapecoord[i][1];
                shape[a].shapecoord[av + i][2] = 0.0f;
            }
        }
        shape[a].shapetype = (nv == 2) ? 1 : (nv == 3) ? 2 : (nv == 4) ? 3 : 4;
        shape[a].isPoint = false;
        shape[a].isAnimated = true;
    }
}

static void removeShape(int idx)
{
    if (idx < shapeCount - 1) {
        shape[idx] = shape[shapeCount - 1];
    }
    shapeCount = (shapeCount > 0) ? shapeCount - 1 : 0;
}

// 도형 랜덤 초기화: 각 타입 최소 3개씩
void ResetShapes()
{
    shapeCount = 0;
    std::random_device rd; std::mt19937 gen(rd());
    std::uniform_real_distribution<float> rx(-0.9f, 0.9f);
    std::uniform_real_distribution<float> ry(-0.9f, 0.9f);

    auto addShape = [&](int type, float cx, float cy)
    {
        if (shapeCount >= MAX_SHAPES) return;
        Shape& s = shape[shapeCount++];
        s.shapetype = type;
        s.isPoint = (type == 1);
        s.isAnimated = false;
        s.vx = 0.01f; s.vy = 0.015f;
        // 간단한 크기
        float d = 0.08f;
        if (type == 1) {
            // 점: 작은 삼각형으로 표현 (다른 도형보다 약간 작게)
            float dp = 0.06f;
            s.shapecoord[0][0] = cx;      s.shapecoord[0][1] = cy + dp; s.shapecoord[0][2] = 0.0f;
            s.shapecoord[1][0] = cx - dp; s.shapecoord[1][1] = cy - dp; s.shapecoord[1][2] = 0.0f;
            s.shapecoord[2][0] = cx + dp; s.shapecoord[2][1] = cy - dp; s.shapecoord[2][2] = 0.0f;
        } else if (type == 2) {
            s.shapecoord[0][0] = cx;      s.shapecoord[0][1] = cy+d;   s.shapecoord[0][2] = 0.0f;
            s.shapecoord[1][0] = cx-d;    s.shapecoord[1][1] = cy-d;   s.shapecoord[1][2] = 0.0f;
            s.shapecoord[2][0] = cx+d;    s.shapecoord[2][1] = cy-d;   s.shapecoord[2][2] = 0.0f;
        } else if (type == 3) {
            s.shapecoord[0][0] = cx-d;    s.shapecoord[0][1] = cy+d;   s.shapecoord[0][2] = 0.0f;
            s.shapecoord[1][0] = cx+d;    s.shapecoord[1][1] = cy+d;   s.shapecoord[1][2] = 0.0f;
            s.shapecoord[2][0] = cx-d;    s.shapecoord[2][1] = cy-d;   s.shapecoord[2][2] = 0.0f;
            s.shapecoord[3][0] = cx+d;    s.shapecoord[3][1] = cy-d;   s.shapecoord[3][2] = 0.0f;
        } else if (type == 4) {
            // 오각형 대충 배치
            s.shapecoord[0][0] = cx;      s.shapecoord[0][1] = cy+d;   s.shapecoord[0][2] = 0.0f;
            s.shapecoord[1][0] = cx-d;    s.shapecoord[1][1] = cy+d*0.3f; s.shapecoord[1][2] = 0.0f;
            s.shapecoord[2][0] = cx-d*0.6f; s.shapecoord[2][1] = cy-d;   s.shapecoord[2][2] = 0.0f;
            s.shapecoord[3][0] = cx+d*0.6f; s.shapecoord[3][1] = cy-d;   s.shapecoord[3][2] = 0.0f;
            s.shapecoord[4][0] = cx+d;    s.shapecoord[4][1] = cy+d*0.3f; s.shapecoord[4][2] = 0.0f;
        }
        s.Colors();
    };

    // 각 타입 최소 3개씩
    for (int k = 0; k < 3; ++k) addShape(1, rx(gen), ry(gen));
    for (int k = 0; k < 3; ++k) addShape(2, rx(gen), ry(gen));
    for (int k = 0; k < 3; ++k) addShape(3, rx(gen), ry(gen));
    for (int k = 0; k < 3; ++k) addShape(4, rx(gen), ry(gen));
}

GLvoid drawScene()
{
	glClearColor(bGCr, bGCg, bGCb, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramID);
	glBindVertexArray(vao);
	UpdateBuffer();

    {
        for (int i = 0; i < shapeCount; i++)
		{
			if (shape[i].shapetype == 1)
				glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, (void*)(i * 5 * sizeof(GLuint)));
			else if (shape[i].shapetype == 2)
				glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(i * 5 * sizeof(GLuint)));
			else if (shape[i].shapetype == 3)
				glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, (void*)(i * 5 * sizeof(GLuint)));
			else if (shape[i].shapetype == 4 || shape[i].shapetype == 5)
				glDrawElements(GL_TRIANGLE_FAN, 5, GL_UNSIGNED_INT, (void*)(i * 5 * sizeof(GLuint)));
		}
	}

	glutSwapBuffers();
}



GLvoid Keyboard(unsigned char key, int x, int y)
{
	donecount = 0;
		switch (key)
		{
	case 'c':
		ResetShapes();
			break;
	case 's':
		animating = !animating;
			break;
		case 'q':
			glutLeaveMainLoop();
			break;
	}
	glutPostRedisplay();
	glutTimerFunc(10, TimerFunction, 1);
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
	glBufferData(GL_ARRAY_BUFFER, MAX_SHAPES * 15 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
	//--- 좌표값을 attribute 인덱스 0번에 명시한다: 버텍스 당 3* float
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//--- attribute 인덱스 0번을 사용가능하게 함
	glEnableVertexAttribArray(0);

	//--- 2번째 VBO를 활성화 하여 바인드 하고, 버텍스 속성 (색상)을 저장
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	//--- 변수 colors에서 버텍스 색상을 복사한다.
	//--- colors 배열의 사이즈: 9 *float
	glBufferData(GL_ARRAY_BUFFER, MAX_SHAPES * 15 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
	//--- 색상값을 attribute 인덱스 1번에 명시한다: 버텍스 당 3*float
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//--- attribute 인덱스 1번을 사용 가능하게 함.
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	GLuint indices[MAX_SHAPES * 5];
	for (int i = 0; i < MAX_SHAPES; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			indices[i * 5 + j] = i * 5 + j;
		}
	}
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);
}

void UpdateBuffer()
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    for (int i = 0; i < shapeCount; i++)
	{
		glBufferSubData(GL_ARRAY_BUFFER, i * 15 * sizeof(GLfloat), 15 * sizeof(GLfloat), shape[i].shapecoord);
	}
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	for (int i = 0; i < shapeCount; i++)
	{
		glBufferSubData(GL_ARRAY_BUFFER, i * 15 * sizeof(GLfloat), 15 * sizeof(GLfloat), shape[i].color);
	}
}

GLvoid TimerFunction(int value)
{
    // 합쳐진 도형(또는 점)만 일정 속도로 이동 + 바운딩 박스 벽 반사
    if (animating) {
        for (int i = 0; i < shapeCount; ++i) {
            if (!(shape[i].isPoint || shape[i].isAnimated)) continue;
            float minx, miny, maxx, maxy; getBounds(i, minx, miny, maxx, maxy);
            if (minx <= -1.0f || maxx >= 1.0f) shape[i].vx = -shape[i].vx;
            if (miny <= -1.0f || maxy >= 1.0f) shape[i].vy = -shape[i].vy;
            int vc = getLogicalVertexCount(i) == 1 ? 3 : getVertexCount(shape[i].shapetype);
            for (int v = 0; v < vc; ++v) {
                shape[i].shapecoord[v][0] += shape[i].vx;
                shape[i].shapecoord[v][1] += shape[i].vy;
            }
        }
    }
	glutPostRedisplay();
	glutTimerFunc(10, TimerFunction, 1);
}

// ---- 마우스 선택/드래그/합치기 ----
GLvoid Mouse(int button, int state, int x, int y)
{
    float fx, fy; convertXY(x, y, fx, fy);
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        selectedIndex = -1;
        // 간단: 바운딩 박스에 포함되는 첫 도형 선택
        for (int i = 0; i < shapeCount; ++i) {
            float minx, miny, maxx, maxy; getBounds(i, minx, miny, maxx, maxy);
            if (fx >= minx && fx <= maxx && fy >= miny && fy <= maxy) {
                selectedIndex = i;
                lastMouseX = fx; lastMouseY = fy;
                break;
            }
        }
    }
    else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
    {
        if (selectedIndex != -1) {
            // 다른 도형과 겹치면 합치기
            for (int j = 0; j < shapeCount; ++j) {
                if (j == selectedIndex) continue;
                if (boundsOverlap(selectedIndex, j)) {
                    combineShapes(selectedIndex, j);
                    removeShape(j);
                    break;
                }
            }
        }
        selectedIndex = -1;
    }
}

GLvoid Motion(int x, int y)
{
    if (selectedIndex == -1) return;
    float fx, fy; convertXY(x, y, fx, fy);
    float dx = fx - lastMouseX;
    float dy = fy - lastMouseY;
    int vc = getVertexCount(shape[selectedIndex].shapetype);
    for (int v = 0; v < vc; ++v) {
        shape[selectedIndex].shapecoord[v][0] += dx;
        shape[selectedIndex].shapecoord[v][1] += dy;
    }
    lastMouseX = fx; lastMouseY = fy;
	glutPostRedisplay();
}