#version 330 core

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec4 inCol;
out vec4 channelCol;

uniform float waterLevel;
uniform float sharkPositions[4];


void main()
{
    if ((inPos.y > -0.1 && inCol.r == 0.0 && inCol.g == 0.0 && inCol.b == 0.4)) {
        // ako je boja tamno plava i ako je y vece od -0.1 sto vazi samo za more
        gl_Position = vec4(inPos.x, inPos.y + waterLevel, 0.0, 1.0);
    } 
    else if ((inCol.g == 0.5 && inCol.r == 0.5)) {
        // tamno sivo, ajkule
        int sharkIndex = int(inCol.a);
        vec2 sharkPosition = vec2(sharkPositions[sharkIndex], 0.0);
        vec2 adjustedPos = inPos + sharkPosition;
        gl_Position = vec4(adjustedPos.x, adjustedPos.y + waterLevel, 0.0, 1.0);
    }
    else if (inPos.x == -0.58 || inPos.x == -0.56) {
         int sharkIndex = int(inCol.a);
        vec2 sharkPosition = vec2(sharkPositions[sharkIndex], 0.0);
        vec2 adjustedPos = inPos + sharkPosition;
        gl_Position = vec4(adjustedPos.x, adjustedPos.y + waterLevel, 0.0, 1.0);
    }
    else {
        gl_Position = vec4(inPos, 0.0, 1.0);
    }

    channelCol = inCol;
}