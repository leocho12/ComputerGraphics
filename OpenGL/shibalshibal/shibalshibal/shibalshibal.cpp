// Unified_Practice15_18_CubesOnly.cpp
// g++ Unified_Practice15_18_CubesOnly.cpp -lglut -lGLEW -lGL -std=c++17
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
#include <cmath>

using glm::vec3;
using glm::mat4;

struct Vertex { vec3 pos; vec3 color; };

GLuint program = 0, vao = 0, vbo = 0;
int W = 800, H = 600;

// ================================================================
// Shader load
// ================================================================
std::string loadText(const char* path) {
    std::ifstream f(path);
    if (!f) { std::cerr << "Failed to open " << path << "\n"; return ""; }
    std::stringstream ss; ss << f.rdbuf(); return ss.str();
}
GLuint compileFromFile(const char* path, GLenum type) {
    std::string s = loadText(path);
    const char* src = s.c_str();
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
void makeProgram() {
    GLuint vs = compileFromFile("vertex.glsl", GL_VERTEX_SHADER);
    GLuint fs = compileFromFile("fragment.glsl", GL_FRAGMENT_SHADER);
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

// ================================================================
// Common camera / axes / toggles
// ================================================================
float basePitch = 0.0f, baseYaw = 0.0f; // 기본 기울기
bool depthOn = true;
bool wireOn = false;

void uploadAndDraw(const std::vector<Vertex>& vtx, GLenum mode) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vtx.size() * sizeof(Vertex), vtx.data(), GL_DYNAMIC_DRAW);
    glDrawArrays(mode, 0, (GLsizei)vtx.size());
}

void drawAxes(const mat4& VP) {
    GLint loc = glGetUniformLocation(program, "uMVP");
    glUniformMatrix4fv(loc, 1, GL_FALSE, &VP[0][0]);

    std::vector<Vertex> axes = {
        {{0,0,0},{1,0,0}}, {{1,0,0},{1,0,0}}, // x red
        {{0,0,0},{0,1,0}}, {{0,1,0},{0,1,0}}, // y green
        {{0,0,0},{0,0,1}}, {{0,0,1},{0,0,1}}  // z blue
    };
    uploadAndDraw(axes, GL_LINES);
}

mat4 globalTilt() {
    mat4 G(1.0f);
    G = glm::rotate(G, glm::radians(basePitch), vec3(1, 0, 0));
    G = glm::rotate(G, glm::radians(baseYaw), vec3(0, 1, 0));
    return G;
}

// ================================================================
// Geometry: cube
// ================================================================
float size = 0.7f;
vec3 C[8]; // cube corners

int cubeFaces[6][4] = {
    {4,5,6,7}, // +Z front
    {0,1,2,3}, // -Z back
    {1,5,6,2}, // +X right
    {0,4,7,3}, // -X left
    {3,2,6,7}, // +Y top
    {0,1,5,4}  // -Y bottom
};

vec3 cubeFaceCol[6] = {
    {0,1,1},{1,0,1},{0,1,0},{0,0,1},{1,1,0},{1,0,0}
};

std::vector<Vertex> cubeMeshTri;   // for left/right cubes

void buildBaseVerts() {
    float s = size;
    C[0] = { -s,-s,-s }; C[1] = { s,-s,-s }; C[2] = { s, s,-s }; C[3] = { -s, s,-s };
    C[4] = { -s,-s, s }; C[5] = { s,-s, s }; C[6] = { s, s, s }; C[7] = { -s, s, s };
}

void buildCubeMesh() {
    cubeMeshTri.clear();
    for (int f = 0; f < 6; ++f) {
        int* q = cubeFaces[f];
        vec3 col = cubeFaceCol[f];
        // two triangles
        cubeMeshTri.push_back({ C[q[0]],col });
        cubeMeshTri.push_back({ C[q[1]],col });
        cubeMeshTri.push_back({ C[q[2]],col });
        cubeMeshTri.push_back({ C[q[0]],col });
        cubeMeshTri.push_back({ C[q[2]],col });
        cubeMeshTri.push_back({ C[q[3]],col });
    }
}

// ================================================================
// Center cube: face drawing + face animations + movement + axis rotations
// ================================================================

// 면별 그리기 상태
bool showFace[6] = { false,false,false,false,false,false };
bool showAllWhenEmpty = true; // 아무 면도 선택 안됐을 때 전체 출력 여부

// 중앙 큐브 이동/회전
vec3 cubePos(0, 0, 0);
float cubeRotX = 0, cubeRotY = 0;
float cubeMoveStep = 0.1f;
float cubeRotStep = 5.0f;

// 면 애니메이션 on/off
bool animTop = false;
bool animFront = false;
bool animSide = false;
bool animBack = false;

// 애니메이션 변수
float topSpinAng = 0.0f;
float frontAng = 0.0f; bool frontOpening = true;
float sideSpinAng = 0.0f;
float backScale = 1.0f; bool backShrinking = true;

std::vector<vec3> applyMat(const std::vector<vec3>& in, const mat4& T) {
    std::vector<vec3> out; out.reserve(in.size());
    for (auto& p : in) {
        glm::vec4 q = T * glm::vec4(p, 1);
        out.push_back(vec3(q));
    }
    return out;
}

void drawCenterCube(const mat4& VP) {
    mat4 G = globalTilt();

    mat4 M(1.0f);
    M = glm::translate(M, cubePos);
    M = glm::rotate(M, glm::radians(cubeRotX), vec3(1, 0, 0));
    M = glm::rotate(M, glm::radians(cubeRotY), vec3(0, 1, 0));

    mat4 baseM = G * M;
    mat4 MVPbase = VP * baseM;

    GLint loc = glGetUniformLocation(program, "uMVP");
    glUniformMatrix4fv(loc, 1, GL_FALSE, &MVPbase[0][0]);

    bool any = false;
    for (int i = 0; i < 6; i++) if (showFace[i]) any = true;
    bool drawAll = (!any && showAllWhenEmpty);

    for (int f = 0; f < 6; ++f) {
        if (!drawAll && !showFace[f]) continue;

        std::vector<vec3> fv;
        for (int k = 0; k < 4; k++) fv.push_back(C[cubeFaces[f][k]]);

        mat4 local(1.0f);

        // 윗면 중심축 회전
        if (f == 4 && animTop) {
            vec3 center(0, size, 0);
            local = glm::translate(local, center);
            local = glm::rotate(local, glm::radians(topSpinAng), vec3(0, 1, 0));
            local = glm::translate(local, -center);
        }

        // 앞면 열기/닫기
        if (f == 0 && animFront) {
            vec3 pivot(0, size, size);
            local = glm::translate(local, pivot);
            local = glm::rotate(local, glm::radians(frontAng), vec3(1, 0, 0));
            local = glm::translate(local, -pivot);
        }

        // 옆면(+X) 제자리 회전
        if (f == 2 && animSide) {
            vec3 center(size, 0, 0);
            local = glm::translate(local, center);
            local = glm::rotate(local, glm::radians(sideSpinAng), vec3(1, 0, 0));
            local = glm::translate(local, -center);
        }

        // 뒷면(-Z) 스케일 펄스
        if (f == 1 && animBack) {
            vec3 center(0, 0, -size);
            local = glm::translate(local, center);
            local = glm::scale(local, vec3(backScale));
            local = glm::translate(local, -center);
        }

        auto tfv = applyMat(fv, local);

        std::vector<Vertex> vtx;
        for (auto& p : tfv) vtx.push_back({ p, cubeFaceCol[f] });
        uploadAndDraw(vtx, GL_TRIANGLE_FAN);
    }
}

// ================================================================
// Two cubes (left + right): modeling transforms together
// ================================================================
struct ObjState {
    vec3 basePos;
    vec3 pos;
    float selfX = 0, selfY = 0;
    float orbitY = 0;
    float scale = 1.0f;
};

ObjState leftObj, rightObj;

mat4 modelObj(const ObjState& o) {
    mat4 M(1.0f);
    M = glm::rotate(M, glm::radians(o.orbitY), vec3(0, 1, 0));
    M = glm::translate(M, o.pos);
    M = glm::rotate(M, glm::radians(o.selfX), vec3(1, 0, 0));
    M = glm::rotate(M, glm::radians(o.selfY), vec3(0, 1, 0));
    M = glm::scale(M, vec3(o.scale));
    return M;
}

void drawTwoObjects(const mat4& VP) {
    mat4 G = globalTilt();
    GLint loc = glGetUniformLocation(program, "uMVP");

    // left cube
    {
        mat4 MVP = VP * (G * modelObj(leftObj));
        glUniformMatrix4fv(loc, 1, GL_FALSE, &MVP[0][0]);
        uploadAndDraw(cubeMeshTri, GL_TRIANGLES);
    }
    // right cube
    {
        mat4 MVP = VP * (G * modelObj(rightObj));
        glUniformMatrix4fv(loc, 1, GL_FALSE, &MVP[0][0]);
        uploadAndDraw(cubeMeshTri, GL_TRIANGLES);
    }
}

// ================================================================
// Floor (바닥) 추가
// ================================================================
void drawFloor(const mat4& VP) {
    mat4 G = globalTilt();

    // 바닥은 y = -size에 큰 XZ 사각형
    float y = -size;
    float half = 4.0f; // 바닥 크기

    std::vector<Vertex> floorV = {
        {{-half, y, -half}, {0.7f,0.7f,0.7f}},
        {{ half, y, -half}, {0.7f,0.7f,0.7f}},
        {{ half, y,  half}, {0.7f,0.7f,0.7f}},

        {{-half, y, -half}, {0.7f,0.7f,0.7f}},
        {{ half, y,  half}, {0.7f,0.7f,0.7f}},
        {{-half, y,  half}, {0.7f,0.7f,0.7f}}
    };

    mat4 MVP = VP * G;
    GLint loc = glGetUniformLocation(program, "uMVP");
    glUniformMatrix4fv(loc, 1, GL_FALSE, &MVP[0][0]);

    uploadAndDraw(floorV, GL_TRIANGLES);
}

// ================================================================
// Update / idle
// ================================================================
int lastMs = 0;
void idle() {
    int now = glutGet(GLUT_ELAPSED_TIME);
    float dt = (now - lastMs) / 1000.0f;
    lastMs = now;

    if (animTop) {
        topSpinAng += 120.0f * dt;
        if (topSpinAng > 360) topSpinAng -= 360;
    }
    if (animFront) {
        float v = 60.0f * dt;
        frontAng += frontOpening ? v : -v;
        if (frontAng >= 90) { frontAng = 90; frontOpening = false; }
        if (frontAng <= 0) { frontAng = 0;  frontOpening = true; }
    }
    if (animSide) {
        sideSpinAng += 180.0f * dt;
        if (sideSpinAng > 360) sideSpinAng -= 360;
    }
    if (animBack) {
        float v = 1.2f * dt;
        backScale += backShrinking ? -v : v;
        if (backScale <= 0) { backScale = 0; backShrinking = false; }
        if (backScale >= 1) { backScale = 1; backShrinking = true; }
    }

    glutPostRedisplay();
}

// ================================================================
// Display
// ================================================================
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(program);
    glBindVertexArray(vao);

    if (depthOn) glEnable(GL_DEPTH_TEST);
    else        glDisable(GL_DEPTH_TEST);

    glPolygonMode(GL_FRONT_AND_BACK, wireOn ? GL_LINE : GL_FILL);

    mat4 Pj = glm::perspective(glm::radians(45.0f), (float)W / H, 0.1f, 100.0f);
    mat4 V = glm::lookAt(vec3(2, 2, 4), vec3(0, 0, 0), vec3(0, 1, 0));
    mat4 VP = Pj * V;

    drawAxes(VP);
    drawFloor(VP);      // 바닥 먼저
    drawCenterCube(VP);
    drawTwoObjects(VP);

    glutSwapBuffers();
}

// ================================================================
// Reset
// ================================================================
void resetAll() {
    for (int i = 0; i < 6; i++) showFace[i] = false;
    showAllWhenEmpty = true; // 리셋하면 전체 보이게

    cubePos = vec3(0, 0, 0);
    cubeRotX = cubeRotY = 0;

    animTop = animFront = animSide = animBack = false;
    topSpinAng = 0;
    frontAng = 0; frontOpening = true;
    sideSpinAng = 0;
    backScale = 1; backShrinking = true;

    leftObj.basePos = vec3(-1.5f, 0, 0);
    rightObj.basePos = vec3(+1.5f, 0, 0);
    leftObj.pos = leftObj.basePos;
    rightObj.pos = rightObj.basePos;
    leftObj.selfX = leftObj.selfY = rightObj.selfX = rightObj.selfY = 0;
    leftObj.orbitY = rightObj.orbitY = 0;
    leftObj.scale = rightObj.scale = 1.0f;

    depthOn = true; wireOn = false;
}

// ================================================================
// Keyboard
// ================================================================
void keyboard(unsigned char k, int, int) {
    const float rot = 5.0f;
    const float orbit = 5.0f;
    const float move = 0.1f;

    switch (k) {
        // common toggles
    case 'h': depthOn = !depthOn; break;
    case 'w': case 'W': wireOn = !wireOn; break;
    case 's': resetAll(); break;
    case 'q': case 27: exit(0);

        // face toggle
    case '1': showFace[0] = !showFace[0]; break;
    case '2': showFace[1] = !showFace[1]; break;
    case '3': showFace[2] = !showFace[2]; break;
    case '4': showFace[3] = !showFace[3]; break;
    case '5': showFace[4] = !showFace[4]; break;
    case '6': showFace[5] = !showFace[5]; break;

        // ✅ z: 전체 면 켜기
    case 'z':
        for (int i = 0; i < 6; i++) showFace[i] = true;
        showAllWhenEmpty = true;
        break;

        // ✅ m: 전체 면 끄기 (핵심 수정!)
    case 'm':
        for (int i = 0; i < 6; i++) showFace[i] = false;
        showAllWhenEmpty = false;   // << 이게 없어서 전체가 다시 그려졌던 것
        break;

        // center cube axis rotation
    case 'i': cubeRotX += cubeRotStep; break;
    case 'k': cubeRotX -= cubeRotStep; break;
    case 'j': cubeRotY += cubeRotStep; break;
    case 'l': cubeRotY -= cubeRotStep; break;

        // center cube animations
    case 't': animTop = !animTop; break;
    case 'f': animFront = !animFront; break;
    case 'o': animSide = !animSide; break;
    case 'u': animBack = !animBack; break; // b 충돌 피해서 u

        // two cubes modeling transforms together
    case 'x': leftObj.selfX += rot; rightObj.selfX += rot; break;
    case 'X': leftObj.selfX -= rot; rightObj.selfX -= rot; break;
    case 'y': leftObj.selfY += rot; rightObj.selfY += rot; break;
    case 'Y': leftObj.selfY -= rot; rightObj.selfY -= rot; break;

    case 'r': leftObj.orbitY += orbit; rightObj.orbitY += orbit; break;
    case 'R': leftObj.orbitY -= orbit; rightObj.orbitY -= orbit; break;

    case 'a': leftObj.scale *= 1.1f; rightObj.scale *= 1.1f; break;
    case 'A': leftObj.scale *= 0.9f; rightObj.scale *= 0.9f; break;

    case 'b':
        leftObj.pos *= 1.1f; rightObj.pos *= 1.1f;
        leftObj.scale *= 1.1f; rightObj.scale *= 1.1f;
        break;
    case 'B':
        leftObj.pos *= 0.9f; rightObj.pos *= 0.9f;
        leftObj.scale *= 0.9f; rightObj.scale *= 0.9f;
        break;

    case 'd': leftObj.pos.x += move; rightObj.pos.x += move; break;
    case 'D': leftObj.pos.x -= move; rightObj.pos.x -= move; break;
    case 'e': leftObj.pos.y += move; rightObj.pos.y += move; break;
    case 'E': leftObj.pos.y -= move; rightObj.pos.y -= move; break;
    }

    glutPostRedisplay();
}

void special(int key, int, int) {
    switch (key) {
    case GLUT_KEY_LEFT:  cubePos.x -= cubeMoveStep; break;
    case GLUT_KEY_RIGHT: cubePos.x += cubeMoveStep; break;
    case GLUT_KEY_UP:    cubePos.z -= cubeMoveStep; break;
    case GLUT_KEY_DOWN:  cubePos.z += cubeMoveStep; break;
    }
    glutPostRedisplay();
}

void reshape(int w, int h) {
    W = w; H = h; glViewport(0, 0, w, h);
}

// ================================================================
// init
// ================================================================
void initGL() {
    glewInit();
    glEnable(GL_DEPTH_TEST);

    makeProgram();
    buildBaseVerts();
    buildCubeMesh();

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);

    glClearColor(1, 1, 1, 1);
    resetAll();
    lastMs = glutGet(GLUT_ELAPSED_TIME);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(W, H);
    glutCreateWindow("Unified Example - Cubes Only + Floor");

    initGL();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutIdleFunc(idle);

    glutMainLoop();
    return 0;
}
