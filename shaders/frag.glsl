#version 460
out vec4 FragColor;
  

in vec2 TexCoord;
in vec3 Color;
uniform sampler2D blockTexture;

void main()
{
    //FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    FragColor = texture2D(blockTexture, TexCoord) * vec4(Color, 1.0f);
}