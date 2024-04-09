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
	vec3 H = normalize(vec3(interPos) + L);
	float diffuseCoefficient = max(0, dot(vec3(N),L));
	vec3 diffColor = vec3(diffuseCoefficient * vertexColor * light.color);
	float shininess = 10.0;
	float specularCoefficient = diffuseCoefficient * pow(max(0, dot(vec3(N),H)), shininess);
	vec3 specularColor = vec3(vec3(1.0,1.0,1.0) * vec3(light.color) * specularCoefficient);

	// Just output interpolated color
	out_color = vec4(vec3(H ), 1.0);

}
