#version 330
uniform vec2 OldMouse;
uniform vec2 NewMouse;

out vec2 ocolor;
void main()
{
	ocolor = (NewMouse - OldMouse);	
}