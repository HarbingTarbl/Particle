#ifndef _PROGRAM_H_
#define _PROGRAM_H_
#include <unordered_map>
#include <string>
#include <stack>
#include <vector>

#include <glload\gl_3_3.h>
#include <glm\ext.hpp>
#include "binding.h"

using namespace std;
using namespace glm;

namespace particle_
{
	class program_builder;

	class program
	{
	private:
		friend class program_builder;
		program(const program& program) {}
		program& operator=(const program& rhs) {}

		program()
		{
			m_id = glCreateProgram();
		}


		void m_buildUniforms();

	protected:
		unsigned m_id;
		unordered_map<string, unsigned> m_uniforms;
		
	public:
		~program()
		{
			glDeleteProgram(m_id);
		}

		const unsigned getId() const { return m_id; }

		bool uniform(const string& name, const int i);
		bool uniform(const string& name, const float f);

		bool uniform(const string& name, const fvec2& vec);
		bool uniform(const string& name, const fvec3& vec);
		bool uniform(const string& name, const fvec4& vec);

		bool uniform(const string& name, const fmat4& vec);

		void use() const { glUseProgram(m_id); }
	};

	class program_builder
	{
	public:
		enum ProgramPartType
		{
			INFERRED,
			VERTEX,
			FRAGMENT,
			GEOMETRY,
		};

	private:
		struct ProgramPart
			: boost::noncopyable
		{
			const ProgramPartType mPartType;
			const string mName;
		};

		vector<size_t> m_shaderParts;

	public:
		void pushShader(const std::string& fname, ProgramPartType type = INFERRED);
		void reset();
		program* buildProgram();

		program_builder() {}
		~program_builder() { reset(); }
	};
};


#endif