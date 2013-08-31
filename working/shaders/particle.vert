#version 330

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 velocity;
layout(location = 2) in vec2 acceleration;

uniform mat4 Projection;
uniform mat4 View;
uniform float TimeStep;
uniform sampler2D ForceTexture;

out vec2 varyPosition;
out vec2 varyVelocity;
out vec2 varyAcceleration;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{
	gl_Position = (Projection *  vec4(position, 0.0f, 1.0f));
	varyAcceleration = texture(ForceTexture, gl_Position.xy).xy;
	vec2 d = acceleration * TimeStep;
	varyVelocity = velocity + d;
	varyPosition = position + (velocity + 0.5f * d) * TimeStep;
}
