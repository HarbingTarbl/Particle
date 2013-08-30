#version 330

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 velocity;
layout(location = 2) in vec2 acceleration;

uniform mat4 Projection;
uniform mat4 View;
uniform float TimeStep;

out vec2 varyPosition;
out vec2 varyVelocity;
out vec2 varyAcceleration;

void main()
{
	gl_Position = (Projection *  vec4(position, 0.0f, 1.0f));
	//varyAcceleration = max(acceleration * ( 1 - 0.002 * TimeStep), 0.0); //Should be based on image
	varyAcceleration = acceleration;
	vec2 d = acceleration * TimeStep;
	varyVelocity = velocity + d;
	varyPosition = position + (velocity + 0.5f * d) * TimeStep;
}
