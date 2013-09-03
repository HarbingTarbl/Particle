#version 330
uniform sampler2D tex;
smooth in vec2 uv;
out vec4 ocolor;

uniform vec4 color;
uniform vec2 MousePos;


const vec4 Up = vec4(1.0, 0.0, 0.0, 1.0);
const vec4 Down = vec4(0.0, 0.0, 1.0, 1.0);
const vec4 Left = vec4(1.0, 0.0, 0.0, 1.0);
const vec4 Right = vec4(0.0, 1.0, 0.0, 1.0);




void main()
{
	vec2 normmouse = vec2(MousePos.x, 600.0 - MousePos.y);
	vec2 dir = normalize(gl_FragCoord.xy - normmouse);
	dir += 1.0;
	dir /= 2.0;
	//ocolor = vec4(mix(Up, Down, dir.x) + mix(Left, Right, dir.y));
	ocolor = texture(tex, dir);
} 