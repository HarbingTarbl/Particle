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

namespace particle
{
	class ProgramBuilder;

	class Program
	{
	private:
		friend class ProgramBuilder;
		Program(const Program& program) {}
		Program& operator=(const Program& rhs) {}

		Program()
		{
			m_id = glCreateProgram();
		}


		void m_buildUniforms();

	protected:
		unsigned m_id;
		unordered_map<string, unsigned> m_uniforms;
		
	public:
		~Program()
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

	class ProgramBuilder
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
		void PushShader(const std::string& fname, ProgramPartType type = INFERRED);
		void Reset();
		Program* BuildProgram();

		ProgramBuilder() {}
		~ProgramBuilder() { Reset(); }
	};
};


#endif