#version 330 core

in vec3 dir;
out vec4 FragColor;

uniform samplerCube env;


const mat3 aces_input_matrix = mat3(
    vec3(0.59719f, 0.35458f, 0.04823f),
    vec3(0.07600f, 0.90834f, 0.01566f),
    vec3(0.02840f, 0.13383f, 0.83777f)
);

const mat3 aces_output_matrix = mat3(
    vec3( 1.60475f, -0.53108f, -0.07367f),
    vec3(-0.10208f,  1.10813f, -0.00605f),
    vec3(-0.00327f, -0.07276f,  1.07602f)
);


vec3 rtt_and_odt_fit(vec3 v)
{
    vec3 a = v * (v + vec3(0.0245786f)) - vec3(0.000090537f);
    vec3 b = v * (0.983729f * v + vec3(0.4329510f)) + vec3(0.238081f);
    return a / b;
}

vec3 aces_fitted(vec3 v)
{
    v = aces_input_matrix*v;
    v = rtt_and_odt_fit(v);
    return aces_output_matrix*v;
}

vec3 reinhard_extended(vec3 v, float max_white)
{
    vec3 numerator = v * (vec3(1.0f) + (v / vec3(max_white * max_white)));
    return numerator / (vec3(1.0f) + v);
}

void main(){
	
	vec3 envColor = texture(env, dir).rgb;
	envColor = reinhard_extended(envColor, 4.0);
	envColor = pow(envColor, vec3(1.0/2.2));

	FragColor = vec4(envColor, 1.0f);
}
