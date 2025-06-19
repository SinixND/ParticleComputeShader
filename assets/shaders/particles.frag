#version 430

in vec4 vertexColor;

// Output fragment color
out vec4 finalColor;

void main()
{
    finalColor = vertexColor;
}
