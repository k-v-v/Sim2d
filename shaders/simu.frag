#version 330 core
out vec4 FragColor;

in vec4 Position;
in vec2 TexCoord;

uniform sampler2D Texture1;
uniform sampler2D Texture2;

void main()
{   
    FragColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);
    //Use green and blue channels because they look cooler
    FragColor.x = abs(texture( Texture1, TexCoord).x);
    FragColor.y = abs(texture( Texture2, TexCoord).x);
    FragColor.xyz = length(FragColor.xy)*vec3(1.0f, 1.0f, 1.0f);
    FragColor.w = 1.0f;
}