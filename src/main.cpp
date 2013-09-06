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
#include "particle.h"
#include "utilities.h"


using namespace particle;
using namespace std;

using glm::fmat4x4;
using glm::fmat4;
using glm::fvec4;
using glm::fvec3;
using glm::fvec2;
using glm::fvec1;
using glm::ortho;
using glm::value_ptr;

GLuint defaultVAO;
unique_ptr<Program> shader;
unique_ptr<ParticleField> field;

GLuint particleVAO[1];
GLuint particleFramebuffer[1];
GLuint particleTransformObjects[2];
GLuint particleTextures[2];
GLuint particleBuffers[3];
unsigned int particleCount = 2500;

void InitParticles()
{
	glGenBuffers(3, particleBuffers);
	glGenTextures(2, particleTextures);
	glGenFramebuffers(1, particleFramebuffer);
	glGenVertexArrays(1, particleVAO);
	glGenTransformFeedbacks(2, particleTransformObjects);

	{
		struct posvel
		{
			fvec2 pos;
			fvec2 vel;
		};

		vector<float> charges(particleCount);
		vector<posvel> posvel(particleCount);


		glBindBuffer(GL_ARRAY_BUFFER, particleBuffers[0]);
		glBufferData(GL_ARRAY_BUFFER, particleCount * sizeof(float), &charges[0], GL_STATIC_DRAW);

		for(int i = 1; i < 3; i++)
		{
			glBindBuffer(GL_ARRAY_BUFFER, particleBuffers[i]);
			glBufferData(GL_ARRAY_BUFFER, particleCount * sizeof(fvec2) * 2, &posvel[0], GL_DYNAMIC_COPY);
		}

	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	for(int i = 0; i < 2; i++)
	{
		glBindTexture(GL_TEXTURE_2D, particleTextures[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16_SNORM, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, particleFramebuffer[0]);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, particleTextures[0], 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, particleTextures[1], 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	for(int i = 1; i < 3; i++)
	{
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, particleTransformObjects[i - 1]);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, particleBuffers[i]);
	}
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);

	glBindVertexArray(particleVAO[0]);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, particleBuffers[0]);
	glVertexAttribPointer(0, 1, GL_FLOAT, false, 0, 0);

	for(int i = 1; i < 3; i++)
	{
		glBindBuffer(GL_ARRAY_BUFFER, particleBuffers[i]);
		glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(fvec2) * 2, 0);
		glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(fvec2) * 2, reinterpret_cast<void*>(sizeof(fvec2)));
	}
	glBindVertexArray(0);

	CheckError("InitParticles");
}

void Init()
{
	if(!glload::LoadFunctions())
		cout << "Can't load functions" << endl;
	std::cout << "Loaded : " << glload::GetMajorVersion() << "." << glload::GetMinorVersion() << std::endl;
	std::cout << "glGetString(GL_VERSION) : " << glGetString(GL_VERSION) << std::endl;
	//Set GL States
	glClearColor(0, 0, .3, 1);
	glGenVertexArrays(1, &defaultVAO);
	glBindVertexArray(defaultVAO);
	try
	{
		ProgramBuilder builder;
		builder.PushShader("shaders/particle.frag");
		builder.PushShader("shaders/particle.vert");
		shader.reset(builder.BuildProgram());

		const char* vary[] = 
		{
			"vPosition",
			"vVelocity"
		};

		glTransformFeedbackVaryings(shader->getId(), 2, vary, GL_INTERLEAVED_ATTRIBS);
		glLinkProgram(shader->getId());
		int status;
		glGetProgramiv(shader->getId(), GL_LINK_STATUS, &status);
		if(!status)
		{
			int size;
			glGetProgramiv(shader->getId(), GL_INFO_LOG_LENGTH, &size);
			vector<char> s(size);
			glGetProgramInfoLog(shader->getId(), size, &size, &s[0]);
			cout << &s[0] << endl;
			throw exception("Varying Link Failed");
		}
	}
	catch(exception& e)
	{
		cout << e.what() << endl;
		cin.get();
	}

	field.reset(new ParticleField(64));
	InitParticles();
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
	//Draw Particles

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