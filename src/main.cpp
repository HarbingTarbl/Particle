#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <random>
#include <memory>

#include <glload\gl_4_3.h>
#include <glload\gl_load.hpp>
#include <gl\glfw.h>
#include <glimg\glimg.h>
#include <glutil\glutil.h>
#include <glm\glm.hpp>
#include <glm\ext.hpp>



#include "program.h"
using namespace particle_;

using namespace std;

using glm::fmat4x4;
using glm::fmat4;
using glm::fvec4;
using glm::fvec3;
using glm::fvec2;
using glm::fvec1;
using glm::ortho;
using glm::value_ptr;



unique_ptr<program> shader;
GLuint textures[2];
GLuint framebuffers[2];

GLuint vao;


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

#define __UG(x) #x
#define _UG(x) __UG(x)
#ifdef _DEBUG
#define CheckErrorL(CMD) CMD; CheckError(__UG(CMD) "@" __FILE__ ":" _UG(__LINE__))
#else
#define CheckErrorL(CMD) CMD
#endif

void Init()
{

	
	CheckErrorL(glGenVertexArrays(1, &vao));
	CheckErrorL(glBindVertexArray(vao));
		CheckError("In1it");


	CheckErrorL(glGenFramebuffers(2, framebuffers));
	CheckErrorL(glGenTextures(2, textures));
	CheckErrorL(glBindTexture(GL_TEXTURE_2D, textures[0]));
	CheckErrorL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 800, 600, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL));
	CheckErrorL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	CheckErrorL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	CheckErrorL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, 0));
	CheckErrorL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, 0));
	CheckErrorL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0));
	CheckErrorL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	CheckErrorL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	CheckErrorL(glBindTexture(GL_TEXTURE_2D, 0));
		CheckError("I2nit");


	CheckErrorL(glBindTexture(GL_TEXTURE_2D, textures[1]));
	CheckErrorL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 800, 600, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL));
	CheckError("In3it");

	CheckErrorL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	CheckErrorL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	CheckErrorL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, 0));
	CheckErrorL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, 0));
	CheckErrorL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0));
	CheckErrorL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	CheckErrorL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	CheckErrorL(glBindTexture(GL_TEXTURE_2D, 0));
	CheckError("Ini4t");

	CheckErrorL(glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[0]));

	CheckErrorL(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textures[0], 0));
	CheckErrorL(glReadBuffer(GL_COLOR_ATTACHMENT0));
	CheckError("I5nit");


	const float colori[] = {1.0, 0.0, 1.0, 1.0};
	CheckErrorL(glClearBufferfv(GL_COLOR, 0, colori));

	CheckErrorL(glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[1]));
	CheckError("In6it");
	CheckErrorL(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textures[1], 0));
	CheckError("In7it");

	const GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	CheckErrorL(glDrawBuffers(1, drawBuffers));
	CheckError("In8it");


	CheckErrorL(glBindFramebuffer(GL_FRAMEBUFFER, 0));


	cout << (glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE ? "Complete" : "Incomplete") << endl; //Reports as Complete
	cout << (glCheckFramebufferStatus(GL_READ_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE ? "Complete" : "Incomplete") << endl; //Reports as Complete

	try
	{
		using namespace particle_;
		program_builder b;
		b.pushShader("shaders/square.frag");
		b.pushShader("shaders/square.vert");
		//shader.reset(b.buildProgram());

	}
	catch(exception &ex)
	{
		cout << ex.what() << endl;
	}

	//shader->use();

	CheckErrorL(glDisable(GL_DEPTH_TEST));
	CheckErrorL(glDisable(GL_STENCIL_TEST));
	CheckErrorL(glDisable(GL_CULL_FACE));
	
	CheckError("Init");
	cout << "Init Ok?" << endl;
}

void Display()
{
	CheckErrorL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));


	CheckErrorL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffers[1]));
	const GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	CheckErrorL(glDrawBuffers(1, drawBuffers));
	
	CheckErrorL(glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffers[0]));
	CheckErrorL(glReadBuffer(GL_COLOR_ATTACHMENT0));

	CheckErrorL(glColorMask(true, true, true, true));
	CheckErrorL(glBlitFramebuffer(0, 0, 0, 0, 800, 600, 800, 600, GL_COLOR_BUFFER_BIT, GL_NEAREST));
	CheckError("Display");
}




void Destroy()
{

}

void Update()
{
	if(glfwGetKey(GLFW_KEY_ESC))
		glfwCloseWindow();
	
	CheckError("Update");
}




int main(int argc, char* args[])
{
	glfwInit();
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 3);
	glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwOpenWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
	glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 0);

	if(!glfwOpenWindow(800, 600, 0, 0, 0, 0, 0, 0, GLFW_WINDOW))
	{
		std::cout << "Unable to create window\n";
		goto exit;
	}

	if(!glload::LoadFunctions())
		cout << "Can't load functions" << endl;
	std::cout << "Loaded : " << glload::GetMajorVersion() << "." << glload::GetMinorVersion() << std::endl; //Reports 3.3
	std::cout << "glGetString(GL_VERSION) : " << glGetString(GL_VERSION) << std::endl; //Reports 3.3

	glfwSwapBuffers();  
	Init();
	while(glfwGetWindowParam(GLFW_OPENED))
	{
		Display();
		glfwSwapBuffers();
		Update();
	}
	Destroy();

exit:
	glfwTerminate();
	glfwCloseWindow();
}