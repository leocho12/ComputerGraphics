#version 330 compatibility
layout(location = 0) in vec2 aPos;   // glVertex2f가 넘기는 위치(일반적으로 0번 속성)
void main() {
    gl_Position = vec4(aPos, 0.0, 1.0);
}
