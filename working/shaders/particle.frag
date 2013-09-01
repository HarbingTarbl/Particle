#version 330
out vec4 ocolor;
in vec2 varyVelocity;
in vec2 varyAcceleration;

uniform bool DrawParticleMap;
uniform sampler2D ForceTexture;

void main()
{
	vec2 dir = normalize(varyAcceleration);
	dir += 1;
	dir /= 2.0f;
	ocolor = vec4(length(varyVelocity), dir,  1.0f);
	//ocolor = vec4(texture(ForceTexture, gl_PointCoord ).rg, 0, 0);
}