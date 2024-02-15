#version 430 core
// 410 for mac

layout(location=0) in vec3 position;

uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projMat;


void main()
{
    vec4 pos = vec4(position, 1.0);
    vec4 viewPos = viewMat * modelMat * pos;
    vec4 projPos = projMat * viewPos;
    gl_Position = projPos;
}
