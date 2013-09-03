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

struct particle
{
	fvec2 position;
	fvec2 velocity;
	fvec2 acceleration;

	particle(float x, float y)
		: position(x, y), velocity(0), acceleration(0)
	{

	}

	particle(const fvec2& pos,
		const fvec2& vel,
		const fvec2& accel)
		: position(pos), velocity(vel), acceleration(accel)
	{

	}
};

GLuint arrayBuffer[2];
GLuint currrentArrayBuffer;
GLuint defaultVAO[2];
GLuint numParticles;
GLuint feedbackObject[2];

unique_ptr<program> particleShader;

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

	try //Create Shader
	{
		using namespace glutil;
		program_builder builder;
		builder.pushShader("shaders/particle.vert");
		CheckError("Vertex Creation");

		builder.pushShader("shaders/particle.frag");
		CheckError("Fragment Creation");

		particleShader.reset(builder.buildProgram());
		CheckError("Program Creation");

		const char* varyings[] = 
		{
			"varyPosition",
			"varyVelocity",
			"varyAcceleration"
		};

		glTransformFeedbackVaryings(particleShader->getId(), 3, varyings, GL_INTERLEAVED_ATTRIBS);
		glLinkProgram(particleShader->getId());
		GLint status;
		glGetProgramiv(particleShader->getId(), GL_LINK_STATUS, &status);
		cout << "LINK : " << (status ? "OK!" : "FAILED!") << endl;
		if(!status)
		{
			GLint count;
			glGetProgramiv(particleShader->getId(), GL_INFO_LOG_LENGTH, &count);
			vector<char> err(count);
			glGetProgramInfoLog(particleShader->getId(), count, &count, &err[0]);
			cerr << &err[0] << endl;
#if (_MSC_VER)  && (_DEBUG)

			OutputDebugString(&err[0]);
#endif
			throw exception("Link Failed");
		}
		CheckError("Program Linking");
	}
	catch(exception& e)
	{
		#if (_MSC_VER)  && (_DEBUG)
			OutputDebugString(e.what());
		#endif
		cerr << e.what() << endl;
		exit(-1);
	}

	//Buffer Information

	glGenTransformFeedbacks(2, feedbackObject);

	glGenBuffers(2, arrayBuffer);
	currrentArrayBuffer = 0;

	vector<particle> particles;
	int xpart = 500;
	int ypart = 500;
	numParticles = xpart * ypart;;
	particles.reserve(numParticles);
	auto posx(uniform_real_distribution<float>(150.0f,650.0f));
	auto posy(uniform_real_distribution<float>(150.0f,450.0f));
	auto gen = minstd_rand();
	for(int x = 0; x < xpart; x++)
	{
		for(int y = 0; y < ypart; y++)
		{
			particles.emplace_back(
				fvec2(posx(gen), posy(gen)),
				fvec2(0, 0), 
				fvec2(0, 0));
		}
	}
	
	particleShader->use();
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedbackObject[0]);
	CheckError("Buffer[0] Transform Feedback");

	glBindBuffer(GL_ARRAY_BUFFER, arrayBuffer[0]);
	CheckError("Buffer[0] Bind");

	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, arrayBuffer[0]);
	CheckError("Buffer[0] BufferBase");

	glBufferData(GL_ARRAY_BUFFER, numParticles * sizeof(particle), &particles[0], GL_DYNAMIC_COPY);
	CheckError("Buffer[0] BufferData");
	
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedbackObject[1]);
	CheckError("Buffer[1] Transform Feedback");

	glBindBuffer(GL_ARRAY_BUFFER, arrayBuffer[1]);
	CheckError("Buffer[1] Bind");

	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, arrayBuffer[1]);
	CheckError("Buffer[1] BufferData");

	glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof(particle), NULL, GL_DYNAMIC_COPY);
	CheckError("Buffer[1] BufferData");

	
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);
	CheckError("Reset Buffers");

	glGenVertexArrays(2, defaultVAO);
	glBindVertexArray(0);
	CheckError("Generate VAOs");
	for(auto i = 0; i < 2; i++)
	{
		glBindVertexArray(defaultVAO[i]);
		CheckError("Bind VAO");
		glBindBuffer(GL_ARRAY_BUFFER, arrayBuffer[i]);
		CheckError("Bind Buffer");
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, arrayBuffer[i]);
		CheckError("Bind Transform Buffer");

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		CheckError("VertexAttribArray");

		glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(particle), 0);
		glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(particle), (void*)(sizeof(fvec2)));
		glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(particle), (void*)(2*sizeof(fvec2)));
		CheckError("VertexAttribPointer");
	}

	glBindVertexArray(0);
	glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	particleShader->use();
	particleShader->uniform("ForceTexture", 0);
	particleShader->uniform("TimeStep", 0.0005f);
	particleShader->uniform("Projection", ortho(0.0f, 800.0f, 0.0f, 600.0f));
	particleShader->uniform("View", fmat4(1.0f));
	CheckError("Uniforms");

	glEnable(GL_RASTERIZER_DISCARD);
	glBindVertexArray(defaultVAO[0]);
	CheckError("First Draw, bind VAO");
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedbackObject[1]);
	CheckError("First Draw, bind TF");
	glBeginTransformFeedback(GL_POINTS);
	CheckError("First Draw, Begin");
	glDrawArrays(GL_POINTS, 0, numParticles);
	CheckError("First Draw, Draw");
	glEndTransformFeedback();
	CheckError("First Draw, End");
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
	CheckError("First Draw, Unbind TF");
	glBindVertexArray(0);
	CheckError("First Draw, Unbind VAO");
	glDisable(GL_RASTERIZER_DISCARD);

	particleShader->use();

	CheckError("Square Program Uniforms");

	cout << "Init Ok?" << endl;
}

void Destroy()
{

}

glm::ivec2 oldmouse;
glm::ivec2 newmouse;
int mousedown = 0;
char go = 0;
int steps = 10;
double tts = 0;
void Update()
{
	if(glfwGetKey(GLFW_KEY_ESC))
		glfwCloseWindow();
	double ctime = glfwGetTime();
	if(ctime >= tts)
	{
		if(glfwGetKey(GLFW_KEY_PAUSE))
		{
			go = !go;
			cout << "Auto Step : " << (go ? "Enabled" : "Disabled") << endl;
			tts = ctime + 0.1;
		}
		if(glfwGetKey(GLFW_KEY_UP))
		{
			tts = ctime + 0.05;
			steps++;
			cout << "Steps : " << steps << endl;
		}
		else if(glfwGetKey(GLFW_KEY_DOWN))
		{
			tts = ctime + 0.05;
			steps--;
			cout << "Steps : " << steps << endl;
		}
	}

	glfwGetMousePos(&newmouse.x, &newmouse.y);
	newmouse.y = 600 - newmouse.y;

	if(go || glfwGetKey(GLFW_KEY_SPACE))
	{
		particleShader->use();
		glEnable(GL_RASTERIZER_DISCARD);
		for(int i = 0; i < steps; i++)
		{
			glBindVertexArray(defaultVAO[currrentArrayBuffer]);
			currrentArrayBuffer = !currrentArrayBuffer;
			glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedbackObject[currrentArrayBuffer]);
			glBeginTransformFeedback(GL_POINTS);
			CheckError("Begin Feedback");
			glDrawArrays(GL_POINTS, 0, numParticles);
			CheckError("Draw Arrays");
			glEndTransformFeedback();
			CheckError("Update");
		}
		glDisable(GL_RASTERIZER_DISCARD);
	}
}




void Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Draw Particles
	particleShader->use();
	particleShader->uniform("NewMouse", fvec2(newmouse));
	glBindVertexArray(defaultVAO[currrentArrayBuffer]);
	glDrawArrays(GL_POINTS, 0, numParticles);
	CheckError("Display Particles");
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