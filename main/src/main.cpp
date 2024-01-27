#include "util.h"
#include "square.h"
#include "cube.h"
#include "transform.h"

#include <stdio.h>

#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/quaternion.hpp>

int main()
{
	const int SCREEN_WIDTH = 640;
	const int SCREEN_HEIGHT = 480;
	GLFWwindow* window = initialize(SCREEN_WIDTH, SCREEN_HEIGHT, "OpenGL", 4, 6);

	glfwSetFramebufferSizeCallback(window, frameBufferResize);
	glDebugMessageCallback(glDebugCallback, nullptr);
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glEnable(GL_DEPTH_TEST);

	//-------------------------------------------------------------

	Prefab<CUBE> prefab;
	createPrefab<CUBE>(prefab);

	for(size_t face = 0; face < 6; face++)
	{
		prefab.faces[face][0].textureCoordinate = {1.0f, 1.0f};
		prefab.faces[face][1].textureCoordinate = {0.0f, 1.0f};
		prefab.faces[face][2].textureCoordinate = {0.0f, 0.0f};
		prefab.faces[face][3].textureCoordinate = {1.0f, 0.0f};
	}

	Transform trans{};
	trans.position = {0.0f, 0.0f, -3.0f};

	Transform view{};

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);

	unsigned int vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(prefab.vertices), prefab.vertices, GL_DYNAMIC_DRAW);

	unsigned int ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(prefab.indices), prefab.indices, GL_STATIC_DRAW);

	unsigned int vao;
	glGenVertexArrays(1, &vao);
	setVAOFormat<CUBE>(vao);
	attachVBO<CUBE>(vao, vbo, 0);
	attachEBO<CUBE>(vao, ebo);

	unsigned int shaderProgram = createShaderProgram("../res/vertex_shader.shader", "../res/fragment_shader.shader");
	glUseProgram(shaderProgram);

	TextureData texture = loadTexture("../res/texture3.jpeg");
	unsigned int textureID;
	glGenTextures(1, &textureID);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture.width, texture.height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture.data);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glUniform1i(glGetUniformLocation(shaderProgram, "u_sampler"), 0);

	unsigned int u_modelTransform = glGetUniformLocation(shaderProgram, "u_modelTransform");
	unsigned int u_view = glGetUniformLocation(shaderProgram, "u_view");
	unsigned int u_projection = glGetUniformLocation(shaderProgram, "u_projection");

	while(!glfwWindowShouldClose(window))
	{
		processInputs(window);

		trans.rotation = glm::angleAxis((float)glfwGetTime(), glm::normalize(glm::vec3{1.0f, 1.0f, -1.0f}));

		glUniformMatrix4fv(u_modelTransform, 1, GL_FALSE, glm::value_ptr(trans.matrix()));
		glUniformMatrix4fv(u_view, 1, GL_FALSE, glm::value_ptr(view.matrix()));
		glUniformMatrix4fv(u_projection, 1, GL_FALSE, glm::value_ptr(projection));

		glClearColor(0.12f, 0.12f, 0.12f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(vao);
		glDrawElementsInstanced(GL_TRIANGLES, sizeof(prefab.indices) / sizeof(unsigned int), 
								GL_UNSIGNED_INT, (const void*)0, 1);

		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &ebo);
	glDeleteProgram(shaderProgram);
	glDeleteTextures(1, &textureID);
	stbi_image_free(texture.data);
	glfwTerminate();

	return 0;
}