#include "program.h"
#include <memory>
#include <fstream>
#include <boost/filesystem/path.hpp>

namespace particle_
{
	using namespace std;
	using namespace glm;

	bool program::uniform(const string& name, const int i)
	{
		auto kv = m_uniforms.find(name);
		if(kv == m_uniforms.end())
			return false;
		glUniform1i(kv->second, i);

	}

	bool program::uniform(const string& name, const float f)
	{
		auto kv = m_uniforms.find(name);
		if(kv == m_uniforms.end())
			return false;
		glUniform1f(kv->second, f);
	}

	bool program::uniform(const string& name, const fvec2& vec)
	{
		auto kv = m_uniforms.find(name);
		if(kv == m_uniforms.end())
			return false;
		glUniform2fv(kv->second, 1, value_ptr(vec));
	}

	bool program::uniform(const string& name, const fvec3& vec)
	{
		auto kv = m_uniforms.find(name);
		if(kv == m_uniforms.end())
			return false;
		glUniform3fv(kv->second, 1, value_ptr(vec));
	}

	bool program::uniform(const string& name, const fvec4& vec)
	{
		auto kv = m_uniforms.find(name);
		if(kv == m_uniforms.end())
			return false;
		glUniform4fv(kv->second, 1, value_ptr(vec));
	}

	bool program::uniform(const string& name, const fmat4& mat)
	{
		auto kv = m_uniforms.find(name);
		if(kv == m_uniforms.end())
			return false;
		glUniformMatrix4fv(kv->second, 1, false, value_ptr(mat));
	}


	void program_builder::pushShader(const std::string& fname, ProgramPartType type)
	{
		size_t fsize = 0;
		unique_ptr<char[]> buffer;

		try
		{
			using boost::filesystem::path;
			using namespace std;

			ifstream file(path(fname).make_preferred().string(), ifstream::binary);	
			if(!file.is_open())
				throw exception(string("Unable to open " + fname).c_str());
			file.seekg(0, ifstream::end);
			fsize = file.tellg();
			file.seekg(0, ifstream::beg);
			buffer.reset(new char[fsize + 1]);
			file.read(buffer.get(), fsize);
			buffer[fsize] = 0;
			file.close();
		}
		catch(exception& e)
		{
			throw e;
		}

		GLuint shaderType = GL_INVALID_ENUM;
		switch(type)
		{
		case FRAGMENT:
			shaderType = GL_FRAGMENT_SHADER;
			break;
		case VERTEX:
			shaderType = GL_VERTEX_SHADER;
			break;
		case GEOMETRY:
			shaderType = GL_GEOMETRY_SHADER;
			break;
		case INFERRED:
			{
				size_t dotpos = fname.find_last_of(".");
				if(dotpos == string::npos)
				{
					throw exception("Unknown file type");
				}
				string suffix = fname.substr(dotpos);
				if(!suffix.compare(".frag") || !suffix.compare(".fragment"))
				{
					shaderType = GL_FRAGMENT_SHADER;
				}
				else if(!suffix.compare(".vert") || !suffix.compare(".vertex"))
				{
					shaderType = GL_VERTEX_SHADER;
				}
				else if(!suffix.compare(".geom") || !suffix.compare(".geometry"))
				{
					shaderType = GL_GEOMETRY_SHADER;
				}
				else if(!suffix.compare(".geom") || !suffix.compare(".geometry"))
				{
					shaderType = GL_GEOMETRY_SHADER;
				}
				else throw exception("Unknown file type");
				break;
			}
			break;
		default:
			throw exception("Unknown shader type");

		}

		GLuint shader = glCreateShader(shaderType);
		const char* bufptr = buffer.get();
		glShaderSource(shader, 1, &bufptr, NULL);
		glCompileShader(shader);
		GLint status;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
		if(!status)
		{
			unique_ptr<char[]> log;
			GLint bufsize;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &bufsize);
			log.reset(new char[bufsize]);
			glGetShaderInfoLog(shader, bufsize, NULL, log.get());
			throw exception(log.get());
		}

		m_shaderParts.push_back(shader);
	}

	void CheckError(const char* str = NULL)
	{
		bool errored = true;
		switch(glGetError())
		{
		case GL_INVALID_ENUM:
			cout << "Enum";
			break;
		case GL_INVALID_VALUE:
			cout << "Value";
			break;
		case GL_INVALID_OPERATION:
			cout << "Operation";
			break;
		case GL_NO_ERROR:
			errored = false;
			return;
		}

		if(str)
			cout << " : " << str;

		cout << endl;
	}

	program* program_builder::buildProgram()
	{
		unique_ptr<program> program(new program());

		for(auto shader : m_shaderParts)
			glAttachShader(program->getId(), shader);

		glLinkProgram(program->getId());
		GLint linkStatus;
		glGetProgramiv(program->getId(), GL_LINK_STATUS, &linkStatus);
		if(!linkStatus)
		{
			unique_ptr<char[]> infobuf;
			GLint logsize;
			glGetProgramiv(program->getId(), GL_INFO_LOG_LENGTH, &logsize);
			infobuf.reset(new char[logsize]);
			glGetProgramInfoLog(program->getId(), logsize, NULL, infobuf.get());
			throw exception(infobuf.get());
		}

		program->m_buildUniforms();
		return program.release();
	}

	void program::m_buildUniforms()
	{
		GLint nUniforms;
		glGetProgramiv(getId(), GL_ACTIVE_UNIFORMS, &nUniforms);
		CheckError("ACTIVE_UNI");
		GLsizei maxUniformLength, readUniformLength;
		glGetProgramiv(getId(), GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxUniformLength);
		CheckError("ACTIVE_UNI_LENGTH");

		unique_ptr<char[]> uniformName(new char[maxUniformLength]);
		m_uniforms.reserve(nUniforms);
		for(auto i = 0; i < nUniforms; i++)
		{
			glGetActiveUniformName(getId(), i, maxUniformLength, &readUniformLength, uniformName.get());
			CheckError("ACTIVE_UNI_NAME");

			m_uniforms.insert(make_pair(string(uniformName.get(), readUniformLength), i));
		}
	}

	void program_builder::reset()
	{
		for(auto shader : m_shaderParts)
		{
			glDeleteShader(shader);
		}

		m_shaderParts.clear();
	}
};