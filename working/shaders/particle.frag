#version 330

smooth in vec2 iUV;

uniform sampler2D ForceTexture;
uniform sampler2D FieldTexture;

out vec3 oForceTexture;

void main()
{
	vec3 force = texture(ForceTexture, iUV).rgb;
	vec3 field = texture(FieldTexture, gl_PointCoord).rgb;
	vec2 newForce = vec2(force.rg * force.b + field.rg * field.b);
	float newForceLength = length(newForce);
	oForceTexture = vec3(newForce / newForceLength, newForceLength);
}