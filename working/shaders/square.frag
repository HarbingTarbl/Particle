#version 330
uniform vec2 OldMouse;
uniform vec2 NewMouse;

out vec2 ocolor;
void main()
{
	vec2 dir = normalize(NewMouse - OldMouse);
	ocolor =  dir * 10 / distance(NewMouse, gl_FragCoord.xy);
}