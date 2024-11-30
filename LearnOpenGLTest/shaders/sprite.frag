#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D image;
uniform vec3 spriteColor;
uniform vec2 texRepeat;
uniform vec2 texOffset;

void main()
{    
    color = vec4(spriteColor, 1.0) * texture(image, vec2(TexCoords.x * texRepeat.x + texOffset.x, TexCoords.y * texRepeat.y + texOffset.y));
}  