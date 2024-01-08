#version 330 core

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec4 inCol;
out vec4 channelCol;

uniform vec4 flameColor1;
uniform vec4 flameColor2;

uniform float flamePosition;

void main()
{
    
    if ((inPos.x == -0.58 && inPos.y == -0.1) || (inPos.x == -0.56 && inPos.y == -0.1)) {
        gl_Position = vec4(inPos.x - flamePosition, inPos.y, 0.0, 1.0);
    }
    else if ((inPos.x == -0.52 && inPos.y == -0.1) || (inPos.x == -0.54 && inPos.y == -0.1)) {
        gl_Position = vec4(inPos.x + flamePosition, inPos.y, 0.0, 1.0);
    }
    else if ((inPos.x == -0.55 && inPos.y == 0.0) || (inPos.x == -0.55 && inPos.y == -0.05)) {
        gl_Position = vec4(inPos.x, inPos.y + flamePosition, 0.0, 1.0);
    }
    else {
        gl_Position = vec4(inPos, 0.0, 1.0);
         channelCol = inCol;
    }

    if (inCol.r == 1 && inCol.g == 0 && inCol.b == 0) {
        channelCol = flameColor1;
    }
    else {
        channelCol = flameColor2;
    }
}