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
int clicked;
int showtex;

fvec2 mouse;


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

	unique_ptr<char[]> image(new char[800*600*4]);
	memset(image.get(), 0, 800*600*4);
	for(int i = 0; i < 800; i++)
	{
		for(int x = 0; x < 600; x++)
		{
			if(x == 0 || x == 599 || i == 0 || i == 799)
			for(int n = 0; n < 4; n++)
				image[x*800*4 + i*4 + n] = 0xFF;

		}
	}
	CheckErrorL(glGenFramebuffers(2, framebuffers));
	CheckErrorL(glGenTextures(2, textures));
	CheckErrorL(glGenRenderbuffers(2, renders));

	CheckErrorL(glBindTexture(GL_TEXTURE_2D, textures[0]));
	CheckErrorL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 800, 600, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL));
	CheckErrorL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	CheckErrorL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	CheckErrorL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, 0));
	CheckErrorL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, 0));
	CheckErrorL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0));
	CheckErrorL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	CheckErrorL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	CheckErrorL(glBindTexture(GL_TEXTURE_2D, 0));


	CheckErrorL(glBindTexture(GL_TEXTURE_2D, textures[1]));
	CheckErrorL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 800, 600, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.get()));

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
	//CheckErrorL(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, textures[1], 0));
	CheckErrorL(glBindFramebuffer(GL_FRAMEBUFFER, 0));


	cout << (glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE ? "Complete" : "Incomplete") << endl; //Reports as Complete

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
	shader->uniform("Projection", ortho(0.0f, 800.0f, 600.0f, 0.0f));
	shader->uniform("tex", 0);
	glActiveTexture(GL_TEXTURE0);

	GLuint arr;
	glGenBuffers(1, &arr);
	glBindBuffer(GL_ARRAY_BUFFER, arr);
	const float buffdata[] = 
	{
		-0, 0,
		800, 0,
		800, 600,
		800, 600,
		0, 600,
		-0, 0,
	};

	glBufferData(GL_ARRAY_BUFFER, sizeof(buffdata), buffdata, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, 0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);

	CheckError("Init");
	cout << "Init Ok?" << endl;
}

int currentFramebuffer = 0;
fvec4 color;
void Display()
{
	CheckErrorL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	CheckErrorL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffers[0]));
	CheckErrorL(glBindTexture(GL_TEXTURE_2D, textures[1]));
	//currentFramebuffer = !currentFramebuffer;
	CheckErrorL(glDrawBuffer(GL_COLOR_ATTACHMENT0));
	shader->uniform("Projection", ortho(0.0f, 800.0f, 600.0f, 0.0f));
	shader->uniform("MousePos", mouse);
	//shader->uniform("color", fvec4(0,0,0,0));
	CheckErrorL(glDrawArrays(GL_TRIANGLES, 0, 6));

	CheckErrorL(glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffers[0]));
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	CheckErrorL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
	CheckErrorL(glBlitFramebuffer(0, 0, 800, 600, 0, 0, 800, 600, GL_COLOR_BUFFER_BIT, GL_NEAREST));
	CheckError("Display");
}




void Destroy()
{

}

void Update()
{
	if(glfwGetKey(GLFW_KEY_ESC))
		glfwCloseWindow();

	int x, y;

	glfwGetMousePos(&x, &y);
	mouse.x = x;
	mouse.y = y;
	clicked = glfwGetMouseButton(GLFW_MOUSE_BUTTON_RIGHT);
	showtex = glfwGetKey(GLFW_KEY_F1);

	if(glfwGetKey(GLFW_KEY_KP_1))
		color = fvec4(1.0, 0, 0, 1);
	else if(glfwGetKey(GLFW_KEY_KP_2))
		color = fvec4(0.0, 1.0, 0, 1);
	else if(glfwGetKey(GLFW_KEY_KP_3))
		color = fvec4(0.0, 0.0, 1.0, 1);
	else if(glfwGetKey(GLFW_KEY_KP_3))
		color = fvec4(0.0, 1.0, 1.0, 1);





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