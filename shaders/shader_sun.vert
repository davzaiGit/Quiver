#version 430 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTexCoord;
layout(location = 2) in vec3 vertexNormal;
//standard
uniform mat4 transformation;
uniform mat4 model;

out vec3 interpNormal;
out vec3 vecPos;
out vec2 texCoord;

//blur
//out vec2 y_blurTexCoords[11];
//out vec2 x_blurTexCoords[11];

float target = 20;
vec2 blurTexCoords[11];

void main()
{
	gl_Position = transformation * vec4(vertexPosition, 1.0);
	interpNormal = vec3(model * vec4(vertexNormal,0.0f));
	vecPos = vec3(model * vec4(vertexPosition,1.0f));
	texCoord = vec2(vertexTexCoord.x,1-vertexTexCoord.y);
}

/*
    gl_Position = transformation * vec4(vertexPosition, 1.0);
    vec3 centerTexCoords = vertexPosition * 0.5 + 0.5;
    texCoord = vec2(vertexTexCoord.x,1-vertexTexCoord.y);
    vecPos = vec3(model * vec4(vertexPosition,1.0f));
    interpNormal = vec3(model * vec4(vertexNormal,0.0f));

    
    float pixSize = 1.0 / target;

    //horizontal
    for(int i=-5; i<=5; i++){
        blurTexCoords[i+5] = vec2(centerTexCoords.x + pixSize * i, centerTexCoords.y + pixSize * i);
    }
    /vertical
    for(int i=-5; i<=5; i++){
        y_blurTexCoords[i+5] = centerTexCoords + vec2(0.0, pixSize * i);
    }
*/

	