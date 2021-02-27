#version 430 core

uniform vec3 objectColor;
uniform vec3 lightPos;
uniform vec3 cameraPos;

in vec3 interpNormal;
in vec3 vecPos;



void main()
{
	vec3 lightDir = normalize(vecPos - lightPos);
	vec3 view = normalize(cameraPos - vecPos);
	float dif = max(dot(-lightDir,normalize(interpNormal)),0.0);
	vec3 ref = reflect(lightDir,normalize(interpNormal));
	float spec = pow(max(0,dot(ref,view)), 50);
	double result = dot(interpNormal,-lightDir);
	gl_FragColor = vec4(objectColor * dif + vec3(1.0) * spec, 1.0);
}
