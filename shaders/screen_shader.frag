#version 330 core
in vec2 TexCoords;

uniform sampler2D screenTexture;

void main()
{
    vec3 col = texture(screenTexture, TexCoords).rgb;
    gl_FragColor = vec4(col, 1.0);
} 