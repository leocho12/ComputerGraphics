// Practice17_full.cpp
// g++ Practice17_full.cpp -lglut -lGLEW -lGL -std=c++17
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <ctime>
#include <random>

using glm::vec3;
using glm::mat4;

struct Vertex {
    vec3 pos;
    vec3 color;
};

GLuint program = 0, vao = 0, vbo = 0;
int windowWidth = 800, windowHeight = 600;

// ===== 공통 상태 =====
enum ObjType { OBJ_CUBE, OBJ_PYRAMID };
ObjType currentObj = OBJ_CUBE;

float size = 0.7f;
float basePitchDeg = 0.0f;
float baseYawDeg = 0.0f;

bool depthOn = true;
bool wireframeOn = false;   // 실습17에 없지만 디버그용(원하면 지워도 됨)

// y축 자전(현재 객체)
bool spinY = false;
float spinYSpeed = 60.0f; // deg/sec
float spinYAngle = 0.0f;

// 타이머
int lastTimeMs = 0;

// ===== 육면체 애니메이션 상태 =====
bool animCubeT = false;   // 전체 y축 제자리 회전
bool animCubeF = false;   // 앞면 열기
bool animCubeS = false;   // 오른쪽면 제자리 회전
bool animCubeB = false;   // 뒷면 스케일

float cubeTAngle = 0.0f;        // t
float cubeFrontAngle = 0.0f;    // f (0~90)
bool  cubeFrontOpening = true;

float cubeSideAngle = 0.0f;     // s (회전 계속)
float cubeBackScale = 1.0f;     // b (1->0->1)
bool  cubeBackShrinking = true;

// ===== 사각뿔 애니메이션 상태 =====
bool animPyrO = false;    // 전체 동시 열기(0~180)
float pyrAllAngle = 0.0f;
bool  pyrAllOpening = true;

bool animPyrR = false;    // 순차 열기(0~90)
int   pyrSeqIndex = 0;    // 0~3
float pyrSeqAngle = 0.0f;
bool  pyrSeqOpening = true;

// 랜덤(안 써도 됨)
std::mt19937 rng((unsigned)time(nullptr));

// ===== 지오메트리 =====
vec3 C[8]; // cube corners
vec3 P[5]; // pyramid vertices

// cube faces by corner indices (CCW)
int cubeFaces[6][4] = {
    {4,5,6,7}, // +Z front
    {0,1,2,3}, // -Z back
    {1,5,6,2}, // +X right
    {0,4,7,3}, // -X left
    {3,2,6,7}, // +Y top
    {0,1,5,4}  // -Y bottom
};

// pyramid side triangles indices (CCW)
int pyrSides[4][3] = {
    {4,0,1}, // front
    {4,1,2}, // right
    {4,2,3}, // back
    {4,3,0}  // left
};
int pyrBase[4] = { 0,1,2,3 };

// face colors (실습17: 면마다 다른색)
vec3 cubeFaceColor[6] = {
    {0.0f,1.0f,1.0f}, // front cyan
    {1.0f,0.0f,1.0f}, // back magenta
    {0.0f,1.0f,0.0f}, // right green
    {0.0f,0.0f,1.0f}, // left blue
    {1.0f,1.0f,0.0f}, // top yellow
    {1.0f,0.0f,0.0f}  // bottom red
};
vec3 pyrFaceColor[4] = {
    {1.0f,0.0f,0.0f}, // front red
    {0.0f,0.0f,1.0f}, // right blue
    {0.0f,1.0f,0.0f}, // back green
    {1.0f,1.0f,0.0f}  // left yellow
};
vec3 pyrBaseColor = { 1.0f,0.0f,1.0f }; // base magenta

// ===== 셰이더 로딩 =====
std::string loadTextFile(const char* path) {
    std::ifstream file(path);
    if (!file) { std::cerr << "Failed to open " << path << "\n"; return ""; }
    std::stringstream ss; ss << file.rdbuf(); return ss.str();
}

GLuint compileShaderFromFile(const char* path, GLenum type) {
    std::string srcStr = loadTextFile(path);
    const char* src = srcStr.c_str();
    GLuint sh = glCreateShader(type);
    glShaderSource(sh, 1, &src, nullptr);
    glCompileShader(sh);

    GLint ok; glGetShaderiv(sh, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[1024]; glGetShaderInfoLog(sh, 1024, nullptr, log);
        std::cerr << "Shader compile error " << path << ":\n" << log << "\n";
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
    GLint ok; glGetProgramiv(program, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[1024]; glGetProgramInfoLog(program, 1024, nullptr, log);
        std::cerr << "Program link error:\n" << log << "\n";
    }
    glDeleteShader(vs); glDeleteShader(fs);
}

// ===== 기본 정점 생성 =====
void buildVertices() {
    float s = size;
    C[0] = { -s,-s,-s }; C[1] = { s,-s,-s }; C[2] = { s, s,-s }; C[3] = { -s, s,-s };
    C[4] = { -s,-s, s }; C[5] = { s,-s, s }; C[6] = { s, s, s }; C[7] = { -s, s, s };

    float h = size * 1.2f;
    P[0] = { -s,-s,-s }; P[1] = { s,-s,-s }; P[2] = { s,-s, s }; P[3] = { -s,-s, s };
    P[4] = { 0, h, 0 };
}

// ===== 렌더 헬퍼 =====
void uploadAndDraw(const std::vector<Vertex>& vtx, GLenum mode) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vtx.size() * sizeof(Vertex), vtx.data(), GL_DYNAMIC_DRAW);
    glDrawArrays(mode, 0, (GLsizei)vtx.size());
}

std::vector<vec3> applyMat(const std::vector<vec3>& in, const mat4& T) {
    std::vector<vec3> out; out.reserve(in.size());
    for (auto& p : in) {
        glm::vec4 q = T * glm::vec4(p, 1.0f);
        out.push_back(vec3(q));
    }
    return out;
}

mat4 rotateAroundEdge(const vec3& v0, const vec3& v1, float angleDeg) {
    vec3 axis = glm::normalize(v1 - v0);
    mat4 T(1.0f);
    T = glm::translate(T, v0);
    T = glm::rotate(T, glm::radians(angleDeg), axis);
    T = glm::translate(T, -v0);
    return T;
}

// ===== 축 =====
void drawAxes(const mat4& VP) {
    GLint locMVP = glGetUniformLocation(program, "uMVP");
    glUniformMatrix4fv(locMVP, 1, GL_FALSE, &VP[0][0]);

    std::vector<Vertex> axes = {
        {{0,0,0},{1,0,0}}, {{1,0,0},{1,0,0}},
        {{0,0,0},{0,1,0}}, {{0,1,0},{0,1,0}},
        {{0,0,0},{0,0,1}}, {{0,0,1},{0,0,1}}
    };
    uploadAndDraw(axes, GL_LINES);
}

// ===== 육면체 그리기(면 단위 애니메이션) =====
void drawCubeAnimated() {
    for (int f = 0; f < 6; ++f) {
        // face vertices (object space)
        std::vector<vec3> fv;
        for (int k = 0; k < 4; k++) {
            fv.push_back(C[cubeFaces[f][k]]);
        }

        mat4 local(1.0f);

        // t: 전체 회전은 모델 매트릭스에서 처리(cubeTAngle)
        // f: 앞면(+Z) 열기 (힌지: y=+s, z=+s, x축 방향)
        if (f == 0 && animCubeF) {
            vec3 pivot(0, size, size); // top-front edge center
            local = glm::translate(local, pivot);
            local = glm::rotate(local, glm::radians(cubeFrontAngle), vec3(1, 0, 0));
            local = glm::translate(local, -pivot);
        }

        // s: 오른쪽면(+X) 자체 중심/법선축(+X) 제자리 회전
        if (f == 2 && animCubeS) {
            vec3 center(size, 0, 0); // right face center
            local = glm::translate(local, center);
            local = glm::rotate(local, glm::radians(cubeSideAngle), vec3(1, 0, 0));
            local = glm::translate(local, -center);
        }

        // b: 뒷면(-Z) 중심 기준 스케일
        if (f == 1 && animCubeB) {
            vec3 center(0, 0, -size);
            local = glm::translate(local, center);
            local = glm::scale(local, vec3(cubeBackScale));
            local = glm::translate(local, -center);
        }

        auto tfv = applyMat(fv, local);

        std::vector<Vertex> vtx;
        for (auto& p : tfv) {
            vtx.push_back({ p, cubeFaceColor[f] });
        }
        uploadAndDraw(vtx, GL_TRIANGLE_FAN);
    }
}

// ===== 사각뿔 그리기(면 단위 애니메이션) =====
void drawPyramidAnimated() {
    // 4 side faces
    for (int s = 0; s < 4; ++s) {
        std::vector<vec3> sv;
        for (int k = 0; k < 3; k++) {
            sv.push_back(P[pyrSides[s][k]]);
        }

        mat4 local(1.0f);

        // o: 전체 동시 열기
        if (animPyrO) {
            vec3 v0 = P[pyrSides[s][1]];
            vec3 v1 = P[pyrSides[s][2]];
            local = rotateAroundEdge(v0, v1, -pyrAllAngle);  // ← 여기 - 추가
        }

        // r: 순차 열기
        if (animPyrR && s == pyrSeqIndex) {
            vec3 v0 = P[pyrSides[s][1]];
            vec3 v1 = P[pyrSides[s][2]];
            local = rotateAroundEdge(v0, v1, -pyrSeqAngle);  // ← 여기 - 추가
        }


        auto tsv = applyMat(sv, local);
        std::vector<Vertex> vtx;
        for (auto& p : tsv) {
            vtx.push_back({ p, pyrFaceColor[s] });
        }
        uploadAndDraw(vtx, GL_TRIANGLES);
    }

    // base (static)
    std::vector<Vertex> baseV;
    for (int k = 0; k < 4; k++) {
        baseV.push_back({ P[pyrBase[k]], pyrBaseColor });
    }
    uploadAndDraw(baseV, GL_TRIANGLE_FAN);
}

// ===== display =====
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(program);
    glBindVertexArray(vao);

    if (depthOn) glEnable(GL_DEPTH_TEST);
    else        glDisable(GL_DEPTH_TEST);

    glPolygonMode(GL_FRONT_AND_BACK, wireframeOn ? GL_LINE : GL_FILL);

    mat4 Pj = glm::perspective(glm::radians(45.0f),
        (float)windowWidth / windowHeight,
        0.1f, 100.0f);
    mat4 V = glm::lookAt(vec3(2, 2, 2), vec3(0, 0, 0), vec3(0, 1, 0));
    mat4 VP = Pj * V;

    // axes fixed
    drawAxes(VP);

    // model (기본 기울기 + 자전 + t회전)
    mat4 M(1.0f);
    M = glm::rotate(M, glm::radians(basePitchDeg), vec3(1, 0, 0));

    float yawTotal = baseYawDeg + spinYAngle + (animCubeT ? cubeTAngle : 0.0f);
    M = glm::rotate(M, glm::radians(yawTotal), vec3(0, 1, 0));

    mat4 MVP = VP * M;
    GLint locMVP = glGetUniformLocation(program, "uMVP");
    glUniformMatrix4fv(locMVP, 1, GL_FALSE, &MVP[0][0]);

    if (currentObj == OBJ_CUBE) drawCubeAnimated();
    else                       drawPyramidAnimated();

    glutSwapBuffers();
}

// ===== idle (애니메이션 업데이트) =====
void idle() {
    int now = glutGet(GLUT_ELAPSED_TIME);
    float dt = (now - lastTimeMs) / 1000.0f;
    lastTimeMs = now;

    if (spinY) {
        spinYAngle += spinYSpeed * dt;
        if (spinYAngle > 360.0f) spinYAngle -= 360.0f;
    }

    // cube t
    if (animCubeT) {
        cubeTAngle += 90.0f * dt;
        if (cubeTAngle > 360.0f) cubeTAngle -= 360.0f;
    }

    // cube f open/close (0~90)
    if (animCubeF) {
        float v = 60.0f * dt;
        if (cubeFrontOpening) cubeFrontAngle += v;
        else                 cubeFrontAngle -= v;

        if (cubeFrontAngle >= 90.0f) { cubeFrontAngle = 90.0f; cubeFrontOpening = false; }
        if (cubeFrontAngle <= 0.0f) { cubeFrontAngle = 0.0f;  cubeFrontOpening = true; }
    }

    // cube s spin right face
    if (animCubeS) {
        cubeSideAngle += 180.0f * dt;
        if (cubeSideAngle > 360.0f) cubeSideAngle -= 360.0f;
    }

    // cube b scale back face (1->0->1)
    if (animCubeB) {
        float v = 1.2f * dt;
        if (cubeBackShrinking) cubeBackScale -= v;
        else                  cubeBackScale += v;

        if (cubeBackScale <= 0.0f) { cubeBackScale = 0.0f; cubeBackShrinking = false; }
        if (cubeBackScale >= 1.0f) { cubeBackScale = 1.0f; cubeBackShrinking = true; }
    }

    // pyramid o all open (0~180)
    if (animPyrO) {
        float v = 60.0f * dt;
        if (pyrAllOpening) pyrAllAngle += v;
        else              pyrAllAngle -= v;

        if (pyrAllAngle >= 180.0f) { pyrAllAngle = 180.0f; pyrAllOpening = false; }
        if (pyrAllAngle <= 0.0f) { pyrAllAngle = 0.0f;   pyrAllOpening = true; }
    }

    // pyramid r sequential (0~90)
    if (animPyrR) {
        float v = 60.0f * dt;
        if (pyrSeqOpening) pyrSeqAngle += v;
        else              pyrSeqAngle -= v;

        if (pyrSeqAngle >= 90.0f) {
            pyrSeqAngle = 90.0f;
            pyrSeqOpening = false;
        }
        if (pyrSeqAngle <= 0.0f) {
            pyrSeqAngle = 0.0f;
            pyrSeqOpening = true;
            pyrSeqIndex = (pyrSeqIndex + 1) % 4; // 다음 면으로
        }
    }

    glutPostRedisplay();
}

// ===== reset =====
void resetAll() {
    currentObj = OBJ_CUBE;
    depthOn = true;
    wireframeOn = false;

    spinY = false;
    spinYAngle = 0.0f;

    animCubeT = animCubeF = animCubeS = animCubeB = false;
    cubeTAngle = 0.0f;
    cubeFrontAngle = 0.0f; cubeFrontOpening = true;
    cubeSideAngle = 0.0f;
    cubeBackScale = 1.0f; cubeBackShrinking = true;

    animPyrO = animPyrR = false;
    pyrAllAngle = 0.0f; pyrAllOpening = true;
    pyrSeqIndex = 0; pyrSeqAngle = 0.0f; pyrSeqOpening = true;
}

// ===== keyboard =====
void keyboard(unsigned char key, int, int) {
    switch (key) {
    case 'h':
        depthOn = !depthOn;
        break;

    case 'p':
        currentObj = (currentObj == OBJ_CUBE) ? OBJ_PYRAMID : OBJ_CUBE;
        break;

    case 'y':
        spinY = !spinY;
        break;

    case 'c':
        resetAll();
        break;

        // cube animations
    case 't':
        if (currentObj == OBJ_CUBE) animCubeT = !animCubeT;
        break;
    case 'f':
        if (currentObj == OBJ_CUBE) animCubeF = !animCubeF;
        break;
    case 's':
        if (currentObj == OBJ_CUBE) animCubeS = !animCubeS;
        break;
    case 'b':
        if (currentObj == OBJ_CUBE) animCubeB = !animCubeB;
        break;

        // pyramid animations
    case 'o':
        if (currentObj == OBJ_PYRAMID) {
            animPyrO = !animPyrO;
            animPyrR = false; // 충돌 방지
        }
        break;
    case 'r':
        if (currentObj == OBJ_PYRAMID) {
            animPyrR = !animPyrR;
            animPyrO = false;
        }
        break;

    case 'q':
    case 27:
        exit(0);
    }
    glutPostRedisplay();
}

void reshape(int w, int h) {
    windowWidth = w; windowHeight = h;
    glViewport(0, 0, w, h);
}

// ===== init =====
void initGL() {
    glewInit();
    glEnable(GL_DEPTH_TEST);

    createProgram();
    buildVertices();

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);

    glClearColor(0, 0, 0, 1);

    resetAll();
    lastTimeMs = glutGet(GLUT_ELAPSED_TIME);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Practice 17");

    initGL();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(idle);

    glutMainLoop();
    return 0;
}
