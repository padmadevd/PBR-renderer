#version 330 core

layout (location=0) in vec3 vPos;
out vec3 localPos;

uniform mat4 proj;
uniform mat4 view;

void main(){
	
	localPos = vPos;
	gl_Position = proj*view*vec4(vPos, 1.0f);
}