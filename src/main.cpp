#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <random>
#include <memory>

#include <glload\gl_3_3.h>
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
GLuint renders[2];

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


	CheckErrorL(glGenFramebuffers(2, framebuffers));
	CheckErrorL(glGenTextures(2, textures));
	CheckErrorL(glGenRenderbuffers(2, renders));

	unique_ptr<char[]> image(new char[800*600*4]);
	for(int i = 0; i < 16; i++)
	{
		memset(image.get() + (int)(800.0f*(i*600/16)*4), (16)*i,  800*(600/16)*4);
	}


	CheckErrorL(glBindTexture(GL_TEXTURE_2D, textures[0]));
	CheckErrorL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 800, 600, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.get()));
	CheckErrorL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	CheckErrorL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	CheckErrorL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, 0));
	CheckErrorL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, 0));
	CheckErrorL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0));
	CheckErrorL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	CheckErrorL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	CheckErrorL(glBindTexture(GL_TEXTURE_2D, 0));


	CheckErrorL(glBindTexture(GL_TEXTURE_2D, textures[1]));
	CheckErrorL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 800, 600, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0));

	CheckErrorL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	CheckErrorL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	CheckErrorL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, 0));
	CheckErrorL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, 0));
	CheckErrorL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0));
	CheckErrorL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	CheckErrorL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	CheckErrorL(glBindTexture(GL_TEXTURE_2D, 0));

	CheckErrorL(glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[0]));
	CheckErrorL(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textures[0], 0));

	CheckErrorL(glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[1]));
	CheckErrorL(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textures[1], 0));

	CheckErrorL(glBindFramebuffer(GL_FRAMEBUFFER, 0));


	cout << (glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE ? "Complete" : "Incomplete") << endl; //Reports as Complete
	cout << (glCheckFramebufferStatus(GL_READ_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE ? "Complete" : "Incomplete") << endl; //Reports as Complete

	try
	{
		using namespace particle_;
		program_builder b;
		b.pushShader("shaders\\square.frag");
		b.pushShader("shaders\\square.vert");
		shader.reset(b.buildProgram());

	}
	catch(exception &ex)
	{
		cout << ex.what() << endl;
	}

	shader->use();
	shader->uniform("Projection", ortho(0.0f, 800.0f, 0.0f, 600.0f));
	shader->uniform("tex", 0);

	GLuint arr;
	glGenBuffers(1, &arr);
	glBindBuffer(GL_ARRAY_BUFFER, arr);
	const float buffdata[] = 
	{
		0, 0,
		800, 0,
		800, 600,
		800, 600,
		0, 600,
		0, 0,
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(buffdata), buffdata, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, 0);

	CheckErrorL(glDisable(GL_DEPTH_TEST));
	CheckErrorL(glDisable(GL_STENCIL_TEST));
	CheckErrorL(glDisable(GL_CULL_FACE));
	
	CheckError("Init");
	cout << "Init Ok?" << endl;
}

void Display()
{
	CheckErrorL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));


	CheckErrorL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
	CheckErrorL(glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffers[0]));
	CheckErrorL(glBlitFramebuffer(0, 0, 800, 600, 0,0, 800, 600, GL_COLOR_BUFFER_BIT, GL_LINEAR));


	CheckErrorL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	CheckErrorL(glBindTexture(GL_TEXTURE_2D, textures[0]));
	CheckErrorL(glDrawArrays(GL_TRIANGLES, 0, 6));
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