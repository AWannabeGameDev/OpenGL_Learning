#ifndef APPLICATION_H
#define APPLICATION_H

#include "util.h"
#include "models.h"
#include "transform.h"
#include "camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <iostream>

static void mousePositionCallbackProxy(GLFWwindow* window, double xpos, double ypos);
static void mouseButtonCallbackProxy(GLFWwindow* window, int button, int action, int mods);

class Application 
{
private :
	struct Cube
	{
		union
		{
			Vertex vertices[24];
			Vertex faces[6][4];
		};
		unsigned int indices[36];
	};

	const int SCREEN_WIDTH, SCREEN_HEIGHT;
	GLFWwindow* window;

	const float MOUSE_SENSITIVITY;
	Camera camera;

	Cube cubeModel;
	Transform cubeTransform;

	unsigned int vbo, ebo, vao;
	unsigned int shaderProgram;
	unsigned int u_modelMatrix;
	unsigned int u_viewMatrix;

	bool dragging;
	double mouseLastX, mouseLastY;

	float deltaTime, prevTime;

	friend static void mousePositionCallbackProxy(GLFWwindow* window, double xpos, double ypos);
	friend static void mouseButtonCallbackProxy(GLFWwindow* window, int button, int action, int mods);

public :
	Application() :
		SCREEN_WIDTH{1280}, SCREEN_HEIGHT{720}, window{initialize(SCREEN_WIDTH, SCREEN_HEIGHT, "OpenGL", 4, 6)},
		MOUSE_SENSITIVITY{6.0f}, camera{glm::radians(45.0f), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f},
		cubeTransform{},
		shaderProgram{createShaderProgram("../res/vertex_shader.shader", "../res/fragment_shader.shader")},
		u_modelMatrix{(unsigned int)glGetUniformLocation(shaderProgram, "u_modelMatrix")},
		u_viewMatrix{(unsigned int)glGetUniformLocation(shaderProgram, "u_viewMatrix")},
		dragging{false},
		prevTime{0}
	{
		glfwSetWindowUserPointer(window, this);
		glfwSetCursorPosCallback(window, mousePositionCallbackProxy);
		glfwSetMouseButtonCallback(window, mouseButtonCallbackProxy);

		glDebugMessageCallback(glDebugCallback, nullptr);
		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		glEnable(GL_DEPTH_TEST);

		std::copy(models::cube::vertices, models::cube::vertices + 24, cubeModel.vertices);
		std::copy(models::cube::indices, models::cube::indices + 36, cubeModel.indices);

		for(size_t vertex = 0; vertex < 4; vertex++)
		{
			cubeModel.faces[0][vertex].color = {1.0f, 0.0f, 0.0f, 1.0f}; // red
			cubeModel.faces[1][vertex].color = {1.0f, 0.65f, 0.0f, 1.0f}; // orange
			cubeModel.faces[2][vertex].color = {1.0f, 1.0f, 1.0f, 1.0f}; // white
			cubeModel.faces[3][vertex].color = {1.0f, 1.0f, 0.0f, 1.0f}; // yellow
			cubeModel.faces[4][vertex].color = {0.0f, 1.0f, 0.0f, 1.0f}; // green
			cubeModel.faces[5][vertex].color = {0.0f, 0.0f, 1.0f, 1.0f}; // blue
		}

		camera.position = {3.0f, 3.0f, 3.0f};
		camera.rotateGlobal(camera.up(), glm::radians(45.0f));
		camera.rotateGlobal(camera.right(), -atanf(1.0f / sqrtf(2.0f)));

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeModel.vertices), cubeModel.vertices, GL_STATIC_DRAW);

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		glBindVertexBuffer(0, vbo, 0, sizeof(Vertex));

		glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
		glVertexAttribBinding(0, 0);
		glEnableVertexAttribArray(0);

		glVertexAttribFormat(1, 4, GL_FLOAT, GL_FALSE, offsetof(Vertex, color));
		glVertexAttribBinding(1, 0);
		glEnableVertexAttribArray(1);

		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeModel.indices), cubeModel.indices, GL_STATIC_DRAW);

		glUseProgram(shaderProgram);

		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "u_projectionMatrix"),
						   1, GL_FALSE, glm::value_ptr(camera.projectionMatrix()));
	}

	~Application()
	{
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ebo);
		glDeleteVertexArrays(1, &vao);
		glDeleteProgram(shaderProgram);
		glfwTerminate();
	}

private :
	void mouseButtonCallback(int button, int action)
	{
		if(button == GLFW_MOUSE_BUTTON_LEFT)
		{
			if(action == GLFW_PRESS)
			{
				dragging = true;
				glfwGetCursorPos(window, &mouseLastX, &mouseLastY);
			}
			else
			{
				dragging = false;
			}
		}
	}

	void mousePositionCallback(double xpos, double ypos)
	{
		if(dragging)
		{
			float mouseDX = (float)(xpos - mouseLastX);
			float mouseDY = -(float)(ypos - mouseLastY);

			glm::vec3 mouseMove{mouseDX, mouseDY, 0.0f};
			mouseMove = glm::vec3{camera.inverseRotationMatrix() * glm::vec4{mouseMove, 1.0f}};

			glm::vec3 rotationAxis = glm::normalize(glm::cross(camera.behind(), mouseMove));
			float angle = mouseMove.length() * MOUSE_SENSITIVITY * deltaTime;

			cubeTransform.rotation = glm::angleAxis(angle, rotationAxis) * cubeTransform.rotation;

			mouseLastX = xpos;
			mouseLastY = ypos;
		}
	}

public :
	void run()
	{
		while(!glfwWindowShouldClose(window))
		{
			float currentTime = glfwGetTime();
			deltaTime = currentTime - prevTime;
			prevTime = currentTime;

			glfwPollEvents();

			glm::mat4 modelMatrix = cubeTransform.matrix();
			glUniformMatrix4fv(u_modelMatrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

			glm::mat4 viewMatrix = camera.viewMatrix();
			glUniformMatrix4fv(u_viewMatrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

			glClearColor(0.12f, 0.12f, 0.12f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glDrawElements(GL_TRIANGLES, sizeof(cubeModel.indices) / sizeof(unsigned int), GL_UNSIGNED_INT, (const void*)0);

			glfwSwapBuffers(window);
		}
	}
};

static void mousePositionCallbackProxy(GLFWwindow* window, double xpos, double ypos)
{
	((Application*)glfwGetWindowUserPointer(window))->mousePositionCallback(xpos, ypos);
}

static void mouseButtonCallbackProxy(GLFWwindow* window, int button, int action, int mods)
{
	((Application*)glfwGetWindowUserPointer(window))->mouseButtonCallback(button, action);
}

#endif