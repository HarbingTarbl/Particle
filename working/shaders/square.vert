#version 330
layout(location = 0) in vec2 position;

uniform mat4 Projection;
uniform mat4 Transform;
uniform vec2 NewMouse;

void main()
{
	gl_Position = vec4(position, 0.0f, 1.0f);
}