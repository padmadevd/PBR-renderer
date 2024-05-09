#version 330 core

layout (location = 0) in vec3 v_pos;

uniform mat4 view;
uniform mat4 proj;

out vec3 dir;

void main(){

	dir = normalize(v_pos);
	gl_Position = proj*mat4(mat3(view))*vec4(v_pos, 1.0);
}