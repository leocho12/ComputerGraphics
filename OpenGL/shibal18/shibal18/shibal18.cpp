// Practice18_full.cpp
// g++ Practice18_full.cpp -lglut -lGLEW -lGL -std=c++17
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>

using glm::vec3;
using glm::mat4;

struct Vertex {
    vec3 pos;
    vec3 color;
};

GLuint program = 0, vao = 0, vbo = 0;
int windowWidth = 800, windowHeight = 600;

// ====== 공통 기울기(좌표계/객체 모두) ======
float basePitchDeg = 0.0f;  // x축
float baseYawDeg = 0.0f; // y축

// ====== 셰이더 로딩 ======
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

// ====== 객체 타입 ======
enum ShapeType { SHAPE_SPHERE, SHAPE_CUBE };

struct ObjectState {
    ShapeType type;
    vec3 pos;              // 현재 위치
    float selfRotX = 0;      // 자전 X
    float selfRotY = 0;      // 자전 Y
    float orbitY = 0;        // 공전 각도(Y축 around origin)
    float scale = 1.0f;      // 제자리 스케일
};

ObjectState leftObj, rightObj;

// 변환 대상 선택
int targetMask = 3; // 1=left, 2=right, 3=both

// 공통 옵션
bool depthOn = true;

// ====== 애니메이션 상태 ======
bool animT = false, animU = false, animV = false;
float animTime = 0.0f;
const float animDurT = 2.0f;
const float animDurU = 2.0f;
const float animDurV = 2.0f;

vec3 tStartL, tStartR, tEndL, tEndR;
float vBaseScaleL = 1.0f, vBaseScaleR = 1.0f;

// ====== 메시 ======
std::vector<Vertex> cubeMesh;
std::vector<Vertex> sphereMesh;

// --- VBO 업로드/드로우
void uploadAndDraw(const std::vector<Vertex>& vtx, GLenum mode) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vtx.size() * sizeof(Vertex), vtx.data(), GL_DYNAMIC_DRAW);
    glDrawArrays(mode, 0, (GLsizei)vtx.size());
}

// ====== 축 ======
void drawAxes(const mat4& VP) {
    GLint locMVP = glGetUniformLocation(program, "uMVP");
    glUniformMatrix4fv(locMVP, 1, GL_FALSE, &VP[0][0]);

    std::vector<Vertex> axes = {
        {{0,0,0},{1,0,0}}, {{1,0,0},{1,0,0}}, // x red
        {{0,0,0},{0,1,0}}, {{0,1,0},{0,1,0}}, // y green
        {{0,0,0},{0,0,1}}, {{0,0,1},{0,0,1}}  // z blue
    };
    uploadAndDraw(axes, GL_LINES);
}

// ====== 큐브 메시 생성(면마다 다른색) ======
void buildCubeMesh(float s = 0.6f) {
    vec3 C[8] = {
        {-s,-s,-s},{ s,-s,-s},{ s, s,-s},{-s, s,-s},
        {-s,-s, s},{ s,-s, s},{ s, s, s},{-s, s, s}
    };
    int faces[6][4] = {
        {4,5,6,7}, // +Z
        {0,1,2,3}, // -Z
        {1,5,6,2}, // +X
        {0,4,7,3}, // -X
        {3,2,6,7}, // +Y
        {0,1,5,4}  // -Y
    };
    vec3 col[6] = {
        {0,1,1}, {1,0,1}, {0,1,0}, {0,0,1}, {1,1,0}, {1,0,0}
    };

    cubeMesh.clear();
    for (int f = 0; f < 6; ++f) {
        int* q = faces[f];
        // 두 삼각형
        cubeMesh.push_back({ C[q[0]], col[f] });
        cubeMesh.push_back({ C[q[1]], col[f] });
        cubeMesh.push_back({ C[q[2]], col[f] });

        cubeMesh.push_back({ C[q[0]], col[f] });
        cubeMesh.push_back({ C[q[2]], col[f] });
        cubeMesh.push_back({ C[q[3]], col[f] });
    }
}

// ====== 구 메시 생성(버텍스 컬러 그라데이션) ======
void buildSphereMesh(float r = 0.6f, int stacks = 18, int slices = 36) {
    sphereMesh.clear();
    for (int i = 0; i < stacks; i++) {
        float v0 = (float)i / stacks;
        float v1 = (float)(i + 1) / stacks;
        float phi0 = glm::pi<float>() * (v0 - 0.5f);
        float phi1 = glm::pi<float>() * (v1 - 0.5f);

        for (int j = 0; j < slices; j++) {
            float u0 = (float)j / slices;
            float u1 = (float)(j + 1) / slices;
            float th0 = glm::two_pi<float>() * u0;
            float th1 = glm::two_pi<float>() * u1;

            vec3 p00 = r * vec3(cos(phi0) * cos(th0), sin(phi0), cos(phi0) * sin(th0));
            vec3 p10 = r * vec3(cos(phi0) * cos(th1), sin(phi0), cos(phi0) * sin(th1));
            vec3 p01 = r * vec3(cos(phi1) * cos(th0), sin(phi1), cos(phi1) * sin(th0));
            vec3 p11 = r * vec3(cos(phi1) * cos(th1), sin(phi1), cos(phi1) * sin(th1));

            auto colorFromPos = [&](const vec3& p) {
                vec3 n = glm::normalize(p);
                return vec3(n.x * 0.5f + 0.5f, n.y * 0.5f + 0.5f, n.z * 0.5f + 0.5f);
                };

            vec3 c00 = colorFromPos(p00);
            vec3 c10 = colorFromPos(p10);
            vec3 c01 = colorFromPos(p01);
            vec3 c11 = colorFromPos(p11);

            // 두 삼각형
            sphereMesh.push_back({ p00,c00 });
            sphereMesh.push_back({ p10,c10 });
            sphereMesh.push_back({ p11,c11 });

            sphereMesh.push_back({ p00,c00 });
            sphereMesh.push_back({ p11,c11 });
            sphereMesh.push_back({ p01,c01 });
        }
    }
}

// ====== 오브젝트 그리기 ======
void drawShape(const ObjectState& obj) {
    if (obj.type == SHAPE_CUBE) {
        uploadAndDraw(cubeMesh, GL_TRIANGLES);
    }
    else {
        uploadAndDraw(sphereMesh, GL_TRIANGLES);
    }
}

// ====== 모델 매트릭스 생성 ======
mat4 makeModelMatrix(const ObjectState& obj) {
    mat4 M(1.0f);

    // 공전: 원점 중심 Y축 회전
    M = glm::rotate(M, glm::radians(obj.orbitY), vec3(0, 1, 0));

    // 위치 이동
    M = glm::translate(M, obj.pos);

    // 자전
    M = glm::rotate(M, glm::radians(obj.selfRotX), vec3(1, 0, 0));
    M = glm::rotate(M, glm::radians(obj.selfRotY), vec3(0, 1, 0));

    // 제자리 스케일
    M = glm::scale(M, vec3(obj.scale));

    return M;
}

// ====== display ======
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(program);
    glBindVertexArray(vao);

    if (depthOn) glEnable(GL_DEPTH_TEST);
    else        glDisable(GL_DEPTH_TEST);

    // 카메라
    mat4 Pj = glm::perspective(glm::radians(45.0f),
        (float)windowWidth / windowHeight,
        0.1f, 100.0f);
    mat4 V = glm::lookAt(vec3(2, 2, 3), vec3(0, 0, 0), vec3(0, 1, 0));
    mat4 VP = Pj * V;

    // 축(고정)
    drawAxes(VP);

    // 공통 기울기
    mat4 G(1.0f);
    G = glm::rotate(G, glm::radians(basePitchDeg), vec3(1, 0, 0));
    G = glm::rotate(G, glm::radians(baseYawDeg), vec3(0, 1, 0));

    GLint locMVP = glGetUniformLocation(program, "uMVP");

    // 왼쪽
    {
        mat4 M = G * makeModelMatrix(leftObj);
        mat4 MVP = VP * M;
        glUniformMatrix4fv(locMVP, 1, GL_FALSE, &MVP[0][0]);
        drawShape(leftObj);
    }
    // 오른쪽
    {
        mat4 M = G * makeModelMatrix(rightObj);
        mat4 MVP = VP * M;
        glUniformMatrix4fv(locMVP, 1, GL_FALSE, &MVP[0][0]);
        drawShape(rightObj);
    }

    glutSwapBuffers();
}

// ====== 유틸: 대상에 변환 적용 ======
template<typename F>
void applyToTargets(F fn) {
    if (targetMask == 1 || targetMask == 3) fn(leftObj);
    if (targetMask == 2 || targetMask == 3) fn(rightObj);
}

// ====== reset ======
void resetAll() {
    leftObj.type = SHAPE_SPHERE;
    rightObj.type = SHAPE_CUBE;

    leftObj.pos = vec3(-1, 0, 0);
    rightObj.pos = vec3(1, 0, 0);

    leftObj.selfRotX = leftObj.selfRotY = 0;
    rightObj.selfRotX = rightObj.selfRotY = 0;

    leftObj.orbitY = rightObj.orbitY = 0;
    leftObj.scale = rightObj.scale = 1.0f;

    targetMask = 3;
    depthOn = true;

    animT = animU = animV = false;
    animTime = 0.0f;
}

// ====== 애니메이션 시작 함수들 ======
void startAnimT() {
    animT = true; animU = false; animV = false;
    animTime = 0.0f;

    tStartL = leftObj.pos;
    tStartR = rightObj.pos;
    tEndL = tStartR;
    tEndR = tStartL;
}
void startAnimU() {
    animU = true; animT = false; animV = false;
    animTime = 0.0f;

    tStartL = leftObj.pos;
    tStartR = rightObj.pos;
    tEndL = tStartR;
    tEndR = tStartL;
}
void startAnimV() {
    animV = true; animT = false; animU = false;
    animTime = 0.0f;

    vBaseScaleL = leftObj.scale;
    vBaseScaleR = rightObj.scale;
}

// ====== idle ======
int lastTimeMs = 0;
void idle() {
    int now = glutGet(GLUT_ELAPSED_TIME);
    float dt = (now - lastTimeMs) / 1000.0f;
    lastTimeMs = now;

    // ====== 애니메이션 t ======
    if (animT) {
        animTime += dt;
        float u = animTime / animDurT;
        if (u >= 1.0f) {
            u = 1.0f; animT = false;
            leftObj.pos = tEndL;
            rightObj.pos = tEndR;
        }
        // 선형 교환(원점 통과)
        leftObj.pos = glm::mix(tStartL, tEndL, u);
        rightObj.pos = glm::mix(tStartR, tEndR, u);
    }

    // ====== 애니메이션 u ======
    if (animU) {
        animTime += dt;
        float u = animTime / animDurU;
        if (u >= 1.0f) {
            u = 1.0f; animU = false;
            leftObj.pos = tEndL;
            rightObj.pos = tEndR;
        }
        float h = 0.8f * sin(glm::pi<float>() * u);
        leftObj.pos = glm::mix(tStartL, tEndL, u) + vec3(0, +h, 0);
        rightObj.pos = glm::mix(tStartR, tEndR, u) + vec3(0, -h, 0);
    }

    // ====== 애니메이션 v ======
    if (animV) {
        animTime += dt;
        float phase = fmod(animTime / animDurV, 1.0f); // 0..1 반복
        float s = 0.5f * (sin(glm::two_pi<float>() * phase) + 1.0f); // 0..1

        // 왼쪽 확대, 오른쪽 축소
        leftObj.scale = vBaseScaleL * (0.6f + 0.8f * s);
        rightObj.scale = vBaseScaleR * (1.4f - 0.8f * s);

        // 자전+공전도 같이
        leftObj.selfRotY += 120.0f * dt;
        rightObj.selfRotY += 120.0f * dt;
        leftObj.orbitY += 60.0f * dt;
        rightObj.orbitY += 60.0f * dt;
    }

    glutPostRedisplay();
}

// ====== 키보드 ======
void keyboard(unsigned char key, int, int) {
    const float rotStep = 5.0f;
    const float orbitStep = 5.0f;
    const float moveStep = 0.1f;

    switch (key) {
        // 대상 선택
    case '1': targetMask = 1; break;
    case '2': targetMask = 2; break;
    case '3': targetMask = 3; break;

        // 자전
    case 'x':
        applyToTargets([&](ObjectState& o) { o.selfRotX += rotStep; });
        break;
    case 'X':
        applyToTargets([&](ObjectState& o) { o.selfRotX -= rotStep; });
        break;
    case 'y':
        applyToTargets([&](ObjectState& o) { o.selfRotY += rotStep; });
        break;
    case 'Y':
        applyToTargets([&](ObjectState& o) { o.selfRotY -= rotStep; });
        break;

        // 공전
    case 'r':
        applyToTargets([&](ObjectState& o) { o.orbitY += orbitStep; });
        break;
    case 'R':
        applyToTargets([&](ObjectState& o) { o.orbitY -= orbitStep; });
        break;

        // 제자리 확대/축소
    case 'a':
        applyToTargets([&](ObjectState& o) { o.scale *= 1.1f; });
        break;
    case 'A':
        applyToTargets([&](ObjectState& o) { o.scale *= 0.9f; });
        break;

        // 원점 기준 확대/축소 (위치도 같이 변함)
    case 'b':
        applyToTargets([&](ObjectState& o) {
            float f = 1.1f;
            o.pos *= f; o.scale *= f;
            });
        break;
    case 'B':
        applyToTargets([&](ObjectState& o) {
            float f = 0.9f;
            o.pos *= f; o.scale *= f;
            });
        break;

        // 이동
    case 'd':
        applyToTargets([&](ObjectState& o) { o.pos.x += moveStep; });
        break;
    case 'D':
        applyToTargets([&](ObjectState& o) { o.pos.x -= moveStep; });
        break;
    case 'e':
        applyToTargets([&](ObjectState& o) { o.pos.y += moveStep; });
        break;
    case 'E':
        applyToTargets([&](ObjectState& o) { o.pos.y -= moveStep; });
        break;

        // 애니메이션
    case 't': startAnimT(); break;
    case 'u': startAnimU(); break;
    case 'v': startAnimV(); break;

        // 도형 바꾸기(좌우 스왑)
    case 'c':
        std::swap(leftObj.type, rightObj.type);
        break;

        // 초기화
    case 's': resetAll(); break;

        // 은면 제거
    case 'h': depthOn = !depthOn; break;

        // 종료
    case 'q':
    case 27: exit(0);
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

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);

    glClearColor(1, 1, 1, 1); // 슬라이드 배경 흰색
    buildCubeMesh(0.6f);
    buildSphereMesh(0.6f);

    resetAll();
    lastTimeMs = glutGet(GLUT_ELAPSED_TIME);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Practice 18");

    initGL();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(idle);

    glutMainLoop();
    return 0;
}
