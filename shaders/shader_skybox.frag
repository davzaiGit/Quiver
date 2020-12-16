#version 430 core

uniform vec3 objectColor;

uniform vec3 lightPos;

uniform sampler2D colorTexture;

in vec3 interpNormal;

in vec3 vecPos;

in vec2 texCoord;

uniform vec3 cameraPos;



void main()
{
	vec4 textureColor = texture2D(colorTexture,texCoord);
	vec3 result = vec3(textureColor.x,textureColor.y,textureColor.z);
	gl_FragColor = vec4(result, 1.0);
}
