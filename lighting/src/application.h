#ifndef APPLICATION_H
#define APPLICATION_H

#include <utility/util.h>
#include <utility/transform.h>
#include <utility/camera.h>
#include <utility/lighting.h>
#include <utility/keyboard_input.h>
#include <utility/mouse_input.h>
#include <utility/mesh.h>
#include <utility/model.h>

class Application
{
private :
	const int WINDOW_WIDTH, WINDOW_HEIGHT;
	const char* WINDOW_TITLE;
	GLFWwindow* window;

	KeyboardInput keys;
	MouseInput mouse;
	float prevTime;
	const float CAMERA_SPEED;
	const float CAMERA_SENSITIVITY;

	unsigned int cubeVAO, cubeVBO, cubeEBO;
	Mesh cubeMesh;
	Model backpackModel;

	unsigned int lightShader;
	glm::vec4 lightColor;

	unsigned int objectShader;
	Material material;
	static constexpr int DIFFUSE_MAP_INDEX = 0,
						 SPECULAR_MAP_INDEX = 1,
						 EMISSION_MAP_INDEX = 2;

	glm::vec4 ambientColor;
	PointLight pointLightSrc;
	DirectionalLight dirLightSrc;
	ConeLight coneLightSrc;

	Camera camera;
	Transform objectTransforms[100]; 
	Transform lightSrcTransform;

public :
	Application();
	~Application();

	void run();
};

#endif