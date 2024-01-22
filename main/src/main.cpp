#include "util.h"
#include "data.h"
#include "stb_image.h"

#include <stdio.h>

struct Vertex
{
	Vec3 position;
	Vec3 color;
	Vec3 textureCoord;
};

int main()
{
	if(glfwInit() != GLFW_TRUE)
	{
		printf("Failed to initialize GLFW\n");
		return -1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

	const int SCREEN_WIDTH = 640;
	const int SCREEN_HEIGHT = 540;

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "OpenGL", nullptr, nullptr);
	if(!window)
	{
		printf("Failed to create window\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	glfwSetFramebufferSizeCallback(window, frameBufferResize);

	if(glewInit() != GLEW_OK)
	{
		printf("Failed to initialize GLEW\n");
		glfwTerminate();
		return -1;
	}
	printf("Using OppenGL version : %s\n", glGetString(GL_VERSION));

	glDebugMessageCallback(glDebugCallback, nullptr);

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	//-------------------------------------------------------------

	Vertex vertices[] =
	{
		{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
		{{ 0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {2.0f, 0.0f, 0.0f}},
		{{ 0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {2.0f, 2.0f, 0.0f}},
		{{-0.5f,  0.5f, 0.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, 2.0f, 0.0f}},
	};

	unsigned int indices[] =
	{
		0, 1, 2,
		2, 3, 0
	};

	unsigned int vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindVertexBuffer(0, vbo, 0, sizeof(Vertex));

	glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
	glVertexAttribBinding(0, 0);
	glEnableVertexAttribArray(0);

	glVertexAttribFormat(1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, color));
	glVertexAttribBinding(1, 0);
	glEnableVertexAttribArray(1);

	glVertexAttribFormat(2, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, textureCoord));
	glVertexAttribBinding(2, 0);
	glEnableVertexAttribArray(2);

	unsigned int ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	unsigned int shaderProgram = createShaderProgram("../res/vertex_shader.shader", "../res/fragment_shader.shader");
	glUseProgram(shaderProgram);

	int imWidth, imHeight, imNumChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* imData = stbi_load("../res/texture2.jpeg", &imWidth, &imHeight, &imNumChannels, 0);

	unsigned int texture;
	glGenTextures(1, &texture);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imWidth, imHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, imData);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glUniform1i(glGetUniformLocation(shaderProgram, "u_sampler"), 0);

	unsigned int u_time = glGetUniformLocation(shaderProgram, "u_time");

	while(!glfwWindowShouldClose(window))
	{
		processInputs(window);

		glUniform1f(u_time, (float)glfwGetTime());

		glClearColor(0.12f, 0.12f, 0.12f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (const void*)0);

		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &ebo);
	glDeleteProgram(shaderProgram);
	glDeleteTextures(1, &texture);
	stbi_image_free(imData);
	glfwTerminate();

	return 0;
}