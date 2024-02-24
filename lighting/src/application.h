#ifndef APPLICATION_H
#define APPLICATION_H

#include <utility/util.h>
#include <utility/transform.h>
#include <utility/camera.h>
#include <utility/lighting.h>

class Application
{
private :
	const int WINDOW_WIDTH, WINDOW_HEIGHT;
	const char* WINDOW_TITLE;
	GLFWwindow* window;

	unsigned int cubeVAO, cubeVBO, cubeEBO;

	unsigned int lightShader;
	glm::vec4 lightColor;

	unsigned int objectShader;
	Material material;
	LightSource lightSrc;

	Camera camera;

	Transform object, lightSource;

public :
	Application();
	~Application();

	void run();
};

#endif