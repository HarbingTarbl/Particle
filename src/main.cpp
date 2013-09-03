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
GLuint textureId = 0;
GLuint otherTexture = 0;

GLuint framebuffer = 0;
GLuint otherbuffer = 0;

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
#define CheckErrorL CheckError(__FILE__ "@" _UG(__LINE__))

void Init()
{

	
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenTextures(2, &textureId);
	glGenFramebuffers(2, &framebuffer);
	//glGenFramebuffers(1, &otherbuffer);
	//glGenTextures(1, &otherTexture);
	
	glBindTexture(GL_TEXTURE_2D, textureId);
	CheckErrorL;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 800, 600, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	CheckErrorL;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);
	CheckErrorL;

	glBindTexture(GL_TEXTURE_2D, otherTexture);
	CheckErrorL;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 800, 600, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	CheckErrorL;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);
	CheckErrorL;

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	CheckErrorL;

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureId, 0);
	glReadBuffer(GL_COLOR_ATTACHMENT0);

	CheckErrorL;

	const float colori[] = {1.0, 0.0, 1.0, 1.0};
	glClearBufferfv(GL_COLOR, 0, colori);
	CheckErrorL;

	glBindFramebuffer(GL_FRAMEBUFFER, otherbuffer);
	CheckErrorL;
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, otherTexture, 0);
	CheckErrorL;
	const GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);


	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	cout << (glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE ? "Complete" : "Incomplete") << endl; //Reports as Complete
	cout << (glCheckFramebufferStatus(GL_READ_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE ? "Complete" : "Incomplete") << endl; //Reports as Complete

	try
	{
		using namespace particle_;
		program_builder b;
		b.pushShader("shaders/square.frag");
		b.pushShader("shaders/square.vert");
		shader.reset(b.buildProgram());

	}
	catch(exception &ex)
	{
		cout << ex.what() << endl;
	}

	shader->use();

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_CULL_FACE);

	cout << "Init Ok?" << endl;
}

void Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, otherbuffer);
	CheckErrorL;
	const GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);
	
	glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	CheckErrorL;

	glColorMask(true, true, true, true);
	glBlitFramebuffer(0, 0, 0, 0, 800, 600, 800, 600, GL_COLOR_BUFFER_BIT, GL_NEAREST);
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
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 4);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 3);
	glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
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