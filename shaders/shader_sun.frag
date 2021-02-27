#version 430 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

uniform vec3 lightPos;
uniform vec3 cameraPos;
uniform sampler2D tex;

in vec3 interpNormal;
in vec3 vecPos;
in vec2 TexCoords;


void main()
{
    vec3 objectColor = texture(tex, TexCoords).rgb;
	vec3 lightDir = normalize(vecPos - lightPos);
	vec3 view = normalize(cameraPos - vecPos);
	float dif = max(dot(-lightDir,normalize(interpNormal)),0.0);
	vec3 ref = reflect(lightDir,normalize(interpNormal));
	float spec = pow(max(0,dot(ref,view)), 50);
	vec3 result = vec3(objectColor * dif + vec3(1.0) * spec);
 
    BrightColor = vec4(objectColor, 1.0);;
	FragColor = vec4(objectColor, 1.0);
}
