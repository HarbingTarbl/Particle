#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <random>

#include <glload\gl_3_3.h>
#include <glload\gl_load.hpp>
#include <gl\glfw.h>
#include <glimg\glimg.h>
#include <glutil\glutil.h>
#include <glm\glm.hpp>
#include <glm\ext.hpp>

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

GLuint particleShader;
GLuint arrayBuffer[2];
GLuint currrentArrayBuffer;
GLuint defaultVAO[2];
GLuint uniformProjection;
GLuint uniformView;
GLuint uniformTimeStep;
GLuint numParticles;
GLuint feedbackObject[2];

void CheckError()
{
	switch(glGetError())
	{
	case GL_INVALID_ENUM:
		cout << "Enum" << endl;
		break;
	case GL_INVALID_VALUE:
		cout << "Value" << endl;
		break;
	case GL_INVALID_OPERATION:
		cout << "Oper" << endl;
		break;
	}
}

void Init()
{
	if(!glload::LoadFunctions())
		cout << "Can't load functions" << endl;
	std::cout << "Loaded : " << glload::GetMajorVersion() << "." << glload::GetMinorVersion() << std::endl;
	std::cout << "glGetString(GL_VERSION) : " << glGetString(GL_VERSION) << std::endl;

	//Set GL States
	glClearColor(0, 0, .3, 1);

	try //Create Shader
	{
		using namespace glutil;
		auto vert = CompileShader(GL_VERTEX_SHADER, 
			string(
				istreambuf_iterator<char>(ifstream("shaders\\particle.vert", ifstream::binary)),
				istreambuf_iterator<char>()));
		auto frag = CompileShader(GL_FRAGMENT_SHADER,
			string(
				istreambuf_iterator<char>(ifstream("shaders\\particle.frag", ifstream::binary)),
				istreambuf_iterator<char>()));

		auto program = LinkProgram(vert, frag);
		const char* varyings[] = 
		{
			"varyPosition",
			"varyVelocity",
			"varyAcceleration"
		};

		program = glCreateProgram();
		glAttachShader(program, vert);
		glAttachShader(program, frag);
		glTransformFeedbackVaryings(program, 3, varyings, GL_INTERLEAVED_ATTRIBS);
		glLinkProgram(program);
		GLint status;
		glGetProgramiv(program, GL_LINK_STATUS, &status);
		cout << "LINK : " << (status ? "OK!" : "FAILED!") << endl;

		if(!status)
		{
			GLint count;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &count);
			vector<char> err(count);
			glGetProgramInfoLog(program, count, &count, &err[0]);
			throw exception("Link Failed");
		}

		CheckError();
		particleShader = program;
	}
	catch(exception& e)
	{
		cout << e.what() << endl;
	}



	//Buffer Information

	glGenTransformFeedbacks(2, feedbackObject);

	glGenBuffers(2, arrayBuffer);
	currrentArrayBuffer = 0;

	vector<particle> particles;
	int xpart = 200;
	int ypart = 200;
	numParticles = xpart * ypart;;
	float deltax = 3.5f;
	float deltay = 2.5f;
	particles.reserve(numParticles);
	for(int x = 0; x < xpart; x++)
	{
		for(int y = 0; y < ypart; y++)
		{
			particles.emplace_back(
				fvec2((float)x * (float)deltax + 10, (float)y * (float)deltay + 10),
				fvec2((((rand() % 500) - 250) / 250.0f), (((rand() % 500) - 250) / 250.0f)), fvec2(0));
		}
	}
	
	glUseProgram(particleShader);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedbackObject[0]);
	glBindBuffer(GL_ARRAY_BUFFER, arrayBuffer[0]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, arrayBuffer[0]);
	glBufferData(GL_ARRAY_BUFFER, numParticles * sizeof(particle), &particles[0], GL_DYNAMIC_COPY);
	CheckError();
	
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedbackObject[1]);
	glBindBuffer(GL_ARRAY_BUFFER, arrayBuffer[1]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, arrayBuffer[1]);
	glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof(particle), NULL, GL_DYNAMIC_COPY);
	CheckError();
	
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);
	CheckError();

	glGenVertexArrays(2, defaultVAO);
	glBindVertexArray(0);
	CheckError();
	for(auto i = 0; i < 2; i++)
	{
		glBindVertexArray(defaultVAO[i]);
		CheckError();
		glBindBuffer(GL_ARRAY_BUFFER, arrayBuffer[i]);
		CheckError();
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, arrayBuffer[i]);
		CheckError();

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		CheckError();

		glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(particle), 0);
		glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(particle), (void*)(sizeof(fvec2)));
		glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(particle), (void*)(2*sizeof(fvec2)));
		CheckError();
	}

	glBindVertexArray(0);
	glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glUseProgram(particleShader);
	uniformProjection = glGetUniformLocation(particleShader, "Projection");
	uniformView = glGetUniformLocation(particleShader, "View");
	uniformTimeStep = glGetUniformLocation(particleShader, "TimeStep");

	glUniform1f(uniformTimeStep, 0.01f);
	glUniformMatrix4fv(uniformProjection, 1, false, value_ptr(ortho(0.0f, 800.0f, 0.0f, 600.0f)));
	glUniformMatrix4fv(uniformView, 1, false, value_ptr(fmat4(1.0f)));
	CheckError();

	glBindVertexArray(defaultVAO[0]);
	CheckError();
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedbackObject[1]);
	CheckError();
	glBeginTransformFeedback(GL_POINTS);
	CheckError();
	glDrawArrays(GL_POINTS, 0, numParticles);
	CheckError();
	glEndTransformFeedback();
	CheckError();
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
	CheckError();
	glBindVertexArray(0);
	CheckError();

}

void Destroy()
{

}


char go = 0;
int steps = 1;
void Update()
{
	if(glfwGetKey(GLFW_KEY_ESC))
		glfwCloseWindow();

	go = glfwGetKey(GLFW_KEY_PAUSE) ? !go : go;

	if(glfwGetKey(GLFW_KEY_UP))
		steps++;
	else if(glfwGetKey(GLFW_KEY_DOWN))
		steps--;

	if(go || glfwGetKey(GLFW_KEY_SPACE))
	{
		glEnable(GL_RASTERIZER_DISCARD);
		for(int i = 0; i < steps; i++)
		{
			glBindVertexArray(defaultVAO[currrentArrayBuffer]);
			glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedbackObject[currrentArrayBuffer ^= 0]);
			glBeginTransformFeedback(GL_POINTS);
			glDrawArrays(GL_POINTS, 0, numParticles);
			glEndTransformFeedback();
		}
		glDisable(GL_RASTERIZER_DISCARD);
	}
}

void Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindVertexArray(defaultVAO[currrentArrayBuffer]);
	glDrawArrays(GL_POINTS, 0, numParticles);
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