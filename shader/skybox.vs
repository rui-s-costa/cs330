#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;

out vec2 UV;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 pos =  projection * view * model * vec4(position, 1.0);
    gl_Position =  pos.xyww;
    UV = vec2(texCoord.x, 1.0 - texCoord.y);
}