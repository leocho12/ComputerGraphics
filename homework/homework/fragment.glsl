#version 330 core

in vec3 vColor;

uniform vec3 uColor;   // ★ C++에서 넘겨주는 색

out vec4 FragColor;

void main()
{
    FragColor = vec4(uColor, 1.0);   // 오직 C++이 보낸 랜덤 색으로 렌더링
}
