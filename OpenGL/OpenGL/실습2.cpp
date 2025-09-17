#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cstdlib>
#include <ctime>
#include <iostream>

struct Rect {
    float x1, y1, x2, y2; 
    float r, g, b;        
};

Rect rects[4];
float bgColor[3] = { 1.0f, 1.0f, 1.0f };
int winW = 800, winH = 600;

float randf() { return (float)rand() / RAND_MAX; }

void initRects() {
    rects[0] = { -1.0f,  0.0f,  0.0f,  1.0f, 1, 0.8f, 0.0f };   // 좌상
    rects[1] = { 0.0f,  0.0f,  1.0f,  1.0f, 0.3f, 0.8f, 0.3f }; // 우상
    rects[2] = { -1.0f, -1.0f,  0.0f,  0.0f, 0.9f, 0.5f, 0.2f }; // 좌하
    rects[3] = { 0.0f, -1.0f,  1.0f,  0.0f, 0.5f, 0.7f, 1.0f }; // 우하
}

void display() {
    glClearColor(bgColor[0], bgColor[1], bgColor[2], 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    for (int i = 0; i < 4; i++) {
        glColor3f(rects[i].r, rects[i].g, rects[i].b);
        glRectf(rects[i].x1, rects[i].y1, rects[i].x2, rects[i].y2);
    }

    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    winW = w; winH = h;
}

bool insideRect(int idx, float mx, float my) {
    return mx >= rects[idx].x1 && mx <= rects[idx].x2 &&
        my >= rects[idx].y1 && my <= rects[idx].y2;
}

// 사각형 크기 조절
void scaleRect(Rect& r, float scale) {
    float cx = (r.x1 + r.x2) / 2.0f;
    float cy = (r.y1 + r.y2) / 2.0f;
    float halfW = (r.x2 - r.x1) / 2.0f * scale;
    float halfH = (r.y2 - r.y1) / 2.0f * scale;

    // 최소/최대 크기 제한
    float minSize = 0.1f;
    float maxSize = 2.0f;
    if (halfW * 2 < minSize || halfH * 2 < minSize) return;
    if (halfW * 2 > maxSize || halfH * 2 > maxSize) return;

    r.x1 = cx - halfW;
    r.x2 = cx + halfW;
    r.y1 = cy - halfH;
    r.y2 = cy + halfH;
}

void mouse(int button, int state, int x, int y) {
    if (state != GLUT_DOWN) return;

    float mx = (float)x / winW * 2 - 1;
    float my = 1 - (float)y / winH * 2;

    bool clickedRect = false;
    for (int i = 0; i < 4; i++) {
        if (insideRect(i, mx, my)) {
            clickedRect = true;
            if (button == GLUT_LEFT_BUTTON) {
                rects[i].r = randf();
                rects[i].g = randf();
                rects[i].b = randf();
            }
            else if (button == GLUT_RIGHT_BUTTON) {
                scaleRect(rects[i], 0.9f);
            }
        }
    }

    if (!clickedRect) {
        if (button == GLUT_LEFT_BUTTON) {
            bgColor[0] = randf(); bgColor[1] = randf(); bgColor[2] = randf();
        }
        else if (button == GLUT_RIGHT_BUTTON) {
            int target = 0;
			float dis = 1e6;
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    if (j == i) continue;
                    float cx = (rects[j].x1 + rects[j].x2) / 2.0f;
                    float cy = (rects[j].y1 + rects[j].y2) / 2.0f;
                    float dist = (mx - cx) * (mx - cx) + (my - cy) * (my - cy);
                    if (dist < dis) {
                        dis = dist;
                        target = j;
                    }
                }
            }
                scaleRect(rects[target], 1.1f);
        }
    }

    glutPostRedisplay();
}

int main(int argc, char** argv) {
    srand((unsigned int)time(NULL));

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(winW, winH);
    glutCreateWindow("Rectangles Mouse Control");

    glewInit();

    initRects();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);

    glutMainLoop();
    return 0;
}
