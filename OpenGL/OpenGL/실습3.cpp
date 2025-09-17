#include <iostream>
#include<vector>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

using namespace std;

// �簢�� ����ü
struct Rect {
	float x1, y1, x2, y2; // �簢�� ��ǥ
	float r, g, b;        // ���� ����
	bool isSelected = false; // ���� ����
};

// �簢�� �����̳�
vector<Rect> rects;

//--- �Լ�����
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid AddRandomRect(int winW, int winH);
GLvoid Mouse(int button, int state, int x, int y);
GLvoid Motion(int x, int y);
Rect mergeRect(const Rect& r1, const Rect& r2);
vector<Rect> splitRect(const Rect& r);


//--- ��������
float Bgcolor[3] = { 1.0f, 1.0f, 1.0f }; //--- ���� ����
int selectedRectIndex = -1; // ���õ� �簢�� �ε���
int prevMouseX = -1, prevMouseY = -1; // ���� ���콺 ��ġ
bool isOverlap(const Rect& r1, const Rect& r2) {
	return !(r1.x2 < r2.x1 || r1.x1 > r2.x2 || r1.y2 < r2.y1 || r1.y1 > r2.y2);
}


//--- ������ ����ϰ� �ݹ��Լ� ����
void main(int argc, char** argv) 
{
	//--- ������ ����
	glutInit(&argc, argv); //--- glut �ʱ�ȭ
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); //--- ���÷��� ��� ����
	glutInitWindowPosition(0, 0); //--- �������� ��ġ ����
	glutInitWindowSize(800, 600); //--- �������� ũ�� ����
	glutCreateWindow("Example1"); //--- ������ ����(������ �̸�)
	//--- GLEW �ʱ�ȭ
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) //--- glew �ʱ�ȭ 
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized\n";
	glutDisplayFunc(drawScene); //--- ��� �ݹ��Լ��� ����
	glutReshapeFunc(Reshape); //--- �ٽ� �׸��� �ݹ��Լ� ����
	glutKeyboardFunc(Keyboard); //--- Ű���� �Է� �ݹ��Լ� ����
	glutMouseFunc(Mouse);     // Ŭ��/���� �̺�Ʈ
	glutMotionFunc(Motion);   // �巡�� �̺�Ʈ
	glutMainLoop(); //--- �̺�Ʈ ó�� ����
	

}

//--- �簢�� �׸��� �Լ�
void DrawRect(const Rect& rect) 
{
	glColor3f(rect.r, rect.g, rect.b);
	glBegin(GL_QUADS);
	glVertex2f(rect.x1, rect.y1);
	glVertex2f(rect.x2, rect.y1);
	glVertex2f(rect.x2, rect.y2);
	glVertex2f(rect.x1, rect.y2);
	glEnd();
}

//--- ȭ�� ���
GLvoid drawScene() 
{
	//--- ����� ���� ����
	glClearColor(Bgcolor[0],Bgcolor[1],Bgcolor[2], 1.0f);//--- �������� ����
	glClear(GL_COLOR_BUFFER_BIT); //--- ������ ������ ��ü�� ĥ�ϱ�
	
	for(const auto& r : rects) {
		DrawRect(r);
	}

	glutSwapBuffers(); //--- ȭ�鿡 ���
}

//--- ������ ũ�� ����� ��ǥ�� �缳��
GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);          // ������ ��ü�� �׸��� �������� ����
	glMatrixMode(GL_PROJECTION);     // ���� ��� ���� ��ȯ
	glLoadIdentity();                // ���� ���� �ʱ�ȭ
	gluOrtho2D(0, w, 0, h);          // 2D ��ǥ�� ���� (0~w, 0~h)
	glMatrixMode(GL_MODELVIEW);      // �ٽ� �𵨺� ��� ���� ��ȯ
}

//--- Ű���� �Է� ó��
GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'a':AddRandomRect(800, 600); break;
	case 'q':exit(0); break; //--- ���α׷� ����
	}
	glutPostRedisplay(); //--- ������ �ٲ� ������ ��� �ݹ� �Լ��� ȣ���Ͽ� ȭ���� refresh �Ѵ�
}

GLvoid AddRandomRect(int winW, int winH)
{
	Rect newRect;

	float w = rand() % 100 + 20; //--- �簢���� �ʺ�
	float h = rand() % 100 + 20; //--- �簢���� ����

	//--- ��ġ
	newRect.x1 = rand() % (winW - (int)w);
	newRect.y1 = rand() % (winH - (int)h);
	newRect.x2 = newRect.x1 + w;
	newRect.y2 = newRect.y1 + h;

	//--- ����
	newRect.r = (float)(rand() % 100) / 100.0f;
	newRect.g = (float)(rand() % 100) / 100.0f;
	newRect.b = (float)(rand() % 100) / 100.0f;

	newRect.isSelected = false;

	//--- 30�� ��������
	if(rects.size()<30)
		rects.push_back(newRect);
}

//--- ���콺 Ŭ��
GLvoid Mouse(int button, int state, int x, int y)
{
    int winH = glutGet(GLUT_WINDOW_HEIGHT);
    int mouseY = winH - y; // OpenGL ��ǥ��� ��ȯ

    // ==========================
    // ���� ��ư: ���� & ��ġ��
    // ==========================
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        selectedRectIndex = -1;
        for (int i = rects.size() - 1; i >= 0; i--) { // �ڿ������� �˻� �� ���� �ִ� �簢�� �켱
            Rect& r = rects[i];
            if (x >= r.x1 && x <= r.x2 && mouseY >= r.y1 && mouseY <= r.y2) {
                selectedRectIndex = i;
                r.isSelected = true;
                prevMouseX = x;
                prevMouseY = mouseY;

                // ���õ� �簢���� �� �ڷ� �̵� (�׻� ���� ���̵���)
                Rect picked = r;
                rects.erase(rects.begin() + i);
                rects.push_back(picked);
                selectedRectIndex = rects.size() - 1;

                break;
            }
            else {
                r.isSelected = false;
            }
        }
    }
    else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        if (selectedRectIndex != -1) {
            Rect moved = rects[selectedRectIndex];

            // �ٸ� �簢���� ��ġ���� Ȯ�� �� ��ġ��
            for (int i = 0; i < rects.size(); i++) {
                if (i == selectedRectIndex) continue;
                if (!(moved.x2 < rects[i].x1 || moved.x1 > rects[i].x2 ||
                    moved.y2 < rects[i].y1 || moved.y1 > rects[i].y2)) {
                    // ��ħ �� ��ġ��
                    Rect merged;
                    merged.x1 = min(moved.x1, rects[i].x1);
                    merged.y1 = min(moved.y1, rects[i].y1);
                    merged.x2 = max(moved.x2, rects[i].x2);
                    merged.y2 = max(moved.y2, rects[i].y2);
                    merged.r = (float)(rand() % 100) / 100.0f;
                    merged.g = (float)(rand() % 100) / 100.0f;
                    merged.b = (float)(rand() % 100) / 100.0f;
                    merged.isSelected = false;

                    // ���� �� �簢�� ����
                    if (i > selectedRectIndex) {
                        rects.erase(rects.begin() + i);
                        rects.erase(rects.begin() + selectedRectIndex);
                    }
                    else {
                        rects.erase(rects.begin() + selectedRectIndex);
                        rects.erase(rects.begin() + i);
                    }

                    // ������ �簢�� �߰�
                    rects.push_back(merged);
                    break;
                }
            }
        }

        // �巡�� ����
        selectedRectIndex = -1;
        prevMouseX = -1;
        prevMouseY = -1;
        for (auto& r : rects) r.isSelected = false;
    }

    // ==========================
    // ������ ��ư: �и��ϱ�
    // ==========================
    else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        int targetIndex = -1;
        for (int i = rects.size() - 1; i >= 0; i--) {
            const Rect& r = rects[i];
            if (x >= r.x1 && x <= r.x2 && mouseY >= r.y1 && mouseY <= r.y2) {
                targetIndex = i;
                break;
            }
        }

        if (targetIndex != -1) {
            Rect target = rects[targetIndex];
            rects.erase(rects.begin() + targetIndex);

            // �и� (���� �Ǵ� ���� ����)
            bool splitVertically = rand() % 2;
            if (splitVertically) {
                float midX = (target.x1 + target.x2) / 2.0f;

                Rect left = { target.x1, target.y1, midX, target.y2,
                    (float)(rand() % 100) / 100.0f,
                    (float)(rand() % 100) / 100.0f,
                    (float)(rand() % 100) / 100.0f,
                    false };

                Rect right = { midX, target.y1, target.x2, target.y2,
                    (float)(rand() % 100) / 100.0f,
                    (float)(rand() % 100) / 100.0f,
                    (float)(rand() % 100) / 100.0f,
                    false };

                if (rects.size() < 30) rects.push_back(left);
                if (rects.size() < 30) rects.push_back(right);
            }
            else {
                float midY = (target.y1 + target.y2) / 2.0f;

                Rect bottom = { target.x1, target.y1, target.x2, midY,
                    (float)(rand() % 100) / 100.0f,
                    (float)(rand() % 100) / 100.0f,
                    (float)(rand() % 100) / 100.0f,
                    false };

                Rect top = { target.x1, midY, target.x2, target.y2,
                    (float)(rand() % 100) / 100.0f,
                    (float)(rand() % 100) / 100.0f,
                    (float)(rand() % 100) / 100.0f,
                    false };

                if (rects.size() < 30) rects.push_back(bottom);
                if (rects.size() < 30) rects.push_back(top);
            }
        }
    }

    glutPostRedisplay();
}


//--- ���콺 �巡�� ó��
GLvoid Motion(int x, int y) {
	if(selectedRectIndex != -1) {
		int winH = glutGet(GLUT_WINDOW_HEIGHT);
		int mouseY = winH - y; // OpenGL ��ǥ�迡 �°� ��ȯ
		int dx = x - prevMouseX;
		int dy = mouseY - prevMouseY;
		Rect& r = rects[selectedRectIndex];
		r.x1 += dx;
		r.x2 += dx;
		r.y1 += dy;
		r.y2 += dy;
		prevMouseX = x;
		prevMouseY = mouseY;
		glutPostRedisplay();
	}
}

Rect mergeRect(const Rect& r1, const Rect& r2)
{
	Rect merged;

	merged.x1 = min(r1.x1, r2.x1);
	merged.y1 = min(r1.y1, r2.y1);
	merged.x2 = max(r1.x2, r2.x2);
	merged.y2 = max(r1.y2, r2.y2);

	merged.r = (float)(rand() % 100) / 100.0f;
	merged.g = (float)(rand() % 100) / 100.0f;
	merged.b = (float)(rand() % 100) / 100.0f;
	merged.isSelected = false;

	return merged;
}

vector<Rect> splitRect(const Rect& r)
{
	vector<Rect> result;
	bool splitvertically = rand() % 2;

	if (splitvertically) {
		float midX = (r.x1 + r.x2) / 2.0f;

		Rect r1 = { r.x1, r.y1, midX, r.y2, (float)(rand() % 100) / 100.0f, (float)(rand() % 100) / 100.0f, (float)(rand() % 100) / 100.0f, false };
		Rect r2 = { midX, r.y1, r.x2, r.y2, (float)(rand() % 100) / 100.0f, (float)(rand() % 100) / 100.0f, (float)(rand() % 100) / 100.0f, false };
		result.push_back(r1);
		result.push_back(r2);

	}
	else {
		float midY = (r.y1 + r.y2) / 2.0f;

		Rect r3 = { r.x1, r.y1, r.x2, midY, (float)(rand() % 100) / 100.0f, (float)(rand() % 100) / 100.0f, (float)(rand() % 100) / 100.0f, false };
		Rect r4 = { r.x1, midY, r.x2, r.y2, (float)(rand() % 100) / 100.0f, (float)(rand() % 100) / 100.0f, (float)(rand() % 100) / 100.0f, false };
		result.push_back(r3);
		result.push_back(r4);
	}
	return result;
}
