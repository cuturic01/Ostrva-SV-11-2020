#version 330 core 

in vec4 channelCol;
out vec4 outCol;

void main()
{

//	if (channelCol.r == 1.0 && channelCol.g == 0 && channelCol.b == 0) {
//		outCol = mainFlameColor;		
//	}
//	else {
//		outCol = littleFlameColor;
//	}
	outCol = channelCol;
}