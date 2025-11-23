#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aCol;

uniform mat4 modelTransform;
uniform mat4 viewTransform;
uniform mat4 projectionTransform;

out vec3 vColor;

void main()
{
    gl_Position = projectionTransform * viewTransform * modelTransform * vec4(aPos, 1.0);
    vColor = aCol;   // 실제 색은 fragment에서 uColor로 대체될 예정
}
