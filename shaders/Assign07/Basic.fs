#version 430 core
// Change to 410 for macOS

layout(location=0) out vec4 out_color;
 
in vec4 vertexColor; // Now interpolated across face
in vec4 interPos;
in vec3 interNormal;

struct PointLight
{
	vec4 pos;
	vec4 color;
};

uniform PointLight light;
uniform float metallic;
uniform float roughness;
#define PI 3.14159265359

vec3 getFresnelAtAngleZero(vec3 albedo, float metallic)
{
	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);
	return F0;
}

vec3 getFresnel(vec3 F0, vec3 L, vec3 H)
{
	float cosAngle = max(0, dot(L,H));
	vec3 schlick = F0 + (1-F0) * pow(1-max(0, cosAngle), 5);
	return schlick;
	//COS(THETA) = DOT(H,L)
}

float getNDF(vec3 H, vec3 N, float roughness)
{
	float a = pow(roughness, 2);
	float denominatorLeft  = pow(dot(N,H), 2);
	float denominatorRight = (pow(a,2)-1);
	float denominator = PI * pow(denominatorLeft * denominatorRight + 1, 2);
	return pow(a,2)/denominator;
}

float getSchlickGeo(vec3 B, vec3 N, float roughness)
{
	float k = pow(roughness + 1, 2) / 8;
	return (dot(N, B) / (dot(N, B) * (1 - k) + k));
}

float getGF(vec3 L, vec3 V, vec3 N, float roughness)
{
	float GL = getSchlickGeo(L, N, roughness);
	float GV = getSchlickGeo(V, N, roughness);
	return GL * GV;
}


void main()
{
	/*
	vec3 N = vec3(normalize(interNormal));
	vec3 L = normalize(vec3(light.pos-interPos));
	vec3 V = normalize(-1 * vec3(interPos));
	vec3 H = normalize(V + L);	//distance from intersection to eye = -interPos
	float diffuseCoefficient = max(0, dot(N,L));
	vec3 diffColor = vec3(vertexColor * light.color * diffuseCoefficient);
	float shininess = 10.0;
	float preSpec = max(0, dot(N, H));
	float specularCoefficient = diffuseCoefficient * pow(preSpec, shininess);
	vec3 specularColor = vec3(1.0,1.0,1.0) * vec3(light.color) * specularCoefficient;

	// Just output interpolated color
	out_color = vec4(vec3(diffColor + specularColor ), 1.0);
	*/
	vec3 N = vec3(normalize(interNormal));
	vec3 L = normalize(vec3(light.pos-interPos));
	vec3 V = normalize(-1 * vec3(interPos));
	vec3 F0 = getFresnelAtAngleZero(vec3(vertexColor), metallic);
	vec3 H = normalize(V+L);
	vec3 F = getFresnel(F0, L, H);
	vec3 kS = F;
		vec3 kD = 1.0 - kS;
		kD *= (1.0 - metallic);
		kD *= vec3(vertexColor);
		kD = kD / PI;
	float NDF = getNDF(H, N, roughness);
	float G = getGF(L, V, N, roughness);
	kS = kS * NDF * G;
	kS = kS / (4.0 * max(0, dot(N, L)) * max(0, dot(N, V))) + 0.0001;
	vec3 finalColor = (kD + kS) * vec3(light.color) * max(0, dot(N,L));
	out_color = vec4(finalColor, 1.0);
}
