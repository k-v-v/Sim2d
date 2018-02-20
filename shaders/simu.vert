#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec4 Position;
out vec2 TexCoord;
void main()
{   
    Position = vec4(aPos, 1.0f);
    TexCoord = aTexCoord;
    gl_Position = Position;
}