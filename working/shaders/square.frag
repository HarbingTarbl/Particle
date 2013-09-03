#version 330
uniform sampler2D tex;
smooth in vec2 uv;
out vec4 ocolor;
void main()
{
	ocolor = texture(tex, uv);
}