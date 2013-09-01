#version 330

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 velocity;
layout(location = 2) in vec2 acceleration;

uniform mat4 Projection;
uniform mat4 View;
uniform float TimeStep;
uniform sampler2D ForceTexture;
uniform vec2 NewMouse;

out vec2 varyPosition;
out vec2 varyVelocity;
out vec2 varyAcceleration;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{
	gl_Position = (Projection *  vec4(position, 0.0f, 1.0f));
	vec2 dir = normalize(NewMouse - position);
	float scale = distance(position, NewMouse);
	varyAcceleration = dir * 100  - velocity * .1f;
	vec2 d = varyAcceleration * TimeStep;
	clamp(varyVelocity = velocity + d, -100, 100);
	if(position.x >= 800.0f || position.x <= 0)
		varyVelocity.x = -varyVelocity.x*0.8;// * sign(position.x - NewMouse.x);
	if(position.y >= 600.0f || position.y <= 0)
		varyVelocity.y = -varyVelocity.y*0.8;// * sign(position.y - NewMouse.y);
	varyPosition = position + (varyVelocity + 0.5f * d) * TimeStep;
}
