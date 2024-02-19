#include "utility/util.h"
#include "models.h"
#include "utility/transform.h"
#include "utility/camera.h"

#include <stdio.h>
#include <iostream>
#include <time.h>

#include <stb_image/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/quaternion.hpp>

struct CubePrefab
{
	union
	{
		Vertex vertices[24];
		Vertex faces[6][4];
	};
	unsigned int indices[36];
};

void mouseCallback(bool init, Camera* camera, double mouseDX, double mouseDY)
{
	static Camera* cam;
	if(init)
	{
		cam = camera;
		return;
	} 

	float dx = -(float)mouseDX;
	float dy = -(float)mouseDY;

	glm::vec3 up = cam->up();
	glm::vec3 right = cam->right();

	cam->rotateGlobal(up, dx / 200.0);
	cam->rotateGlobal(right, dy / 200.0);
}

void mouseCallbackProxy(GLFWwindow* window, double xpos, double ypos)
{
	static double prevXpos = xpos, prevYpos = ypos;
	double dx = xpos - prevXpos;
	double dy = ypos - prevYpos;
	prevXpos = xpos;
	prevYpos = ypos;

	mouseCallback(false, nullptr, dx, dy);
}

int main()
{
	const int SCREEN_WIDTH = 1280;
	const int SCREEN_HEIGHT = 720;
	GLFWwindow* window = initialize(SCREEN_WIDTH, SCREEN_HEIGHT, "OpenGL", 4, 6);

	glDebugMessageCallback(glDebugCallback, nullptr);
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glEnable(GL_DEPTH_TEST);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouseCallbackProxy);

	//-------------------------------------------------------------

	CubePrefab prefab;
	std::copy(models::cube::vertices, models::cube::vertices + 24, prefab.vertices);
	std::copy(models::cube::indices, models::cube::indices + 36, prefab.indices);

	for(size_t face = 0; face < 6; face++)
	{
		prefab.faces[face][0].textureCoordinate = {1.0f, 1.0f};
		prefab.faces[face][1].textureCoordinate = {0.0f, 1.0f};
		prefab.faces[face][2].textureCoordinate = {0.0f, 0.0f};
		prefab.faces[face][3].textureCoordinate = {1.0f, 0.0f};
	}

	Transform cubes[10];

	srand(time(0));
	for(Transform& cube : cubes)
	{
		cube.position.x = ((((float)rand() / (float)RAND_MAX) * 2.0f) - 1.0f) * 7.0f;
		cube.position.y = ((((float)rand() / (float)RAND_MAX) * 2.0f) - 1.0f) * 7.0f;
		cube.position.z = ((((float)rand() / (float)RAND_MAX) * 2.0f) - 1.0f) * 7.0f;
	}

	Camera camera{glm::radians(45.0f), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f};
	float camSpeed = 0.005f;
	mouseCallback(true, &camera, 0, 0);

	unsigned int vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(prefab.vertices), prefab.vertices, GL_DYNAMIC_DRAW);

	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindVertexBuffer(0, vbo, 0, sizeof(Vertex));

	glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
	glVertexAttribBinding(0, 0);
	glEnableVertexAttribArray(0);

	glVertexAttribFormat(1, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, textureCoordinate));
	glVertexAttribBinding(1, 0);
	glEnableVertexAttribArray(1);

	unsigned int ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(prefab.indices), prefab.indices, GL_STATIC_DRAW);

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
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "u_projection"), 
					   1, GL_FALSE, glm::value_ptr(camera.projectionMatrix()));

	unsigned int u_modelTransform = glGetUniformLocation(shaderProgram, "u_modelTransform");
	unsigned int u_view = glGetUniformLocation(shaderProgram, "u_view");

	float prevTime = glfwGetTime();
	float accumulator = 0.0f;

	while(!glfwWindowShouldClose(window))
	{
		float currentTime = glfwGetTime();
		float deltaTime = currentTime - prevTime;
		prevTime = currentTime;

		accumulator += deltaTime;

		if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			camera.position += camSpeed * -camera.behind();
		} 
		else if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			camera.position += camSpeed * camera.behind();
		}

		if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			camera.position += camSpeed * camera.right();
		} 
		else if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			camera.position += camSpeed * -camera.right();
		}

		if(accumulator >= 0.5f)
		{
			/*
			std::cout << glm::to_string(camera.right()) << '\n';
			std::cout << glm::to_string(camera.up()) << '\n';
			std::cout << glm::to_string(camera.behind()) << "\n\n";
			*/
			accumulator = 0.0f;
		}

		glm::mat4 viewMatrix = camera.viewMatrix();
		glUniformMatrix4fv(u_view, 1, GL_FALSE, glm::value_ptr(viewMatrix));

		glClearColor(0.12f, 0.12f, 0.12f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(vao);
		for(Transform& cube : cubes)
		{
			glm::mat4 modelMatrix = cube.matrix();
			glUniformMatrix4fv(u_modelTransform, 1, GL_FALSE, glm::value_ptr(modelMatrix));

			glDrawElements(GL_TRIANGLES, sizeof(prefab.indices) / sizeof(unsigned int), GL_UNSIGNED_INT, (const void*)0);
		}

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