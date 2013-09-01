#version 330
out vec4 ocolor;
in vec2 varyVelocity;
in vec2 varyAcceleration;


uniform bool DrawParticleMap;
uniform sampler2D ForceTexture;

void main()
{
	float l = length(varyVelocity);
	vec2 dir = normalize(varyVelocity);
	ocolor = vec4(dir , l , 1.0f);
	//ocolor = vec4(texture(ForceTexture, gl_PointCoord ).rg, 0, 0);
}