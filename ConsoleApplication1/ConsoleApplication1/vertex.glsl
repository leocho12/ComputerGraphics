#version 330 compatibility
layout(location = 0) in vec2 aPos;   // glVertex2f�� �ѱ�� ��ġ(�Ϲ������� 0�� �Ӽ�)
void main() {
    gl_Position = vec4(aPos, 0.0, 1.0);
}
