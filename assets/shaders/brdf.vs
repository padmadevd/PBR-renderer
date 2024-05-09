#version 330 core

layout (location=0) in vec3 vPos;

uniform mat4 proj;
uniform mat4 view;

out vec2 texCoords;

void main(){

	vec4 fragPos = proj*view*vec4(vPos, 1.0f);
	texCoords = fragPos.rg;
	gl_Position = fragPos;
}