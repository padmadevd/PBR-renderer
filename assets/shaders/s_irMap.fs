#version 330 core

out vec3 FragColor;
in vec3 texDir;

uniform float roughness;
uniform samplerCube hdriMap;

const float M_PI = 3.14159265359;

float radicalInverse_VdC(uint bits) {
	bits = (bits << 16u) | (bits >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
	return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

vec2 Hammersley(uint i, uint N) {
	return vec2(float(i) / float(N), radicalInverse_VdC(i));
}

float DistributionGGX(float NdotH, float roughness4) {

	float NdotH2 = NdotH * NdotH;
	float denom = (NdotH2 * (roughness4 - 1.0) + 1.0);
	denom = M_PI * denom * denom;

	return roughness4 / denom;
}


vec3 ImportanceSampleGGX(vec2 xi, float roughness4) {

	// Compute distribution direction
	float Phi = 2.0 * M_PI * xi.x;
	float CosTheta = sqrt((1.0 - xi.y) / (1.0 + (roughness4 - 1.0) * xi.y));
	float SinTheta = sqrt(1.0 - CosTheta * CosTheta);

	// Convert to spherical direction
	vec3 H;
	H.x = SinTheta * cos(Phi);
	H.y = SinTheta * sin(Phi);
	H.z = CosTheta;

	return H;
}

void main(){

	vec3 N = normalize(texDir);

	vec4 sum = vec4(0.0, 0.0, 0.0, 0.0);

	float resolution = 512.0;
	float solid_angle_texel = 4.0 * M_PI / (6.0 * resolution * resolution);
	float roughness2 = roughness * roughness;
	float roughness4 = roughness2 * roughness2;
	vec3 UpVector = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
	mat3 T;
	T[0] = normalize(cross(UpVector, N));
	T[1] = cross(N, T[0]);
	T[2] = N;

	uint SAMPLE_COUNT = 1024u;

	for (uint sampleNum = 0u; sampleNum < SAMPLE_COUNT; sampleNum++) {

		vec2 xi = Hammersley(sampleNum, SAMPLE_COUNT);

		vec3 H = T * ImportanceSampleGGX(xi, roughness4);
		float NdotH = dot(N, H);
		vec3 L = (2.0 * NdotH * H - N);

		float ndotl = clamp(dot(N, L), 0.0, 1.0);

		if (ndotl > 0.0) {

			float D = DistributionGGX(NdotH, roughness4);
			float pdf = D * NdotH / (4.0 * NdotH) + 0.0001;

			float solid_angle_sample = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.0001);

			float mipLevel = roughness == 0.0 ? 0.0 : 0.5 * log2(solid_angle_sample / solid_angle_texel);

			vec3 color = textureLod(hdriMap, L, mipLevel).rgb;
			//float luminance = max(dot(color, vec3(0.2126f, 0.7152f, 0.0722f)), 0.0);
			//color = pow(20, luminance) * color;

			sum.rgb += color * ndotl;
			sum.a += ndotl;
		}
	}
	sum /= sum.a;

	FragColor = sum.rgb;
}