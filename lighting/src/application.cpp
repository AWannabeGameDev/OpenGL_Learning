#include "application.h"
#include <utility/models.h>
#include <glm/gtc/type_ptr.hpp>

Application::Application() :
	WINDOW_WIDTH{1280}, WINDOW_HEIGHT{720}, WINDOW_TITLE{"LIGHTING"}, 
	window{initialize(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, 4, 6)},

	lightShader{createShaderProgram("../res/light_vert_shader.glsl", "../res/light_frag_shader.glsl")},
	lightColor{0.2f, 0.9f, 0.9f, 1.0f},

	objectShader{createShaderProgram("../res/obj_vert_shader.glsl", "../res/obj_frag_shader.glsl")},
	material{{1.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, 32.0f},
	lightSrc{{0.0f, 0.0f, -5.0f}, {0.1f, 0.1f, 0.1f, 1.0f}, {0.5f, 0.5f, 0.5f, 1.0f}, lightColor},

	camera{glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f},

	object{}, lightSource{}
{
	glDebugMessageCallback(glDebugCallback, nullptr);
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glEnable(GL_DEPTH_TEST);

	glGenBuffers(1, &cubeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(models::cube::vertices) + sizeof(models::cube::normals), nullptr, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(models::cube::vertices), models::cube::vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(models::cube::vertices), sizeof(models::cube::normals), models::cube::normals);

	glGenBuffers(1, &cubeEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(models::cube::indices), models::cube::indices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &cubeVAO);
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
	glBindVertexBuffer(0, cubeVBO, 0, sizeof(glm::vec3));
	glBindVertexBuffer(1, cubeVBO, sizeof(models::cube::vertices), sizeof(glm::vec3));

	glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(0, 0);
	glEnableVertexAttribArray(0);

	glVertexAttribFormat(1, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(1, 1);
	glEnableVertexAttribArray(1);

	glUseProgram(objectShader);
	setUniform(objectShader, "u_projection", camera.projectionMatrix());
	glUseProgram(lightShader);
	setUniform(lightShader, "u_projection", camera.projectionMatrix());

	camera.position = {0.0f, 0.0f, 5.0f};
	object.position = {0.0f, 0.0f, 0.0f};
	lightSource.position = lightSrc.position;
}

Application::~Application()
{
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteBuffers(1, &cubeVBO);
	glDeleteBuffers(1, &cubeEBO);
	glDeleteProgram(lightShader);
	glDeleteProgram(objectShader);
	glfwTerminate();
}

void Application::run()
{
	while(!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		{
			object.position.x += 0.003f;
		}
		else if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		{
			object.position.x -= 0.003f;
		}

		if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		{
			object.position.y += 0.003f;
		}
		else if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		{
			object.position.y -= 0.003f;
		}

		lightSource.position.x = 3.0f * sinf(30.0f * glm::radians((float)glfwGetTime()));
		lightSource.position.y = 3.0f * cosf(30.0f * glm::radians((float)glfwGetTime()));
		lightSrc.position = lightSource.position;
		lightColor = glm::rotate(glm::mat4{1.0f}, 0.001f, {1.0f, 1.0f, 0.0f}) * lightColor;
		lightSrc.specularColor = lightColor;

		glClear(GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glBindVertexArray(cubeVAO);

		glUseProgram(lightShader);
		setUniform(lightShader, "u_model", lightSource.matrix());
		setUniform(lightShader, "u_view", camera.viewMatrix());
		setUniform(lightShader, "u_color", lightColor);
		glDrawElements(GL_TRIANGLES, models::cube::NUM_INDICES, GL_UNSIGNED_INT, (const void*)0);

		glUseProgram(objectShader);
		setUniform(objectShader, "u_model", object.matrix());
		setUniform(objectShader, "u_view", camera.viewMatrix());
		setUniform(objectShader, "u_material.ambientColor", material.ambientColor);
		setUniform(objectShader, "u_material.diffuseColor", material.diffuseColor);
		setUniform(objectShader, "u_material.specularColor", material.specularColor);
		setUniform(objectShader, "u_material.shininess", material.shininess);
		setUniform(objectShader, "u_lightSrc.position", lightSrc.position);
		setUniform(objectShader, "u_lightSrc.ambientColor", lightSrc.ambientColor);
		setUniform(objectShader, "u_lightSrc.diffuseColor", lightSrc.diffuseColor);
		setUniform(objectShader, "u_lightSrc.specularColor", lightSrc.specularColor);
		setUniform(objectShader, "u_viewPosWorld", camera.position);
		glDrawElements(GL_TRIANGLES, models::cube::NUM_INDICES, GL_UNSIGNED_INT, (const void*)0);

		glfwSwapBuffers(window);
	}
}