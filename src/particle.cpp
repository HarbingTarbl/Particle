#include "particle.h"
#include "utilities.h"
#include <glload/gl_3_3.h>
#include <boost/multi_array.hpp>

namespace particle
{
	using namespace boost;
	using namespace glm;

	ParticleField::ParticleField(int diameter)
	{
		CHERRL(glGenTextures(1, &m_texture));
		CHERRL(glBindTexture(GL_TEXTURE_2D, m_texture));
		multi_array<fvec3, 2> image(extents[diameter][diameter]);
		fvec2 center(diameter / 2.0f);
		for(int i = 0; i < diameter; i++)
		{
			for(int n = 0; n < diameter; n++)
			{
				fvec2 point(i, n);
				image[i][n] = vec3(normalize(point - center), distance2(point, center));
			}
		}
		CHERRL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16_SNORM, diameter, diameter, 0, GL_RGB, GL_FLOAT, image.data()));
		CHERRL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		CHERRL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		CHERRL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0));
		CHERRL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, 0));
		CHERRL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, 0));
		CHERRL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0));
		CHERRL(glBindTexture(GL_TEXTURE_2D, 0));
	}

};