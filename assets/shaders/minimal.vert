#version 430 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec4 color;

out vec4 vertexColor;

void main()
{
    gl_PointSize = 5.0f;

    gl_Position = vec4(position, 0.0, 1.0);

    vertexColor = color;
}
