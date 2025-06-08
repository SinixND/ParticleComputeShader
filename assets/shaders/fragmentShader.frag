// https://raw.githubusercontent.com/raysan5/raylib/master/examples/shaders/resources/shaders/glsl330/base.fs

// FRAGMENT SHADER
// := **How** things are drawn
// Adds colors to pixels

// Required
// GLSL version
#version 330

// Input vertex attributes (FROM VERTEX SHADER)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform (const?) values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

// NOTE: Add here your custom variables

void main()
{
    // Texel color fetching from texture sampler
    vec4 texelColor = texture(texture0, fragTexCoord);

    // NOTE: Implement here your fragment shader code

    finalColor = texelColor * colDiffuse;
}
