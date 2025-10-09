#include <gl/glew.h>
#include <gl/freeglut.h>
#include <math.h>
#include <stdlib.h>

// 단일 파일: 4개 삼각형 회전 애니메이션(c/t/s/q)

static const int WIDTH = 800;
static const int HEIGHT = 600;

static GLuint gProgram = 0;
static GLuint gVAO = 0;
static GLuint gVBO = 0;

static float gAngleDeg = 0.0f; // 누적 각도
static int gDir = 1;           // 1: 반시계, -1: 시계
static bool gRunning = true;   // s로 토글
static int gShapeMode = 1;     // 1: 도형1, 2: 도형2

static const float a = 0.20f;  // 중앙 정사각형 반길이
static const float b = 0.45f;  // 바깥 다이아몬드 꼭짓점 거리

// 원본(미회전) 좌표 4개 삼각형(총 12개 정점)
static GLfloat gBase[12 * 3];
static GLfloat gRotated[12 * 3];

static inline float toRad(float d) { return d * (3.14159265359f / 180.0f); }

static GLuint makeShader(GLenum type, const char* src)
{
	GLuint s = glCreateShader(type);
	glShaderSource(s, 1, &src, nullptr);
	glCompileShader(s);
	return s;
}

static void makeProgram()
{
	const char* vsrc =
		"#version 330 core\n"
		"layout(location=0) in vec3 aPos;\n"
		"void main(){ gl_Position = vec4(aPos,1.0); }\n";
	const char* fsrc =
		"#version 330 core\n"
		"out vec4 FragColor;\n"
		"void main(){ FragColor = vec4(1.0, 0.78, 0.0, 1.0); }\n";
	GLuint vs = makeShader(GL_VERTEX_SHADER, vsrc);
	GLuint fs = makeShader(GL_FRAGMENT_SHADER, fsrc);
	gProgram = glCreateProgram();
	glAttachShader(gProgram, vs);
	glAttachShader(gProgram, fs);
	glLinkProgram(gProgram);
	glDeleteShader(vs);
	glDeleteShader(fs);
}

// 모드에 따른 기본 정점 설정
static void setShapeMode(int mode)
{
    gShapeMode = mode;
    if (mode == 1)
    {
        // 도형 1: 다이아몬드 외곽 + 중앙 정사각형 빈공간
        GLfloat tmp[12 * 3] = {
            // 위
            -a,  a, 0.0f,   a,  a, 0.0f,   0,  b, 0.0f,
            // 오른쪽
             a,  a, 0.0f,   a, -a, 0.0f,   b,  0, 0.0f,
            // 아래
             a, -a, 0.0f,  -a, -a, 0.0f,   0, -b, 0.0f,
            // 왼쪽
            -a, -a, 0.0f,  -a,  a, 0.0f,  -b,  0, 0.0f,
        };
        memcpy(gBase, tmp, sizeof(gBase));
    }
    else
    {
        // 도형 2: 정사각형 안쪽을 4개의 삼각형으로(중앙에 흰색 X 틈을 주기 위해 약간 오프셋)
        const float gap = 0.03f; // X 두께
        GLfloat tmp[12 * 3] = {
            // 위 삼각형 (상단 변과 중앙 위쪽 점 두 개)
            -b,  b, 0.0f,   b,  b, 0.0f,   0,  gap, 0.0f,
            // 오른쪽 삼각형
             b,  b, 0.0f,   b, -b, 0.0f,   gap, 0, 0.0f,
            // 아래 삼각형
             b, -b, 0.0f,  -b, -b, 0.0f,   0, -gap, 0.0f,
            // 왼쪽 삼각형
            -b, -b, 0.0f,  -b,  b, 0.0f,  -gap, 0, 0.0f,
        };
        memcpy(gBase, tmp, sizeof(gBase));
    }
}

static void updateBuffer()
{
	float r = toRad(gAngleDeg);
	float c = cosf(r);
	float s = sinf(r);
	for (int i = 0; i < 12; ++i) {
		float x = gBase[i * 3 + 0];
		float y = gBase[i * 3 + 1];
		gRotated[i * 3 + 0] = c * x - s * y;
		gRotated[i * 3 + 1] = s * x + c * y;
		gRotated[i * 3 + 2] = 0.0f;
	}
	glBindBuffer(GL_ARRAY_BUFFER, gVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(gRotated), gRotated);
}

static void initBuffer()
{
	glGenVertexArrays(1, &gVAO);
	glBindVertexArray(gVAO);

	glGenBuffers(1, &gVBO);
	glBindBuffer(GL_ARRAY_BUFFER, gVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(gRotated), nullptr, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	setShapeMode(1);
	updateBuffer();
}

static void draw()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(gProgram);
	glBindVertexArray(gVAO);
	glDrawArrays(GL_TRIANGLES, 0, 12);
	glutSwapBuffers();
}

static void onTimer(int)
{
	if (gRunning) {
		gAngleDeg += gDir * 1.2f;
		if (gAngleDeg >= 360.0f) gAngleDeg -= 360.0f;
		if (gAngleDeg < 0.0f)    gAngleDeg += 360.0f;
		updateBuffer();
	}
	glutPostRedisplay();
	glutTimerFunc(16, onTimer, 0);
}

static void onKey(unsigned char key, int, int)
{
	switch (key) {
	case 'c': case 'C': gDir = -1; break; // 시계
	case 't': case 'T': gDir =  1; break; // 반시계
	case 's': case 'S': gRunning = !gRunning; break; // 토글
	case 'q': case 'Q': exit(0); break; // 종료
	case '1': setShapeMode(1); updateBuffer(); break; // 도형1
	case '2': setShapeMode(2); updateBuffer(); break; // 도형2
	default: break;
	}
}

static void onReshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("Drill14");

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) return -1;

	makeProgram();
	initBuffer();

	glutDisplayFunc(draw);
	glutKeyboardFunc(onKey);
	glutReshapeFunc(onReshape);
	glutTimerFunc(16, onTimer, 0);
	glutMainLoop();
	return 0;
}