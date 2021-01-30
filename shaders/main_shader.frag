#version 430 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

uniform vec3 objectColor;
uniform vec3 lightPos;
uniform vec3 lightPos2;
uniform vec3 cameraPos;
uniform sampler2D diffuseTexture;

in vec3 interpNormal;
in vec3 vecPos;

vec3 lp;

void main()
{
	vec3 lighting = vec3(0.0);

    for(int i = 0; i < 1; i++)
    {
		if(i == 0){
			lp = lightPos;
		}
		else{
			lp = lightPos2;
		}
		
		vec3 lightDir = normalize(vecPos - lp);
		vec3 view = normalize(cameraPos - vecPos);
		float dif = max(dot(-lightDir,normalize(interpNormal)),0.0);
		vec3 ref = reflect(lightDir,normalize(interpNormal));
		float spec = pow(max(0,dot(ref,view)), 50);
		lighting = dif + vec3(1.0) * spec;

	}
	vec3 result = vec3(objectColor * lighting);

    float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        BrightColor = vec4(result, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);

	FragColor = vec4(result, 1.0);;
}
