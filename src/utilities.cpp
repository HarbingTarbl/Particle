#include "utilities.h"
#include <glload/gl_3_3.h>
#include <iostream>
#include <GL/glfw.h>
#include <functional>

namespace particle
{
	using namespace std;

	void CheckError(const char* str)
	{
		switch(glGetError())
		{
		case GL_NO_ERROR:
			return;
		case GL_INVALID_ENUM:
			cout << "GL_INVALID_ENUM";
			break;
		case GL_INVALID_VALUE:
			cout << "GL_INVALID_VALUE";
			break;
		case GL_INVALID_OPERATION:
			cout << "GL_INVALID_OPERATION";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			cout << "GL_INVALID_FRAMEBUFFER_OPERATION";
			break;
		case GL_OUT_OF_MEMORY:
			cout << "GL_OUT_OF_MEMORY";
			break;
		case GL_STACK_UNDERFLOW:
			cout << "GL_STACK_UNDERFLOW";
			break;
		case GL_STACK_OVERFLOW:
			cout << "GL_STACK_OVERFLOW";
			break;
		default:
			cout << "!UNKNOWN ERROR!";
			break;
		}

		if(str)
			cout << " ! " << str;

		cout << endl;
	}


};