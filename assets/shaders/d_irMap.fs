#version 330 core

out vec3 FragColor;
in vec3 texDir;

uniform samplerCube hdriMap;

const float PI = 3.14159265359;

void main(){
	
	vec3 normal = normalize(texDir);

	vec3 irradiance = vec3(0.0);
	vec3 up = vec3(0.0, 1.0, 0.0);
	vec3 right = cross(up, normal);
	up = cross(normal, right);
	float sampleDelta = 0.025;
	float nrSamples = 0.0;
	for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta){

		for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta){

			vec3 tangentSample = vec3(sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta));

			vec3 sampleVec = tangentSample.x*right + tangentSample.y*up + tangentSample.z*normal;

			vec3 color = textureLod(hdriMap, sampleVec, 5).rgb;
			//float luminance = max(dot(color, vec3(0.2126f, 0.7152f, 0.0722f)), 0.0);
			//color = pow(20, luminance) * color;

			irradiance += color * cos(theta) * sin(theta);

			nrSamples++;
		}
	}

	irradiance = PI * irradiance * (1.0 / float(nrSamples));
	FragColor = irradiance;
}