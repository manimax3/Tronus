#shader vertex
#version 330 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec4 iColor;

out vec4 oColor;

void main() { 
    oColor = iColor;
    gl_Position = vec4(position.x, position.y, 0.0, 1.0); }

#shader fragment
#version 330 core

in vec4 oColor;
out vec4 color;

void main() { color = oColor; }

