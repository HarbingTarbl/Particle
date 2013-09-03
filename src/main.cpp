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

void Init()
{

	using namespace particle_;
	//program t;
	//auto b = particle_::make_bind(t);

	if(!glload::LoadFunctions())
		cout << "Can't load functions" << endl;
	std::cout << "Loaded : " << glload::GetMajorVersion() << "." << glload::GetMinorVersion() << std::endl;
	std::cout << "glGetString(GL_VERSION) : " << glGetString(GL_VERSION) << std::endl;

	//Set GL States
	glClearColor(0, 0, .3, 1);

	





	cout << "Init Ok?" << endl;
}

void Destroy()
{

}

void Update()
{
	if(glfwGetKey(GLFW_KEY_ESC))
		glfwCloseWindow();
	



}




void Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
}

int main(int argc, char* args[])
{
	glfwInit();
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 3);
	glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	if(!glfwOpenWindow(800, 600, 0, 0, 0, 0, 0, 0, GLFW_WINDOW))
	{
		std::cout << "Unable to create window\n";
		goto exit;
	}

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