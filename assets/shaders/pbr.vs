#version 330 core

layout (location = 0) in vec3 v_pos;
layout (location = 1) in vec3 v_nor;
layout (location = 2) in vec3 v_Ntangent;
layout (location = 3) in vec2 v_tc;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec3 fragPos;
out vec3 fragNor;
out vec2 fragTc;
out mat3 TBN;

void main(){

	fragPos = vec3(model*vec4(v_pos, 1.0));
	//mat3 Nmodel = mat3(transpose(inverse(model)));
	//vec3 N = normalize(Nmodel*v_nor);
	//fragNor = N;
	vec3 T = normalize(vec3(model * vec4(v_Ntangent, 0.0)));
	vec3 N = normalize(vec3(model * vec4(v_nor, 0.0)));
	// re-orthogonalize T with respect to N
	T = normalize(T - dot(T, N) * N);
	// then retrieve perpendicular vector B with the cross product of T and N
	vec3 B = cross(N, T);

	TBN = mat3(T, B, N);

	fragTc = v_tc;
	gl_Position = proj*view*model*vec4(v_pos, 1.0);
}