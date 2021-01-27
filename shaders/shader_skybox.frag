#version 430 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;


uniform vec3 objectColor;
uniform vec3 lightPos;
uniform sampler2D colorTexture;
uniform vec3 cameraPos;


in vec3 interpNormal;
in vec3 vecPos;
in vec2 texCoord;




void main()
{
	vec4 textureColor = texture2D(colorTexture,texCoord);
	vec3 result = vec3(textureColor.x,textureColor.y,textureColor.z);
	FragColor = vec4(result, 1.0);
	BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}
