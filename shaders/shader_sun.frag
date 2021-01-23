#version 430 core

uniform vec3 objectColor;

uniform vec3 lightPos;
uniform sampler2D colorBuffer;
uniform sampler2D hdrBuffer;
uniform vec3 cameraPos;

in vec3 interpNormal;
in vec3 vecPos;
in vec2 texCoord;

vec2 blurTexCoords[11];


void main()
{
	vec4 textureColor = texture2D(hdrBuffer,texCoord);
	vec3 result = vec3(textureColor.x,textureColor.y,textureColor.z);
	gl_FragColor = vec4(result, 1.0);

}

/*
    float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);
	vec2 tex_offset = 1.0 / textureSize(hdrBuffer, 0); // gets size of single texel
    vec3 result = texture(hdrBuffer, texCoord).rgb * weight[0]; // current fragment's contribution
    for(int i = 1; i < 5; ++i)
    {
        result += texture(hdrBuffer, texCoord + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
        result += texture(hdrBuffer, texCoord - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
    }
    for(int i = 1; i < 5; ++i)
    {
        result += texture(hdrBuffer, texCoord + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
        result += texture(hdrBuffer, texCoord - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
    }
	gl_FragColor=vec4(result + texture(colorBuffer,texCoord).rgb,1.0);
*/

