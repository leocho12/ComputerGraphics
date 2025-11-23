#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <random>
#include <ctime>
#include <fstream>
#include <sstream>
#include <iostream>

using glm::vec3;
using glm::mat4;

struct Vertex {
    vec3 pos;
    vec3 color;
};

GLuint program = 0;
GLuint vao = 0, vbo = 0;

int windowWidth = 800, windowHeight = 600;

// ====== 상태 ======
enum ObjType { OBJ_CUBE, OBJ_PYRAMID };
ObjType currentObj = OBJ_CUBE;

float size = 0.7f;

// 기본 기울기(실습 조건)
float basePitchDeg = 0.0f;   // x축
float baseYawDeg = 0.0f;  // y축

// 자전 애니메이션
bool spinXPos = false, spinXNeg = false;
bool spinYPos = false, spinYNeg = false;
float spinSpeedDeg = 60.0f;   // 초당 60도

// 현재 추가 회전
float addPitchDeg = 0.0f;
float addYawDeg = 0.0f;

// 이동 (x,z 평면)
float moveX = 0.0f;
float moveZ = 0.0f;
float moveStep = 0.1f;

// 모드
bool depthOn = true;
bool wireframeOn = false;

// 타이밍
int lastTimeMs = 0;

// 랜덤
std::mt19937 rng((unsigned)time(nullptr));

// ====== 도형 정점(버텍스컬러) ======
vec3 C[8]; // cube corners
vec3 P[5]; // pyramid vertices

// ====== 셰이더 유틸 ======
std::string loadTextFile(const char* path) {
    std::ifstream file(path);
    if (!file) {
        std::cerr << "Failed to open shader file: " << path << "\n";
        return "";
    }
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

GLuint compileShaderFromFile(const char* path, GLenum type) {
    std::string srcStr = loadTextFile(path);
    const char* src = srcStr.c_str();

    GLuint sh = glCreateShader(type);
    glShaderSource(sh, 1, &src, nullptr);
    glCompileShader(sh);

    GLint ok;
    glGetShaderiv(sh, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetShaderInfoLog(sh, 1024, nullptr, log);
        std::cerr << "Shader compile error (" << path << "):\n" << log << "\n";
    }
    return sh;
}

void createProgram() {
    GLuint vs = compileShaderFromFile("vertex.glsl", GL_VERTEX_SHADER);
    GLuint fs = compileShaderFromFile("fragment.glsl", GL_FRAGMENT_SHADER);

    program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    GLint ok;
    glGetProgramiv(program, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetProgramInfoLog(program, 1024, nullptr, log);
        std::cerr << "Program link error:\n" << log << "\n";
    }

    glDeleteShader(vs);
    glDeleteShader(fs);
}

// ====== 기하 빌드 ======
void buildVertices() {
    float s = size;

    // cube corners (centered)
    C[0] = { -s,-s,-s }; C[1] = { s,-s,-s }; C[2] = { s, s,-s }; C[3] = { -s, s,-s };
    C[4] = { -s,-s, s }; C[5] = { s,-s, s }; C[6] = { s, s, s }; C[7] = { -s, s, s };

    // pyramid vertices (square base y=-s, apex y=+h)
    float h = size * 1.2f;
    P[0] = { -s,-s,-s }; P[1] = { s,-s,-s }; P[2] = { s,-s, s }; P[3] = { -s,-s, s };
    P[4] = { 0, h, 0 };
}

// ====== 축 ======
void uploadAndDraw(const std::vector<Vertex>& vtx, GLenum mode) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vtx.size() * sizeof(Vertex), vtx.data(), GL_DYNAMIC_DRAW);
    glDrawArrays(mode, 0, (GLsizei)vtx.size());
}

void drawAxes(const mat4& VP) {
    GLint locMVP = glGetUniformLocation(program, "uMVP");
    glUniformMatrix4fv(locMVP, 1, GL_FALSE, &VP[0][0]);

    std::vector<Vertex> axes = {
        {{0,0,0},{1,0,0}}, {{1,0,0},{1,0,0}}, // X red
        {{0,0,0},{0,1,0}}, {{0,1,0},{0,1,0}}, // Y green
        {{0,0,0},{0,0,1}}, {{0,0,1},{0,0,1}}  // Z blue
    };
    uploadAndDraw(axes, GL_LINES);
}

// ====== 정육면체 그리기 (각 정점 색 다름) ======
vec3 cubeColor(int idx) {
    // 8개 정점에 고유색(원하는대로 바꿔도 됨)
    static vec3 col[8] = {
        {1,0,0}, {0,1,0}, {0,0,1}, {1,1,0},
        {1,0,1}, {0,1,1}, {1,1,1}, {0.3f,0.3f,0.3f}
    };
    return col[idx];
}

void drawCube() {
    // 6 faces, each as quad (triangle fan)
    int faces[6][4] = {
        {4,5,6,7}, // +Z
        {0,1,2,3}, // -Z
        {1,5,6,2}, // +X
        {0,4,7,3}, // -X
        {3,2,6,7}, // +Y
        {0,1,5,4}  // -Y
    };

    for (int f = 0; f < 6; ++f) {
        std::vector<Vertex> vtx;
        for (int k = 0; k < 4; k++) {
            int id = faces[f][k];
            vtx.push_back({ C[id], cubeColor(id) });
        }
        uploadAndDraw(vtx, GL_TRIANGLE_FAN);
    }
}

// ====== 사각뿔 그리기 (각 정점 색 다름) ======
vec3 pyrColor(int idx) {
    static vec3 col[5] = {
        {1,0,1}, {1,1,0}, {0,1,1}, {0,0,1},
        {1,0,0} // apex
    };
    return col[idx];
}

void drawPyramid() {
    // 4 sides (triangles)
    int sides[4][3] = {
        {4,0,1},
        {4,1,2},
        {4,2,3},
        {4,3,0}
    };
    for (int s = 0; s < 4; s++) {
        std::vector<Vertex> vtx;
        for (int k = 0; k < 3; k++) {
            int id = sides[s][k];
            vtx.push_back({ P[id], pyrColor(id) });
        }
        uploadAndDraw(vtx, GL_TRIANGLES);
    }

    // base (quad)
    int base[4] = { 0,1,2,3 };
    std::vector<Vertex> baseV;
    for (int k = 0; k < 4; k++) {
        int id = base[k];
        baseV.push_back({ P[id], pyrColor(id) });
    }
    uploadAndDraw(baseV, GL_TRIANGLE_FAN);
}

// ====== display ======
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(program);
    glBindVertexArray(vao);

    if (depthOn) glEnable(GL_DEPTH_TEST);
    else        glDisable(GL_DEPTH_TEST);

    glPolygonMode(GL_FRONT_AND_BACK, wireframeOn ? GL_LINE : GL_FILL);

    // camera
    mat4 Pj = glm::perspective(glm::radians(45.0f),
        (float)windowWidth / windowHeight,
        0.1f, 100.0f);
    mat4 V = glm::lookAt(vec3(2, 2, 2), vec3(0, 0, 0), vec3(0, 1, 0));
    mat4 VP = Pj * V;

    // axes fixed
    drawAxes(VP);

    // model matrix
    mat4 M(1.0f);
    M = glm::translate(M, vec3(moveX, 0.0f, moveZ));
    M = glm::rotate(M, glm::radians(basePitchDeg + addPitchDeg), vec3(1, 0, 0));
    M = glm::rotate(M, glm::radians(baseYawDeg + addYawDeg), vec3(0, 1, 0));

    mat4 MVP = VP * M;
    GLint locMVP = glGetUniformLocation(program, "uMVP");
    glUniformMatrix4fv(locMVP, 1, GL_FALSE, &MVP[0][0]);

    if (currentObj == OBJ_CUBE) drawCube();
    else                       drawPyramid();

    glutSwapBuffers();
}

// ====== 애니메이션 타이머 ======
void idle() {
    int now = glutGet(GLUT_ELAPSED_TIME);
    float dt = (now - lastTimeMs) / 1000.0f;
    lastTimeMs = now;

    float delta = spinSpeedDeg * dt;

    if (spinXPos) addPitchDeg += delta;
    if (spinXNeg) addPitchDeg -= delta;
    if (spinYPos) addYawDeg += delta;
    if (spinYNeg) addYawDeg -= delta;

    glutPostRedisplay();
}

// ====== 키보드 ======
void resetAll() {
    currentObj = OBJ_CUBE;
    depthOn = true;
    wireframeOn = false;

    spinXPos = spinXNeg = spinYPos = spinYNeg = false;
    addPitchDeg = 0.0f;
    addYawDeg = 0.0f;

    moveX = 0.0f; moveZ = 0.0f;
}

void keyboard(unsigned char key, int, int) {
    switch (key) {
    case 'c': currentObj = OBJ_CUBE; break;
    case 'p': currentObj = OBJ_PYRAMID; break;

    case 'h': depthOn = !depthOn; break;

    case 'w':
    case 'W': wireframeOn = !wireframeOn; break;

    case 'x': spinXPos = !spinXPos; if (spinXPos) spinXNeg = false; break;
    case 'X': spinXNeg = !spinXNeg; if (spinXNeg) spinXPos = false; break;

    case 'y': spinYPos = !spinYPos; if (spinYPos) spinYNeg = false; break;
    case 'Y': spinYNeg = !spinYNeg; if (spinYNeg) spinYPos = false; break;

    case 's': resetAll(); break;

    case 27: exit(0);
    }
    glutPostRedisplay();
}

// 방향키 이동
void special(int key, int, int) {
    switch (key) {
    case GLUT_KEY_LEFT:  moveX -= moveStep; break;
    case GLUT_KEY_RIGHT: moveX += moveStep; break;
    case GLUT_KEY_UP:    moveZ -= moveStep; break; // 위=앞(-z)
    case GLUT_KEY_DOWN:  moveZ += moveStep; break;
    }
    glutPostRedisplay();
}

void reshape(int w, int h) {
    windowWidth = w; windowHeight = h;
    glViewport(0, 0, w, h);
}

// ====== init ======
void initGL() {
    glewInit();
    glEnable(GL_DEPTH_TEST);

    createProgram();
    buildVertices();

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // position (location 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    // color (location 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);

    glClearColor(0, 0, 0, 1); // 슬라이드처럼 검은 배경
    resetAll();
    lastTimeMs = glutGet(GLUT_ELAPSED_TIME);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Practice 16");

    initGL();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);

    glutMainLoop();
    return 0;
}
