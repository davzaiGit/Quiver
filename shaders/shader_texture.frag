#version 430 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

uniform sampler2D colorTexture;
uniform vec3 lightPos;
uniform vec3 cameraPos;

in vec3 interpNormal;
in vec3 vecPos;
in vec2 vertexTexC;

void main()
{
	vec3 lightDir = normalize(lightPos - vecPos);
	vec3 V = normalize(cameraPos-vecPos);
	vec3 normal = normalize(interpNormal);
	vec3 R = reflect(-lightDir,normal);
	
	float specular = pow(max(0,dot(R,V)),10);
	float diffuse = max(0,dot(normal,lightDir));
	float ambient = 0.1;

	vec2 vertexTexC_fixed = vec2(vertexTexC.x, 1 - vertexTexC.y);
	vec4 textureColor = texture2D(colorTexture, vertexTexC_fixed);

	FragColor = vec4(mix(textureColor.xyz, textureColor.xyz * diffuse + vec3(1) * specular, 1.0 - ambient),1.0);
	BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}
