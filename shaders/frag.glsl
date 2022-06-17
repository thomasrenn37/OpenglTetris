#version 460
out vec4 FragColor;
  
//in vec2 ourColor;
in vec2 TexCoord;
uniform sampler2D blockTexture;

void main()
{
    //FragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
    FragColor = texture2D(blockTexture, TexCoord);
}