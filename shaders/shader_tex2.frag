#version 430 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

uniform sampler2D textureSampler;
uniform sampler2D normalSampler;

in vec3 lightDirTS;
in vec3 lightDirTS2;
in vec2 interpTexCoord;
in vec3 viewDirTS;

void main()
{
	vec3 L = normalize(-lightDirTS);
	vec3 V = normalize(viewDirTS);
	vec3 N = vec3(0, 0, 1);
	N = texture2D(normalSampler, interpTexCoord).rgb;
	N = normalize(N * 2 - 1);
	vec3 R = reflect(-normalize(L), N);

	float diffuse = max(0, dot(N, L));

	float specular_pow = 10;
	float specular = pow(max(0, dot(R, V)), specular_pow);

	vec3 L2 = normalize(-lightDirTS2);
	vec3 V2 = normalize(viewDirTS);
	vec3 N2 = vec3(0, 0, 1);
	N2 = texture2D(normalSampler, interpTexCoord).rgb;
	N2 = normalize(N * 2 - 1);
	vec3 R2 = reflect(-normalize(L), N);

	float diffuse2 = max(0, dot(N, L));

	float specular_pow2 = 10;
	float specular2 = pow(max(0, dot(R, V)), specular_pow);

	vec3 color = texture2D(textureSampler, interpTexCoord).rgb;

	vec3 lightColor = vec3(1);
	vec3 shadedColor = color * diffuse + lightColor * specular * diffuse2 * specular2;

	float ambient = 0.2;
	FragColor = vec4(mix(color, shadedColor, 1.0 - ambient), 1.0);
	BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}
