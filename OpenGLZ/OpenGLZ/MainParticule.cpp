#include <glad.h>
#include <GLFW/glfw3.h>
#include <vec3.hpp>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtx/transform.hpp>
#include "stl.h"
#include "obj.h"
#include "texture.h"

#include <vector>
#include <iostream>
#include <random>
#include <sstream>
#include <fstream>
#include <string>

glm::vec3 cameraPos, cameraFront, cameraUp;

//#define TINYPLY_IMPLEMENTATION
//#include <tinyply.h>

static void error_callback(int /*error*/, const char* description)
{
	std::cerr << "Error: " << description << std::endl;
}

static void key_callback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	float cameraSpeed = 0.05f; // adjust accordingly
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

/* PARTICULES */
struct Particule {
	glm::vec4 position;
	glm::vec4 color;
	glm::vec4 speed;
};

std::vector<Particule> MakeParticules(const int n)
{
	std::default_random_engine generator;
	std::uniform_real_distribution<float> distribution01(0, 1);
	std::uniform_real_distribution<float> distributionWorld(-1, 1);

	std::vector<Particule> p;
	p.reserve(n);

	for (int i = 0; i < n; i++)
	{
		p.push_back(Particule{
				{
				distributionWorld(generator),
				distributionWorld(generator),
				distributionWorld(generator),
				1.0f
				},
				{
				distribution01(generator),
				distribution01(generator),
				distribution01(generator),
				1.0f
				},
				{
				0.0f,
				distributionWorld(generator),
				0.0f,
				0.0f
				}
			}
		);
	}
	return p;
}

GLuint MakeShader(GLuint t, std::string path)
{
	std::cout << path << std::endl;
	std::ifstream file(path.c_str(), std::ios::in);
	std::ostringstream contents;
	contents << file.rdbuf();
	file.close();

	const auto content = contents.str();
	std::cout << content << std::endl;

	const auto s = glCreateShader(t);

	GLint sizes[] = { (GLint)content.size() };
	const auto data = content.data();

	glShaderSource(s, 1, &data, sizes);
	glCompileShader(s);

	GLint success;
	glGetShaderiv(s, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		GLchar infoLog[512];
		GLsizei l;
		glGetShaderInfoLog(s, 512, &l, infoLog);

		std::cout << infoLog << std::endl;
	}

	return s;
}

GLuint AttachAndLink(std::vector<GLuint> shaders)
{
	const auto prg = glCreateProgram();
	for (const auto s : shaders)
	{
		glAttachShader(prg, s);
	}

	glLinkProgram(prg);

	GLint success;
	glGetProgramiv(prg, GL_LINK_STATUS, &success);
	if (!success)
	{
		GLchar infoLog[512];
		GLsizei l;
		glGetProgramInfoLog(prg, 512, &l, infoLog);

		std::cout << infoLog << std::endl;
	}

	return prg;
}

void APIENTRY opengl_error_callback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{

	std::cout << message << std::endl;
}


int main(void)
{
	GLFWwindow* window;
	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

	window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(window, key_callback);
	glfwMakeContextCurrent(window);
	//glfwSwapInterval(1);
	// NOTE: OpenGL error checks have been omitted for brevity

	if (!gladLoadGL()) {
		std::cerr << "Something went wrong!" << std::endl;
		exit(-1);
	}

	glEnable(GL_DEPTH_TEST);
	// Callbacks
	glDebugMessageCallback(opengl_error_callback, nullptr);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

	const size_t nParticules = 1000;
	const auto particules = MakeParticules(nParticules);

	// Shader
	const auto vertex = MakeShader(GL_VERTEX_SHADER, "shader.vert");
	const auto fragment = MakeShader(GL_FRAGMENT_SHADER, "shader.frag");
	const auto compute = MakeShader(GL_COMPUTE_SHADER, "shader.comp");

	const auto program = AttachAndLink({ vertex, fragment});
	const auto comput_program = AttachAndLink({ compute });

	

	// Arrays
	/*std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> colors;
	std::vector<glm::vec3> speeds;

	for (int i = 0; i < nParticules; i++)
	{
		vertices.push_back(particules[i].position);
		colors.push_back(particules[i].color);
		speeds.push_back(particules[i].speed);
	}*/

	// Buffers
	GLuint vbo, vao;
	glGenBuffers(1, &vbo);
	glGenVertexArrays(1, &vao);

	//Compute Shader
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vbo);
	//glProgramUniform2fv(program, glGetUniformLocation(program, "mouse"), 1, &mouse.x);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, nParticules * sizeof(Particule), particules.data(), GL_STATIC_DRAW);
	//glBufferSubData(GL_ARRAY_BUFFER, nParticules * sizeof(glm::vec3), nParticules * sizeof(glm::vec3), colors.data());
	//glBufferSubData(GL_ARRAY_BUFFER,  2 * nParticules * sizeof(glm::vec3), nParticules * sizeof(glm::vec3), speeds.data());

	glm::mat4 modelView, view, projection;

	//Camera
	cameraPos = glm::vec3(0.0f, 0.0f, -0.5f);
	cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
	cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	// Bindings
	const auto indexModelView = glGetUniformLocation(program, "modelView");
	const auto indexView = glGetUniformLocation(program, "view");
	const auto indexProjection = glGetUniformLocation(program, "projection");

	const auto index = glGetAttribLocation(program, "position");
	const auto indexColor = glGetAttribLocation(program, "color");
	const auto indexSpeed = glGetAttribLocation(program, "speed");


	glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec4)*3, nullptr);
	glVertexAttribPointer(indexColor, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec4)*3, (void*)(sizeof(glm::vec4)));
	//glVertexAttribPointer(indexSpeed, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)(nParticules * 2 * sizeof(glm::vec3)));


	glEnableVertexAttribArray(index);
	glEnableVertexAttribArray(indexColor);
	//glEnableVertexAttribArray(indexSpeed);

	glPointSize(2.f);
	modelView = glm::scale(glm::vec3(1.0f, 1.0f, 1.0f));

	float oldTime = glfwGetTime();

	while (!glfwWindowShouldClose(window))
	{
		float deltaTime = glfwGetTime() - oldTime;
		oldTime = glfwGetTime();
		std::cout << 1.0f / deltaTime << std::endl;

		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		glUseProgram(comput_program);
		glDispatchCompute(nParticules / 32, 1, 1);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);

		//glm::vec3 acceleration = particules[i].mass * 9.81f * glm::vec3(0, -1, 0);

		projection = glm::perspective(90.0f, 1.86f, 0.1f, 5.0f);
		glProgramUniformMatrix4fv(program, indexProjection, 1, GL_FALSE, &projection[0][0]);

		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		glProgramUniformMatrix4fv(program, indexView, 1, GL_FALSE, &view[0][0]);

		//modelView = glm::rotate(modelView, 0.01f, glm::vec3(1.0f, 1.0f, 0.0f));
		glProgramUniformMatrix4fv(program, indexModelView, 1, GL_FALSE, &modelView[0][0]);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(program);
		glDrawArrays(GL_POINTS, 0, nParticules);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
