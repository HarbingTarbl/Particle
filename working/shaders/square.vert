#version 330
layout(location = 0) in vec2 position;

uniform mat4 Projection;
smooth out vec2 uv;
void main()
{
	gl_Position = Projection * vec4(position, 0.0f, 1.0f);
	uv = ((gl_Position.xy / gl_Position.w) + 1.0) / 2.0;
}