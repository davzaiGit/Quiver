#version 430 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

uniform sampler2D colorTexture;
uniform vec3 lightPos;
uniform vec3 lightPos2;
uniform vec3 cameraPos;

uniform vec3 position5;  
  
 vec3 ambient5;
 vec3 diffuse5;
 vec3 specular5;
	
uniform float constant5;
uniform float linear5;
uniform float quadratic5;

in vec3 interpNormal;
in vec3 vecPos;
in vec2 vertexTexC;

vec3 lp;

void main()
{
	vec3 lighting = vec3(0.0);

    for(int i = 0; i <= 1; i++)
    {
		if(i == 0){
			lp = lightPos;
			}
		else{
			lp = lightPos2;
			// attenuation
			float distance = length(position5 - vecPos);
			float attenuation = 1.0 / (constant5 + linear5 * distance + quadratic5 * (distance * distance));  
		}

	vec3 lightDir = normalize(lp - vecPos);
	vec3 V = normalize(cameraPos-vecPos);
	vec3 normal = normalize(interpNormal);
	vec3 R = reflect(-lightDir,normal);
	
	float specular = pow(max(0,dot(R,V)),10);
	float diffuse = max(0,dot(normal,lightDir));
	float ambient = 0.1;
	if(i == 0) {
			lighting = diffuse + vec3(1) * specular, 1.0 - ambient;
		}
	else{
			// attenuation
			float distance = length(position5 - vecPos);
			float attenuation = 1.0 / (constant5 + linear5 * distance + quadratic5 * (distance * distance));  
			lighting += attenuation;
		}
	}
	vec2 vertexTexC_fixed = vec2(vertexTexC.x, 1 - vertexTexC.y);
	vec4 textureColor = texture2D(colorTexture, vertexTexC_fixed);

	FragColor = vec4(textureColor.xyz * lighting, 1.0);

	BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}
