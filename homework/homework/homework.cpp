#include <iostream>
#include <cstdlib>
#include <cstdio>

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
#include <map>
#include <random>
#include <functional>
#include <algorithm>
#include <array>
#include <ctime>
#include <cmath>

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "freeglut.lib")
#pragma warning(disable: 4711 4710 4100)

// ----------------- 셀 구조체 -----------------
struct Cell {
    float baseHeight;   // 기본 높이
    float curHeight;    // 현재 높이
    float maxOffset;    // 위아래로 움직이는 최대 편차
    float speed;        // 애니메이션 속도
    float phase;        // 애니메이션 위상
    bool  isMazePath;   // 미로 길인가?
    bool  hasCube;      // 큐브를 그릴 것인가?
    glm::vec3 color;    // ★ 이 셀의 기본 색 (미로 길에서 사용)
};

// ----------------- 전역 변수 -----------------
int gridW = 10;
int gridH = 10;
int windowWidth = 1280;
int windowHeight = 720;

std::vector<std::vector<Cell>> cells;

// 플레이어
int playerCellX = 0;
int playerCellZ = 0;
glm::vec3 playerPos(0.5f, 1.0f, 0.5f);
bool playerActive = false;

// 카메라 / 투영 모드
enum ProjectionMode { ORTHO_MODE, PERSPECTIVE_MODE };
ProjectionMode projMode = PERSPECTIVE_MODE;

enum CameraMode { FIRST_PERSON, THIRD_PERSON };
CameraMode camMode = THIRD_PERSON;

float camYaw = 45.0f;
float camDist = 30.0f;
float camHeight = 15.0f;

// 애니메이션 제어
bool animateCubes = true;
bool lowMountainMode = false;
float globalSpeedScale = 1.0f;

// 시간
float lastTime = 0.0f;

// OpenGL 객체
GLuint vao = 0, vbo = 0, ebo = 0;
GLuint shaderProgram = 0;
GLint locModel = -1, locView = -1, locProj = -1;
GLint locColor = -1;              // ★ uColor 위치

// RNG
std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));

// ----------------- 유틸: 파일 / 셰이더 -----------------
std::string loadTextFile(const char* filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "파일을 열 수 없습니다: " << filename << std::endl;
        return "";
    }
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

GLuint compileShader(GLenum type, const char* filename)
{
    std::string src = loadTextFile(filename);
    const char* cstr = src.c_str();

    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &cstr, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLint logLen;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
        std::vector<char> log(logLen);
        glGetShaderInfoLog(shader, logLen, nullptr, log.data());
        std::cerr << filename << " 셰이더 컴파일 에러:\n" << log.data() << std::endl;
    }
    return shader;
}

GLuint createShaderProgram(const char* vsFile, const char* fsFile)
{
    GLuint vs = compileShader(GL_VERTEX_SHADER, vsFile);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fsFile);

    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);

    GLint success;
    glGetProgramiv(prog, GL_LINK_STATUS, &success);
    if (!success) {
        GLint logLen;
        glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLen);
        std::vector<char> log(logLen);
        glGetProgramInfoLog(prog, logLen, nullptr, log.data());
        std::cerr << "프로그램 링크 에러:\n" << log.data() << std::endl;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);
    return prog;
}

// ----------------- 셀 / 산 초기화 -----------------
void initCells()
{
    cells.assign(gridH, std::vector<Cell>(gridW));

    std::uniform_real_distribution<float> randSpeed(0.5f, 2.0f);
    std::uniform_real_distribution<float> randOffset(1.0f, 4.0f);
    std::uniform_real_distribution<float> randPhase(0.0f, 6.28318f);
    std::uniform_real_distribution<float> randColor(0.3f, 1.0f); // ★ 색 분포

    for (int z = 0; z < gridH; ++z) {
        for (int x = 0; x < gridW; ++x) {
            Cell& c = cells[z][x];
            c.baseHeight = 1.0f;
            c.curHeight = c.baseHeight;
            c.maxOffset = randOffset(rng);
            c.speed = randSpeed(rng);
            c.phase = randPhase(rng);
            c.isMazePath = false;
            c.hasCube = true;

            // ★ 각 셀마다 랜덤 색
            c.color = glm::vec3(
                randColor(rng),
                randColor(rng),
                randColor(rng)
            );
        }
    }

    // 플레이어 초기값
    playerCellX = gridW / 2;
    playerCellZ = gridH / 2;
    playerPos = glm::vec3(playerCellX + 0.5f, 1.0f, playerCellZ + 0.5f);
    playerActive = false;
}

// ----------------- 큐브 VAO/VBO -----------------
void initCube()
{
    // 1x1x1 정육면체
    float vertices[] = {
        // 뒤
        -0.5f,-0.5f,-0.5f,   1.0f, 1.0f, 1.0f,
         0.5f,-0.5f,-0.5f,   1.0f, 1.0f, 1.0f,
         0.5f, 0.5f,-0.5f,   1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f,-0.5f,   1.0f, 1.0f, 1.0f,
        // 앞
        -0.5f,-0.5f, 0.5f,   1.0f, 1.0f, 1.0f,
         0.5f,-0.5f, 0.5f,   1.0f, 1.0f, 1.0f,
         0.5f, 0.5f, 0.5f,   1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f,   1.0f, 1.0f, 1.0f
    };

    unsigned int indices[] = {
        // 뒤
        0,1,2,  2,3,0,
        // 앞
        4,5,6,  6,7,4,
        // 왼
        0,4,7,  7,3,0,
        // 오른
        1,5,6,  6,2,1,
        // 아래
        0,1,5,  5,4,0,
        // 위
        3,2,6,  6,7,3
    };

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // 위치 (location=0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // 색상 (location=1) – 지금은 전부 1,1,1 이고 uColor로 최종색 결정
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
        (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

// ----------------- 카메라/투영 -----------------
glm::mat4 viewMat(1.0f);
glm::mat4 projMat(1.0f);

void updateMainCamera()
{
    float centerX = gridW / 2.0f;
    float centerZ = gridH / 2.0f;

    if (camMode == THIRD_PERSON) {
        float rad = glm::radians(camYaw);
        float eyeX = centerX + camDist * std::cos(rad);
        float eyeZ = centerZ + camDist * std::sin(rad);
        float eyeY = camHeight;

        viewMat = glm::lookAt(
            glm::vec3(eyeX, eyeY, eyeZ),
            glm::vec3(centerX, 0.0f, centerZ),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );
    }
    else {
        float rad = glm::radians(camYaw);
        glm::vec3 eye = playerPos + glm::vec3(0.0f, 0.5f, 0.0f);
        glm::vec3 center = eye + glm::vec3(std::cos(rad), 0.0f, std::sin(rad));

        viewMat = glm::lookAt(
            eye, center,
            glm::vec3(0.0f, 1.0f, 0.0f)
        );
    }

    float aspect = static_cast<float>(windowWidth) / windowHeight;
    if (projMode == PERSPECTIVE_MODE) {
        projMat = glm::perspective(
            glm::radians(60.0f), aspect, 0.1f, 200.0f
        );
    }
    else {
        float size = std::max(gridW, gridH) * 0.8f;
        projMat = glm::ortho(
            -size, size, -size, size, -100.0f, 100.0f
        );
    }

    glUseProgram(shaderProgram);
    glUniformMatrix4fv(locView, 1, GL_FALSE, glm::value_ptr(viewMat));
    glUniformMatrix4fv(locProj, 1, GL_FALSE, glm::value_ptr(projMat));
}

void updateMiniMapCamera()
{
    projMat = glm::ortho(
        0.0f, static_cast<float>(gridW),
        0.0f, static_cast<float>(gridH),
        -10.0f, 10.0f
    );

    viewMat = glm::lookAt(
        glm::vec3(gridW / 2.0f, 10.0f, gridH / 2.0f),
        glm::vec3(gridW / 2.0f, 0.0f, gridH / 2.0f),
        glm::vec3(0.0f, 0.0f, -1.0f)
    );

    glUseProgram(shaderProgram);
    glUniformMatrix4fv(locView, 1, GL_FALSE, glm::value_ptr(viewMat));
    glUniformMatrix4fv(locProj, 1, GL_FALSE, glm::value_ptr(projMat));
}

// ----------------- 그리기 유틸 -----------------
void drawCubeModel(const glm::mat4& model, const glm::vec3& color)
{
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
    glUniform3fv(locColor, 1, glm::value_ptr(color));   // ★ 셀 색 전달
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void drawCubeAt(int gx, int gz, float height)
{
    float x = gx + 0.5f;
    float z = gz + 0.5f;
    float y = height * 0.5f;

    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(x, y, z));
    model = glm::scale(model, glm::vec3(1.0f, height, 1.0f));

    glm::vec3 mountainColor(0.3f, 0.7f, 0.3f);
    drawCubeModel(model, mountainColor);
}

// 바닥
void drawFloor()
{
    glm::vec3 floorColor(0.2f, 0.2f, 0.2f);

    for (int z = 0; z < gridH; ++z) {
        for (int x = 0; x < gridW; ++x) {
            glm::mat4 model(1.0f);
            float posX = x + 0.5f;
            float posZ = z + 0.5f;
            model = glm::translate(model, glm::vec3(posX, 0.0f, posZ));
            model = glm::scale(model, glm::vec3(1.0f, 0.05f, 1.0f));
            drawCubeModel(model, floorColor);
        }
    }
}

// 산 + 미로 길
void drawCubesAndMaze()
{
    for (int z = 0; z < gridH; ++z) {
        for (int x = 0; x < gridW; ++x) {
            Cell& c = cells[z][x];

            if (c.hasCube) {
                drawCubeAt(x, z, c.curHeight); // 산
            }

            if (c.isMazePath) { // ★ 미로 경로 큐브 – 랜덤 색
                glm::mat4 model(1.0f);
                float posX = x + 0.5f;
                float posZ = z + 0.5f;
                model = glm::translate(model, glm::vec3(posX, 0.03f, posZ));
                model = glm::scale(model, glm::vec3(0.9f, 0.02f, 0.9f));
                drawCubeModel(model, c.color);
            }
        }
    }
}

void drawPlayer()
{
    if (!playerActive) return;

    glm::mat4 model(1.0f);
    model = glm::translate(model, playerPos + glm::vec3(0.0f, 0.5f, 0.0f));
    model = glm::scale(model, glm::vec3(0.4f, 1.0f, 0.4f));
    glm::vec3 playerColor(1.0f, 1.0f, 0.2f);
    drawCubeModel(model, playerColor);
}

// 미니맵
void drawMiniMapCells()
{
    glm::vec3 mountainMiniColor(0.2f, 0.5f, 0.2f);

    for (int z = 0; z < gridH; ++z) {
        for (int x = 0; x < gridW; ++x) {
            Cell& c = cells[z][x];

            if (c.hasCube) {
                glm::mat4 model(1.0f);
                model = glm::translate(model, glm::vec3(x + 0.5f, 0.05f, z + 0.5f));
                model = glm::scale(model, glm::vec3(1.0f, 0.1f, 1.0f));
                drawCubeModel(model, mountainMiniColor);
            }

            if (c.isMazePath) {
                glm::mat4 model(1.0f);
                model = glm::translate(model, glm::vec3(x + 0.5f, 0.0f, z + 0.5f));
                model = glm::scale(model, glm::vec3(0.9f, 0.02f, 0.9f));
                drawCubeModel(model, c.color); // ★ 길도 같은 랜덤 색
            }
        }
    }
}

void drawMiniMapPlayer()
{
    if (!playerActive) return;
    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(playerCellX + 0.5f, 0.3f, playerCellZ + 0.5f));
    model = glm::scale(model, glm::vec3(0.4f, 0.3f, 0.4f));
    glm::vec3 playerMiniColor(1.0f, 1.0f, 0.0f);
    drawCubeModel(model, playerMiniColor);
}

// ----------------- 미로 생성 DFS -----------------
void generateMaze()
{
    std::vector<std::vector<bool>> visited(gridH, std::vector<bool>(gridW, false));

    std::function<void(int, int)> dfs = [&](int z, int x)
        {
            visited[z][x] = true;
            cells[z][x].isMazePath = true;
            cells[z][x].hasCube = false; // 길에는 산 제거

            std::array<std::pair<int, int>, 4> dirs = { {
                {0,1}, {0,-1}, {1,0}, {-1,0}
            } };
            std::shuffle(dirs.begin(), dirs.end(), rng);

            for (auto [dz, dx] : dirs) {
                int nz = z + dz;
                int nx = x + dx;
                if (nx < 0 || nx >= gridW || nz < 0 || nz >= gridH) continue;
                if (visited[nz][nx]) continue;
                dfs(nz, nx);
            }
        };

    int startZ = gridH / 2;
    int startX = gridW / 2;
    dfs(startZ, startX);
}

// ----------------- GLUT 콜백 -----------------
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgram);

    // 메인 뷰
    glViewport(0, 0, windowWidth, windowHeight);
    updateMainCamera();
    drawFloor();
    drawCubesAndMaze();
    drawPlayer();

    // 미니맵
    int mini = std::min(windowWidth, windowHeight) / 3;
    glViewport(windowWidth - mini - 10, windowHeight - mini - 10, mini, mini);
    updateMiniMapCamera();
    drawMiniMapCells();
    drawMiniMapPlayer();

    glutSwapBuffers();
}

void reshape(int w, int h)
{
    if (h == 0) h = 1;
    windowWidth = w;
    windowHeight = h;
    glViewport(0, 0, w, h);
}

void idle()
{
    float t = glutGet(GLUT_ELAPSED_TIME) * 0.001f;
    float dt = t - lastTime;
    lastTime = t;

    if (animateCubes && !lowMountainMode) {
        for (int z = 0; z < gridH; ++z) {
            for (int x = 0; x < gridW; ++x) {
                Cell& c = cells[z][x];
                if (!c.hasCube) continue;
                c.phase += c.speed * globalSpeedScale * dt;
                c.curHeight = c.baseHeight +
                    c.maxOffset * std::sin(c.phase);
                if (c.curHeight < 0.2f) c.curHeight = 0.2f;
            }
        }
    }
    else if (lowMountainMode) {
        for (int z = 0; z < gridH; ++z) {
            for (int x = 0; x < gridW; ++x) {
                Cell& c = cells[z][x];
                if (!c.hasCube) continue;
                float target = 0.5f;
                c.curHeight += (target - c.curHeight) * 0.1f;
            }
        }
    }

    glutPostRedisplay();
}

void resetAll()
{
    initCells();
    camYaw = 45.0f;
    camDist = 30.0f;
    camHeight = 15.0f;
    projMode = PERSPECTIVE_MODE;
    camMode = THIRD_PERSON;
    animateCubes = true;
    lowMountainMode = false;
    globalSpeedScale = 1.0f;
}

void keyboard(unsigned char key, int x, int y)
{
    switch (key) {
    case 'q':
    case 'Q':
        std::exit(0);
        break;

    case 'o':
    case 'O':
        projMode = ORTHO_MODE;
        break;

    case 'p':
    case 'P':
        projMode = PERSPECTIVE_MODE;
        break;

    case 'y':
        camYaw += 5.0f;
        break;
    case 'Y':
        camYaw -= 5.0f;
        break;

    case 'z':
        camDist -= 1.0f;
        if (camDist < 5.0f) camDist = 5.0f;
        break;
    case 'Z':
        camDist += 1.0f;
        break;

    case 'm':
    case 'M':
        animateCubes = !animateCubes;
        break;

    case '+':
        globalSpeedScale *= 1.1f;
        break;
    case '-':
        globalSpeedScale *= 0.9f;
        break;

    case 'v':
    case 'V':
        lowMountainMode = !lowMountainMode;
        break;

    case 'c':
    case 'C':
        resetAll();
        break;

    case 'r':
    case 'R':
        generateMaze();
        break;

    case 's':
    case 'S':
        playerCellX = gridW / 2;
        playerCellZ = gridH / 2;
        if (!cells[playerCellZ][playerCellX].isMazePath) {
            bool found = false;
            for (int z = 0; z < gridH && !found; ++z) {
                for (int x = 0; x < gridW && !found; ++x) {
                    if (cells[z][x].isMazePath) {
                        playerCellX = x;
                        playerCellZ = z;
                        found = true;
                    }
                }
            }
        }
        playerPos = glm::vec3(playerCellX + 0.5f, 1.0f, playerCellZ + 0.5f);
        playerActive = true;
        break;

    case '1':
        camMode = FIRST_PERSON;
        break;
    case '3':
        camMode = THIRD_PERSON;
        break;
    }
}

void specialKeys(int key, int x, int y)
{
    int dx = 0, dz = 0;
    if (key == GLUT_KEY_LEFT)  dx = -1;
    if (key == GLUT_KEY_RIGHT) dx = 1;
    if (key == GLUT_KEY_UP)    dz = -1;
    if (key == GLUT_KEY_DOWN)  dz = 1;

    if (!playerActive) return;

    int nx = playerCellX + dx;
    int nz = playerCellZ + dz;

    if (nx >= 0 && nx < gridW && nz >= 0 && nz < gridH) {
        if (cells[nz][nx].isMazePath) {
            playerCellX = nx;
            playerCellZ = nz;
            playerPos = glm::vec3(playerCellX + 0.5f, 1.0f, playerCellZ + 0.5f);
        }
    }
}

// ----------------- 초기화 / main -----------------
void printHelp()
{
    std::cout << "==== 키 설명 ====\n";
    std::cout << "Q : 종료\n";
    std::cout << "O / P : 정투영 / 원근투영\n";
    std::cout << "y / Y : 카메라 좌우 회전\n";
    std::cout << "z / Z : 줌 인 / 아웃\n";
    std::cout << "M : 산 애니메이션 On/Off\n";
    std::cout << "+ / - : 애니메이션 속도 조절\n";
    std::cout << "V : 낮은 산 모드 토글\n";
    std::cout << "C : 전체 초기화\n";
    std::cout << "R : 미로 생성 (산 제거 + 길 생성)\n";
    std::cout << "S : 플레이어 미로에 배치\n";
    std::cout << "방향키 : 미로 안에서 플레이어 이동\n";
    std::cout << "1 / 3 : 1인칭 / 3인칭 모드\n";
    std::cout << "=================\n";
}

void initGL()
{
    glewInit();
    glEnable(GL_DEPTH_TEST);

    shaderProgram = createShaderProgram("vertex.glsl", "fragment.glsl");
    glUseProgram(shaderProgram);

    locModel = glGetUniformLocation(shaderProgram, "modelTransform");
    locView = glGetUniformLocation(shaderProgram, "viewTransform");
    locProj = glGetUniformLocation(shaderProgram, "projectionTransform");
    locColor = glGetUniformLocation(shaderProgram, "uColor"); // ★ 색 uniform

    initCube();

    glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
}

int main(int argc, char** argv)
{
    std::cout << "가로 나누기 개수 (5~25): ";
    std::cin >> gridW;
    std::cout << "세로 나누기 개수 (5~25): ";
    std::cin >> gridH;

    gridW = std::max(5, std::min(25, gridW));
    gridH = std::max(5, std::min(25, gridH));

    initCells();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("움직이는 산과 미로 만들기");

    initGL();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutIdleFunc(idle);

    printHelp();

    glutMainLoop();
    return 0;
}
