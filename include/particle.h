#ifndef _PARTICLE_H_
#define _PARTICLE_H_

#include <glm/glm.hpp>
#include <glm/ext.hpp>

namespace particle
{
	using namespace glm;
	struct Particle
	{
		fvec2 mPosition;
		fvec2 mVelocity;
		float mCharge;

		Particle(float x, float y)
			: mPosition(x, y), mVelocity(0), mCharge(1)
		{

		}

		Particle(const fvec2& pos,
			const fvec2& vel,
			const fvec2& accel)
			: mPosition(pos), mVelocity(vel), mCharge(1)
		{

		}
	};

	class ParticleField
	{
	public:
		ParticleField(int radius);

		const unsigned GetTexture() const { return m_texture; }

	private:
		unsigned m_texture;
	};

};
#endif