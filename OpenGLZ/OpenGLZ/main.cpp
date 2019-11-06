#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include "stl.h"

#include <vector>
#include <iostream>
#include <random>
#include <sstream>
#include <fstream>
#include <string>

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
}

/* PARTICULES */
struct Particule {
	glm::vec3 position;
	glm::vec3 color;
	glm::vec3 speed;
};

std::vector<Particule> MakeParticules(const int n)
{
	std::default_random_engine generator;
	std::uniform_real_distribution<float> distribution01(0, 1);
	std::uniform_real_distribution<float> distributionWorld(-1, 1);

	std::vector<Particule> p;
	p.reserve(n);

	for(int i = 0; i < n; i++)
	{
		p.push_back(Particule{
				{
				distributionWorld(generator),
				distributionWorld(generator),
				distributionWorld(generator)
				},
				{
				distribution01(generator),
				distribution01(generator),
				distribution01(generator)
				},
				{0.f, 0.f, 0.f}
				});
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

	GLint sizes[] = {(GLint) content.size()};
	const auto data = content.data();

	glShaderSource(s, 1, &data, sizes);
	glCompileShader(s);

	GLint success;
	glGetShaderiv(s, GL_COMPILE_STATUS, &success);
	if(!success)
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
	for(const auto s : shaders)
	{
		glAttachShader(prg, s);
	}

	glLinkProgram(prg);

	GLint success;
	glGetProgramiv(prg, GL_LINK_STATUS, &success);
	if(!success)
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
		const GLchar *message,
		const void *userParam)
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
	glfwSwapInterval(1);
	// NOTE: OpenGL error checks have been omitted for brevity

	if(!gladLoadGL()) {
		std::cerr << "Something went wrong!" << std::endl;
		exit(-1);
	}

	// Callbacks
	glDebugMessageCallback(opengl_error_callback, nullptr);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

	const size_t nParticules = 1000;
	const auto particules = MakeParticules(nParticules);

	// Shader
	const auto vertex = MakeShader(GL_VERTEX_SHADER, "shader.vert");
	const auto fragment = MakeShader(GL_FRAGMENT_SHADER, "shader.frag");

	const auto program = AttachAndLink({vertex, fragment});

	glUseProgram(program);


	// Buffers
	GLuint vbo, vao;
	glGenBuffers(1, &vbo);
	glGenVertexArrays(1, &vao);

	std::vector<Triangle> logo = ReadStl("logo.stl");
	glm::mat4 mat, modelView;

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, logo.size() * sizeof(glm::vec3) * 3, logo.data(), GL_STATIC_DRAW);

	// Bindings
	const auto index = glGetAttribLocation(program, "position");
	//const auto indexColor = glGetAttribLocation(program, "color");
	const auto indexScale = glGetUniformLocation(program, "scale");

	glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3) , nullptr);
	//glVertexAttribPointer(indexColor, 3, GL_FLOAT, GL_FALSE, sizeof(Particule) , (void *) (sizeof(float) * 3));
	
	glEnableVertexAttribArray(index);
	//glEnableVertexAttribArray(indexColor);

	glPointSize(20.f);
	float val = 0.01f;
	int modScale = 0;
	double xPos, yPos;
	modelView = glm::mat4(val, 0.0f, 0.0f, 0.0f, 0.0f, val, 0.0f, 0.0f, 0.0f, 0.0f, val, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	glProgramUniformMatrix4fv(program, indexScale, 1, GL_FALSE, &mat[0][0]);
	glm::vec3 oldMousePos;

	while (!glfwWindowShouldClose(window))
	{
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glfwGetCursorPos(window, &xPos, &yPos);

		glViewport(0, 0, width, height);
		
		//glProgramUniform1f(program, indexScale, val);
		//modelView = glm::scale(glm::vec3(val, val, val));
		//modelView = glm::rotate(modelView, 0.01f, glm::vec3(0.0f, 0.0f, 1.0f));
		glProgramUniformMatrix4fv(program, indexScale, 1, GL_FALSE, &modelView[0][0]);
		
		
		if (modScale == 0)
			val += 0.0001f;
		else
			val -= 0.0001f;

		if (val >= 0.02f)
			modScale = 1;
		if (val <= 0.01f)
			modScale = 0;
		

		glClear(GL_COLOR_BUFFER_BIT);
		// glClearColor(1.0f, 0.0f, 0.0f, 1.0f);

		glDrawArrays(GL_TRIANGLES, 0, logo.size() * 3);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
