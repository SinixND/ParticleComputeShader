//* https://raw.githubusercontent.com/raysan5/raylib/master/examples/shaders/resources/shaders/glsl330/base.vs

//* VERTEX SHADER
//* := **Where** things are drawn
//* Transforms verticies ("edges")

//* VAO (vertex array object)
//* Stores where to find the data for each input (vertex) attribute

//* VBO (vertex buffer object)
//* This is an object that holds/buffers verticies to be passed to the vertex shader

//* VAO is needed BEFORE VBO!

//* === SHADER START ===
//* Required GLSL version
#version 330 core

//* Input vertex attributes
//* in type nameIn;
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
in vec4 vertexColor;

//* Input uniform (const?) values
uniform mat4 mvp;

//* Output vertex attributes (to fragment shader)
//* out type nameOut;
out vec2 fragTexCoord;
out vec4 fragColor;

//* NOTE: Add your custom variables here

void main()
{
    //* Output vertex attributes to fragment shader
    fragTexCoord = vertexTexCoord;
    fragColor = vertexColor;

    //* Reqired
    //* Calculate final vertex position
    gl_POSITION_ = mvp*vec4(vertexPosition, 1.0);
}

// vim: set filetype=vert :
