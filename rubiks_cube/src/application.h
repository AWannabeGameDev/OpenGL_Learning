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

class Application 
{
private :

	inline static constexpr size_t VOX_NUM_VERTICES = 24;
	inline static constexpr size_t VOX_NUM_INDICES = 36;
	inline static constexpr size_t NUM_CUBES = 27;
	inline static constexpr size_t NUM_CUBES_PER_FACE = 9;
	inline static constexpr size_t VOX_NUM_FACES = 6;
	inline static constexpr size_t VOX_NUM_VERTS_PER_FACE = 4;

	inline static constexpr glm::vec4 COLORS[VOX_NUM_FACES]
	{
		{0.0f, 0.0f, 1.0f, 1.0f}, // blue
		{0.0f, 1.0f, 0.0f, 1.0f}, // green
		{1.0f, 1.0f, 1.0f, 1.0f}, // white
		{1.0f, 1.0f, 0.0f, 1.0f}, // yellow
		{1.0f, 0.0f, 0.0f, 1.0f}, // red
		{1.0f, 0.65, 0.0f, 1.0f} // orange
	};

	inline static constexpr size_t COLOR_CUBE_INDICES[VOX_NUM_FACES][NUM_CUBES_PER_FACE][3]
	{
		// face +x
		{{0, 0, 0}, {0, 0, 1}, {0, 0, 2},
		{0, 1, 0}, {0, 1, 1}, {0, 1, 2},
		{0, 2, 0}, {0, 2, 1}, {0, 2, 2}},

		// face -x
		{{2, 0, 0}, {2, 0, 1}, {2, 0, 2},
		{2, 1, 0}, {2, 1, 1}, {2, 1, 2},
		{2, 2, 0}, {2, 2, 1}, {2, 2, 2}},

		// face +y
		{{0, 0, 0}, {0, 0, 1}, {0, 0, 2},
		{1, 0, 0}, {1, 0, 1}, {1, 0, 2},
		{2, 0, 0}, {2, 0, 1}, {2, 0, 2}},

		// face -y
		{{0, 2, 0}, {0, 2, 1}, {0, 2, 2},
		{1, 2, 0}, {1, 2, 1}, {1, 2, 2},
		{2, 2, 0}, {2, 2, 1}, {2, 2, 2}},

		// face +z
		{{0, 0, 0}, {1, 0, 0}, {2, 0, 0},
		{0, 1, 0}, {1, 1, 0}, {2, 1, 0},
		{0, 2, 0}, {1, 2, 0}, {2, 2, 0}},

		// face -z
		{{0, 0, 2}, {1, 0, 2}, {2, 0, 2},
		{0, 1, 2}, {1, 1, 2}, {2, 1, 2},
		{0, 2, 2}, {1, 2, 2}, {2, 2, 2}}
	};

	union Cube
	{
		glm::vec3 positions[VOX_NUM_VERTICES];
		glm::vec3 faces[VOX_NUM_FACES][VOX_NUM_VERTS_PER_FACE];
	};

	union CubeColors
	{
		glm::vec4 colors[VOX_NUM_VERTICES];
		glm::vec4 faces[VOX_NUM_FACES][VOX_NUM_VERTS_PER_FACE];
	};

	const int SCREEN_WIDTH, SCREEN_HEIGHT;
	GLFWwindow* window;

	const float MOUSE_SENSITIVITY;
	Camera camera;

	Cube cubeModel;
	unsigned int cubeIndices[VOX_NUM_INDICES];
	
	union
	{
		Transform cubeTransformsMatrix[3][3][3]; // x-index, y-index, z-index
		Transform cubeTransforms[27];
	};
	CubeColors cubeColors[3][3][3];

	unsigned int positionVbo, colorVbo, ebo, vao;
	unsigned int shaderProgram;
	unsigned int u_modelMatrix;
	unsigned int u_viewMatrix;

	bool dragging;
	double mouseLastX, mouseLastY;

	float deltaTime, prevTime;

	static void mousePositionCallbackProxy(GLFWwindow* window, double xpos, double ypos);
	static void mouseButtonCallbackProxy(GLFWwindow* window, int button, int action, int mods);


	void mouseButtonCallback(int button, int action);
	void mousePositionCallback(double xpos, double ypos);

public :
	Application();
	~Application();

	void run();
};

#ifdef APPLICATION_IMPL

Application::Application() :
	SCREEN_WIDTH{1280}, SCREEN_HEIGHT{720}, window{initialize(SCREEN_WIDTH, SCREEN_HEIGHT, "OpenGL", 4, 6)},
	MOUSE_SENSITIVITY{6.0f}, camera{glm::radians(45.0f), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f},
	cubeTransforms{}, cubeColors{},
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

	std::copy(models::cube::positions, models::cube::positions + 24, cubeModel.positions);
	std::copy(models::cube::indices, models::cube::indices + 36, cubeIndices);

	for(size_t face = 0; face < VOX_NUM_FACES; face++)
	{
		for(size_t cube = 0; cube < NUM_CUBES_PER_FACE; cube++)
		{
			CubeColors& cubeColorSet = cubeColors[COLOR_CUBE_INDICES[face][cube][0]]
				[COLOR_CUBE_INDICES[face][cube][1]]
				[COLOR_CUBE_INDICES[face][cube][2]];

			for(size_t vertex = 0; vertex < VOX_NUM_VERTS_PER_FACE; vertex++)
			{
				cubeColorSet.faces[face][vertex] = COLORS[face];
			}
		}
	}

	for(size_t x = 0; x < 3; x++)
	{
		for(size_t y = 0; y < 3; y++)
		{
			for(size_t z = 0; z < 3; z++)
				cubeTransformsMatrix[x][y][z].position = 
				1.03f * glm::vec3{1.0f - (float)x, 1.0f - (float)y, 1.0f - (float)z};
		}
	}

	camera.position = {5.0f, 5.0f, 5.0f};
	camera.rotateGlobal(camera.up(), glm::radians(45.0f));
	camera.rotateGlobal(camera.right(), -atanf(1.0f / sqrtf(2.0f)));

	glGenBuffers(1, &positionVbo);
	glBindBuffer(GL_ARRAY_BUFFER, positionVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeModel.positions), cubeModel.positions, GL_STATIC_DRAW);

	glGenBuffers(1, &colorVbo);
	glBindBuffer(GL_ARRAY_BUFFER, colorVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeColors), cubeColors, GL_STATIC_DRAW);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindVertexBuffer(0, positionVbo, 0, sizeof(glm::vec3));

	glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(0, 0);
	glEnableVertexAttribArray(0);

	glVertexAttribFormat(1, 4, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(1, 1);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

	glUseProgram(shaderProgram);

	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "u_projectionMatrix"),
					   1, GL_FALSE, glm::value_ptr(camera.projectionMatrix()));
}

Application::~Application()
{
	glDeleteBuffers(1, &positionVbo);
	glDeleteBuffers(1, &colorVbo);
	glDeleteBuffers(1, &ebo);
	glDeleteVertexArrays(1, &vao);
	glDeleteProgram(shaderProgram);
	glfwTerminate();
}

void Application::mouseButtonCallback(int button, int action)
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

void Application::mousePositionCallback(double xpos, double ypos)
{
	if(dragging)
	{
		float mouseDX = (float)(xpos - mouseLastX);
		float mouseDY = -(float)(ypos - mouseLastY);

		glm::vec3 mouseMove{mouseDX, mouseDY, 0.0f};
		mouseMove = glm::vec3{camera.inverseRotationMatrix() * glm::vec4{mouseMove, 1.0f}};

		glm::vec3 rotationAxis = glm::normalize(glm::cross(camera.behind(), mouseMove));
		float angle = mouseMove.length() * MOUSE_SENSITIVITY * deltaTime;

		for(Transform& transform : cubeTransforms)
		{
			glm::quat rotate = glm::angleAxis(angle, rotationAxis);
			transform.position = rotate * transform.position;
			transform.rotation = rotate * transform.rotation;
		}

		mouseLastX = xpos;
		mouseLastY = ypos;
	}
}


void Application::run()
{
	while(!glfwWindowShouldClose(window))
	{
		float currentTime = (float)glfwGetTime();
		deltaTime = currentTime - prevTime;
		prevTime = currentTime;

		glfwPollEvents();

		glm::mat4 viewMatrix = camera.viewMatrix();
		glUniformMatrix4fv(u_viewMatrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

		glClearColor(0.12f, 0.12f, 0.12f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(vao);

		for(size_t i = 0; i < NUM_CUBES; i++)
		{
			glBindVertexBuffer(1, colorVbo, i * VOX_NUM_VERTICES * sizeof(glm::vec4), sizeof(glm::vec4));

			glm::mat4 modelMatrix = cubeTransforms[i].matrix();
			glUniformMatrix4fv(u_modelMatrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

			glDrawElements(GL_TRIANGLES, sizeof(cubeIndices) / sizeof(unsigned int), GL_UNSIGNED_INT, (const void*)0);
		}

		glfwSwapBuffers(window);
	}
}

void Application::mousePositionCallbackProxy(GLFWwindow* window, double xpos, double ypos)
{
	((Application*)glfwGetWindowUserPointer(window))->mousePositionCallback(xpos, ypos);
}

void Application::mouseButtonCallbackProxy(GLFWwindow* window, int button, int action, int mods)
{
	((Application*)glfwGetWindowUserPointer(window))->mouseButtonCallback(button, action);
}

#endif

#endif