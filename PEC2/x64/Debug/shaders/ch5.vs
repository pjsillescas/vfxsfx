#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in float aColorIndex;

uniform vec3 color1;
uniform vec3 color2;
uniform vec3 color3;
uniform float offset;

out vec3 vertexColor;

void main()
{
	if(aColorIndex >= 1.0f && aColorIndex < 2.0f)
	{
		vertexColor = color1;
	}
	else if(aColorIndex >= 2.0f && aColorIndex < 3.0f)
	{
		vertexColor = color2;
	}
	else // aColorIndex >= 3.0f
	{
		vertexColor = color3;
	}
	//vertexColor = aColor;
	gl_Position = vec4(aPos.x + offset, aPos.y, aPos.z, 1.0);
}