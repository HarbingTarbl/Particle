#version 330

layout(location = 0) in float iCharge;
layout(location = 1) in vec2 iPosition;
layout(location = 2) in vec2 iVelocity;

out vec2 vPosition;
out vec2 vVelocity;

smooth out vec2 iUV;

uniform mat4 Projection;
uniform float Step;
uniform sampler2D ForceTexture;
uniform sampler2D FieldTexture;

void main()
{
	gl_PointSize = iCharge;
	gl_Position  = Projection * vec4(iPosition, 0.0f, 1.0f);
	iUV = ((gl_Position.xy / gl_Position.w) + 1.0) / 2.0f;

	vPosition = iPosition;
	vVelocity = iVelocity;
}
