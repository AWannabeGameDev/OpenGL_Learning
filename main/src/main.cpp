#include "util.h"
#include "data.h"
#include "stb_image.h"
#include <stdio.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Vertex
{
	glm::vec4 position;
	glm::vec4 color;
	glm::vec4 textureCoord;
};

int main()
{
	const int SCREEN_WIDTH = 640;
	const int SCREEN_HEIGHT = 480;
	GLFWwindow* window = initialize(SCREEN_WIDTH, SCREEN_HEIGHT, "OpenGL", 4, 6);

	glfwSetFramebufferSizeCallback(window, frameBufferResize);
	glDebugMessageCallback(glDebugCallback, nullptr);
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	//-------------------------------------------------------------

	Vertex vertices[] =
	{
		{{-0.5f, -0.5f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}},
		{{ 0.5f, -0.5f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
		{{ 0.5f,  0.5f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 0.0f, 1.0f}},
		{{-0.5f,  0.5f, 0.0f, 1.0f}, {0.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
	};

	unsigned int indices[] =
	{
		0, 1, 2,
		2, 3, 0
	};

	glm::mat4 positionTransform {1.f};
	glm::mat4 texTransform{1.f};

	unsigned int vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindVertexBuffer(0, vbo, 0, sizeof(Vertex));

	glVertexAttribFormat(0, 4, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
	glVertexAttribBinding(0, 0);
	glEnableVertexAttribArray(0);

	glVertexAttribFormat(1, 4, GL_FLOAT, GL_FALSE, offsetof(Vertex, color));
	glVertexAttribBinding(1, 0);
	glEnableVertexAttribArray(1);

	glVertexAttribFormat(2, 4, GL_FLOAT, GL_FALSE, offsetof(Vertex, textureCoord));
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
	unsigned int u_positionTransform = glGetUniformLocation(shaderProgram, "u_positionTransform");
	unsigned int u_texTransform = glGetUniformLocation(shaderProgram, "u_texTransform");

	while(!glfwWindowShouldClose(window))
	{
		processInputs(window);

		positionTransform = glm::rotate(positionTransform, glm::radians(0.005f), {0.0f, 0.0f, 1.0f});

		texTransform = glm::translate(texTransform, {-0.5f, -0.5f, 0.0f});
		texTransform = glm::rotate(texTransform, glm::radians(0.005f), {0.0f, 0.0f, 1.0f});
		texTransform = glm::translate(texTransform, {0.5f, 0.5f, 0.0f});

		glUniform1f(u_time, (float)glfwGetTime());
		glUniformMatrix4fv(u_positionTransform, 1, GL_FALSE, glm::value_ptr(positionTransform));
		glUniformMatrix4fv(u_texTransform, 1, GL_FALSE, glm::value_ptr(texTransform));

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