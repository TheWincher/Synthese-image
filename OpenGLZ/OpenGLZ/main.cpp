#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
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

	glEnable(GL_DEPTH_TEST);
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

	std::vector< glm::vec3 > vertices;
	std::vector< glm::vec2 > uvs;
	std::vector< glm::vec3 > normals;
	bool res = loadOBJ("Baumstamm.obj", vertices, uvs, normals);

	// Buffers
	GLuint vbo, vao;
	glGenBuffers(1, &vbo);
	glGenVertexArrays(1, &vao);
	int bufferOffset = 0;

	//std::vector<Triangle> logo = ReadStl("logo.stl");


	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3) + uvs.size() * sizeof(glm::vec2), vertices.data(), GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), uvs.size() * sizeof(glm::vec2), uvs.data());
	//glBufferData(GL_ARRAY_BUFFER, logo.size() * sizeof(glm::vec3) * 3, logo.data(), GL_STATIC_DRAW);

	glm::mat4 modelView, view, projection;

	//Camera
	cameraPos = glm::vec3(0.0f, 0.0f, -3.0f);
	cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
	cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	// Bindings
	const auto index = glGetAttribLocation(program, "position");
	glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
	glEnableVertexAttribArray(index);

	const auto indexModelView = glGetUniformLocation(program, "modelView");
	const auto indexView = glGetUniformLocation(program, "view");
	const auto indexProjection = glGetUniformLocation(program, "projection");

	const auto indexUV = glGetAttribLocation(program, "uv");
	glVertexAttribPointer(indexUV, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*) (sizeof(glm::vec3) * vertices.size()));
	glEnableVertexAttribArray(indexUV);

	//Textures
	Image imgTexture = LoadImage("Baumstamm.bmp");
	GLuint textureID;
	glCreateTextures(GL_TEXTURE_2D, 1, &textureID);
	glTextureStorage2D(textureID, 1, GL_RGB8, imgTexture.width, imgTexture.height);
	glTextureSubImage2D(textureID, 0, 0, 0, imgTexture.width, imgTexture.height, GL_RGB, GL_UNSIGNED_BYTE, imgTexture.data.data());

	const auto texture = glGetUniformLocation(program, "texScreen");
	glBindTextureUnit(0, textureID);
	glProgramUniform1i(program, texture, 0);

	GLuint textureFrameBuffer = 1;
	glCreateTextures(GL_TEXTURE_2D, 1, &textureFrameBuffer);
	glTextureStorage2D(textureFrameBuffer, 1, GL_RGB8, imgTexture.width, imgTexture.height);

	GLuint textureDepthFrameBuffer = 2;
	glCreateTextures(GL_TEXTURE_2D, 1, &textureDepthFrameBuffer);
	glTextureStorage2D(textureDepthFrameBuffer, 1, GL_DEPTH_COMPONENT24, imgTexture.width, imgTexture.height);

	//FrameBuffer
	GLuint frameID = 1;
	glCreateFramebuffers(1, &frameID);
	glNamedFramebufferTexture(frameID, GL_COLOR_ATTACHMENT0, textureFrameBuffer, 0);
	glNamedFramebufferTexture(frameID, GL_DEPTH_ATTACHMENT, textureDepthFrameBuffer, 0);
	GLenum frameStatut = glCheckNamedFramebufferStatus(frameID, GL_FRAMEBUFFER);
	//if (frameStatut == GL_FRAMEBUFFER_COMPLETE)

	glPointSize(20.f);
	float val = 0.3f;
	modelView = glm::scale(glm::vec3(0.5f,0.5f,0.5f));

	while (!glfwWindowShouldClose(window))
	{

		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		projection = glm::perspective(90.0f, 1.86f, 0.1f, 5.0f);
		glProgramUniformMatrix4fv(program, indexProjection, 1, GL_FALSE, &projection[0][0]);

		view = glm::lookAt(cameraPos,cameraPos + cameraFront , cameraUp);
		glProgramUniformMatrix4fv(program, indexView, 1, GL_FALSE, &view[0][0]);

		modelView = glm::rotate(modelView, 0.01f, glm::vec3(1.0f, 1.0f, 0.0f));
		glProgramUniformMatrix4fv(program, indexModelView, 1, GL_FALSE, &modelView[0][0]);

		//1ere passe
		glBindFramebuffer(GL_FRAMEBUFFER, frameID);
		glViewport(0, 0, imgTexture.width, imgTexture.height);
		glBindTextureUnit(0, textureID);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, vertices.size() * 3);
		
		//2eme passe
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, width, height);
		
		//glBindTextureUnit(0, textureID);
		glBindTextureUnit(1, textureFrameBuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, vertices.size() * 3);

		/*glBindTextureUnit(0, textureID);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, vertices.size() * 3);*/

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
