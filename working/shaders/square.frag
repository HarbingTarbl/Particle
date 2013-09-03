#version 330
uniform sampler2D tex;
smooth in vec2 uv;
out vec4 ocolor;

uniform vec4 color;

void main()
{
	vec4 texcolor = texture(tex, uv);
	texcolor = texcolor * (1.0 - 0.000001);
	texcolor = clamp(texcolor - vec4(0.004f), 0.0, 1.0);


	ocolor = clamp(texcolor + color, 0.0, 1.0);
} 