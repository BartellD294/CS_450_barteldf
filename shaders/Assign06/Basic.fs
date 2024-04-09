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
	vec3 L = normalize(vec3(light.pos-interPos));
	float diffuseCoefficient = max(0, dot(vec3(N),L));
	vec3 diffColor = vec3(diffuseCoefficient * vertexColor * light.color);
	float shininess = 10.0;
	//float specularCoefficient = vec3(vec3(1.0,1.0,1.0) * diffuseCoefficient * pow(max));
	// Just output interpolated color
	out_color = vec4(diffColor, 1.0);
}
