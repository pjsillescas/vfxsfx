#version 330 core

layout (location = 0) in vec3 LVertexPos;
layout (location = 1) in vec2 texCoor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out	vec2 texFragment;

void main(){
	gl_Position = projection * view * model * vec4( LVertexPos.xyz, 1 );
	texFragment = texCoor;
}
