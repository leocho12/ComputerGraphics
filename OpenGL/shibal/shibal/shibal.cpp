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

// ====== 조작/상태 ======
float size = 0.7f;            // [-1,1] 안에 들어오게
float pitchDeg = 0.0f;
float yawDeg = 0.0f;


bool showCube[6] = { false,false,false,false,false,false };
bool showPyrSide[4] = { false,false,false,false };
bool showPyrBase = false;

std::mt19937 rng((unsigned)time(nullptr));

// ====== 도형 정의 ======
struct Face {
    std::vector<vec3> verts;   // position only
    vec3 color;
};

vec3 C[8];            // cube corners
Face cubeFaces[6];

vec3 P[5];            // pyramid 4 base + apex
Face pyrSides[4];
Face pyrBase;

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
void buildGeometry() {
    float s = size;

    // cube 8 corners
    C[0] = { -s,-s,-s }; C[1] = { s,-s,-s }; C[2] = { s, s,-s }; C[3] = { -s, s,-s };
    C[4] = { -s,-s, s }; C[5] = { s,-s, s }; C[6] = { s, s, s }; C[7] = { -s, s, s };

    // cube faces (CCW)
    cubeFaces[0] = { {C[4],C[5],C[6],C[7]}, {0,1,1} }; // +Z
    cubeFaces[1] = { {C[0],C[1],C[2],C[3]}, {1,0,1} }; // -Z
    cubeFaces[2] = { {C[1],C[5],C[6],C[2]}, {0,1,0} }; // +X
    cubeFaces[3] = { {C[0],C[4],C[7],C[3]}, {0,0,1} }; // -X
    cubeFaces[4] = { {C[3],C[2],C[6],C[7]}, {1,1,0} }; // +Y
    cubeFaces[5] = { {C[0],C[1],C[5],C[4]}, {1,0,0} }; // -Y

    // pyramid (square base at y=-s, apex above)
    float h = size * 1.2f;
    P[0] = { -s,-s,-s }; P[1] = { s,-s,-s }; P[2] = { s,-s, s }; P[3] = { -s,-s, s };
    P[4] = { 0, h, 0 };

    // 4 triangular sides (CCW)
    pyrSides[0] = { {P[4],P[0],P[1]}, {1,0,0} };
    pyrSides[1] = { {P[4],P[1],P[2]}, {0,0,1} };
    pyrSides[2] = { {P[4],P[2],P[3]}, {0,1,0} };
    pyrSides[3] = { {P[4],P[3],P[0]}, {1,1,0} };

    // base quad (CCW looking from below; 컬링 안 켰으니 상관은 적음)
    pyrBase = { {P[0],P[1],P[2],P[3]}, {1,0,1} };
}

// ====== 표시 상태 변경 ======
void clearAll() {
    for (int i = 0; i < 6; i++) showCube[i] = false;
    for (int i = 0; i < 4; i++) showPyrSide[i] = false;
    showPyrBase = false;
}

void setOnlyCubeFace(int idx) {
    clearAll();
    showCube[idx] = true;
}

void setOnlyPyrSide(int idx) {
    clearAll();
    showPyrSide[idx] = true;
}

void setRandomTwoCubeFaces() {
    clearAll();
    std::uniform_int_distribution<int> dist(0, 5);
    int a = dist(rng), b = dist(rng);
    while (b == a) b = dist(rng);
    showCube[a] = true; showCube[b] = true;
}

void setPyrBaseAndRandomSide() {
    clearAll();
    std::uniform_int_distribution<int> dist(0, 3);
    int a = dist(rng);
    showPyrBase = true;
    showPyrSide[a] = true;
}

// ====== 렌더 ======
void uploadAndDraw(const std::vector<Vertex>& vtx, GLenum mode) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vtx.size() * sizeof(Vertex), vtx.data(), GL_DYNAMIC_DRAW);
    glDrawArrays(mode, 0, (GLsizei)vtx.size());
}

void drawAxes(const mat4& VP) {
    // 축은 모델 회전 없이 고정으로 보여주고 싶으면 VP만 적용
    GLint locMVP = glGetUniformLocation(program, "uMVP");
    glUniformMatrix4fv(locMVP, 1, GL_FALSE, &VP[0][0]);

    std::vector<Vertex> axes = {
        {{0,0,0},{1,0,0}}, {{1,0,0},{1,0,0}}, // X red
        {{0,0,0},{0,1,0}}, {{0,1,0},{0,1,0}}, // Y green
        {{0,0,0},{0,0,1}}, {{0,0,1},{0,0,1}}  // Z blue
    };
    uploadAndDraw(axes, GL_LINES);
}

void drawFace(const Face& f, GLenum mode) {
    std::vector<Vertex> vtx;
    vtx.reserve(f.verts.size());
    for (auto& p : f.verts) {
        vtx.push_back({ p, f.color });
    }
    uploadAndDraw(vtx, mode);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(program);
    glBindVertexArray(vao);

    // View/Projection
    mat4 Pj = glm::perspective(glm::radians(45.0f),
        (float)windowWidth / windowHeight,
        0.1f, 100.0f);
    mat4 V = glm::lookAt(vec3(2, 2, 2), vec3(0, 0, 0), vec3(0, 1, 0));
    mat4 VP = Pj * V;

    // 축
    drawAxes(VP);

    // 모델(기본 기울기)
    mat4 M(1.0f);
    M = glm::rotate(M, glm::radians(pitchDeg), vec3(1, 0, 0));
    M = glm::rotate(M, glm::radians(yawDeg), vec3(0, 1, 0));

    mat4 MVP = VP * M;
    GLint locMVP = glGetUniformLocation(program, "uMVP");
    glUniformMatrix4fv(locMVP, 1, GL_FALSE, &MVP[0][0]);

    // 큐브 면
    for (int i = 0; i < 6; i++) {
        if (showCube[i]) {
            drawFace(cubeFaces[i], GL_TRIANGLE_FAN);
        }
    }

    // 사각뿔 옆면
    for (int i = 0; i < 4; i++) {
        if (showPyrSide[i]) {
            drawFace(pyrSides[i], GL_TRIANGLES);
        }
    }

    // 사각뿔 바닥
    if (showPyrBase) {
        drawFace(pyrBase, GL_TRIANGLE_FAN);
    }

    glutSwapBuffers();
}

// ====== 입력 ======
void keyboard(unsigned char key, int, int) {
    switch (key) {
    case '1': setOnlyCubeFace(0); break;
    case '2': setOnlyCubeFace(1); break;
    case '3': setOnlyCubeFace(2); break;
    case '4': setOnlyCubeFace(3); break;
    case '5': setOnlyCubeFace(4); break;
    case '6': setOnlyCubeFace(5); break;

    case '7': setOnlyPyrSide(0); break;
    case '8': setOnlyPyrSide(1); break;
    case '9': setOnlyPyrSide(2); break;
    case '0': setOnlyPyrSide(3); break;

    case 'c': setRandomTwoCubeFaces(); break;
    case 't': setPyrBaseAndRandomSide(); break;

        // 각도 조절(옵션)
    case 'q': yawDeg -= 5; break;
    case 'e': yawDeg += 5; break;
    case 'w': pitchDeg += 5; break;
    case 's': pitchDeg -= 5; break;

    case 27: exit(0);
    }
    glutPostRedisplay();
}

void reshape(int w, int h) {
    windowWidth = w; windowHeight = h;
    glViewport(0, 0, w, h);
}

// ====== 초기화 ======
void initGL() {
    glewInit();
    glEnable(GL_DEPTH_TEST);

    createProgram();
    buildGeometry();

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // position (location 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    // color (location 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);

    glClearColor(1, 1, 1, 1);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Practice 15");

    initGL();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutReshapeFunc(reshape);

    glutMainLoop();
    return 0;
}
