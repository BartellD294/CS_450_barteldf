#version 430 core
// Change to 410 for macOS

layout(location=0) out vec4 out_color;
 
in vec4 vertexColor; // Now interpolated across face
in vec4 interPos;
in vec4 interNormal;

struct PointLight
{
	vec4 pos;
	vec4 color;
};

uniform PointLight light;

void main()
{
	vec4 N = normalize(interNormal);
	vec3 L = light
	// Just output interpolated color
	out_color = vertexColor;
}
