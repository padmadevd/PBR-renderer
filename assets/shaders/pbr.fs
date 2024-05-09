#version 330 core

out vec4 FragColor;

uniform samplerCube hdriMap;
uniform samplerCube d_irMap;
uniform samplerCube s_irMap;
uniform sampler2D brdfLUT;

uniform sampler2D albedo_tex;
//uniform sampler2D spec_tex;
uniform sampler2D normal_tex;
uniform sampler2D metal_tex;
uniform vec3 m_channel;
uniform sampler2D rough_tex;
uniform vec3 r_channel;
uniform sampler2D ao_tex;
uniform vec3 ao_channel;

//uniform float metalness;
//uniform float roughness;

in vec3 fragPos;
in vec3 fragNor;
in vec2 fragTc;
in mat3 TBN;

uniform int albedo_b;
uniform int spec_b;
uniform int normal_b;
uniform int metal_b;
uniform int rough_b;
uniform int ao_b;

uniform vec3 camPos;

vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness){
	
	return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

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

	vec3 albedo = vec3(1.0);
	if(albedo_b == 1){

		albedo = texture(albedo_tex, fragTc).rgb;
		albedo = pow(albedo, vec3(2.2));
	}

	float roughness = 0.2;
	if(rough_b == 1)
		roughness = dot(texture(rough_tex, fragTc).rgb, r_channel);

	float metalness = 0.0;
	if(metal_b == 1)
		metalness = dot(texture(metal_tex, fragTc).rgb, m_channel);

	vec3 normal = vec3(0.0, 0.0, 1.0);
	if(normal_b == 1){

		normal = texture(normal_tex, fragTc).rgb;
		normal = normal * 2.0 - 1.0; 
	}

	float ao = 1.0;
	if(ao_b == 1)
		ao = dot(texture(ao_tex, fragTc).rgb, ao_channel);

	vec3 N = normalize(TBN*normal);
	vec3 V = normalize(camPos - fragPos);
	vec3 R = reflect(-V, N);

	//roughness = 0;

	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metalness);
	vec3 F = FresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
	vec3 kD = 1.0 - F;
	kD *= 1.0 - metalness;
	vec3 irradiance = texture(d_irMap, N).rgb;
	vec3 diffuse =  kD * irradiance;
	const float MAX_REFLECTION_LOD = 4.0;
	vec3 prefilteredColor = textureLod(s_irMap, R, roughness * MAX_REFLECTION_LOD).rgb;
	vec2 envBRDF = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
	vec3 specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);

	vec3 ambient = (diffuse*albedo + specular);

	//vec3 color = vec3(envBRDF.x);
	vec3 color = ambient;

	color = reinhard_extended(color, 4.0f);
	color = pow(color, vec3(1.0/2.2));

	FragColor = vec4(color, 1.0);
}
