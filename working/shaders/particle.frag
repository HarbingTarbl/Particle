#version 330
out vec4 ocolor;
in vec2 varyVelocity;
in vec2 varyAcceleration;

void main()
{
	ocolor = vec4(length(varyVelocity) , varyAcceleration, 1.0f);
}