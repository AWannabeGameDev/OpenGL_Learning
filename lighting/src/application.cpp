#include "application.h"
#include <utility/model.h>
#include <glm/gtc/type_ptr.hpp>
#include <ctime>
#include <utility/profiler.h>

float randrange(float min, float max)
{
	return (((float)rand() * (max - min) / RAND_MAX) + min);
}

Application::Application() :
	WINDOW_WIDTH{1280}, WINDOW_HEIGHT{720}, WINDOW_TITLE{"LIGHTING"}, 
	window{initialize(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, 4, 6)},

	keys{window},
	mouse{window},
	prevTime{0.0f},
	CAMERA_SPEED{7.0f}, CAMERA_SENSITIVITY{0.4f},

	lightShader{createShaderProgram("../res/light_vert_shader.glsl", "../res/light_frag_shader.glsl")},
	lightColor{1.0f, 1.0f, 1.0f, 1.0f},

	objectShader{createShaderProgram("../res/obj_vert_shader.glsl", "../res/obj_frag_shader.glsl")},
	material{.shininess{32.0f}, .emissionColor{0.7f, 0.7f, 0.7f, 1.0f}},

	ambientColor{0.1f, 0.1f, 0.1f, 1.0f},
	pointLightSrc{{2.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, lightColor, 1.0f, 0.045f, 0.0075f},
	dirLightSrc{{1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.2f, 0.2f, 0.2f, 1.0f}},
	coneLightSrc{{0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, 0.0f}, cosf(glm::radians(15.0f)),
				 {1.0f, 1.0f, 1.0f, 1.0f}, {0.3f, 0.3f, 0.3f, 1.0f}, 1.0f, 0.045f, 0.0075f},

	camera{glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f},

	objectTransforms{}, lightSrcTransform{}, backpackModel{"../res/backpack/backpack.obj"}
{
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

#ifdef _DEBUG
	glDebugMessageCallback(glDebugCallback, nullptr);
#endif
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glEnable(GL_DEPTH_TEST);

	keys.setKeybind("FORWARD", GLFW_KEY_W);
	keys.setKeybind("BACKWARD", GLFW_KEY_S);
	keys.setKeybind("LEFT", GLFW_KEY_A);
	keys.setKeybind("RIGHT", GLFW_KEY_D);
	keys.setKeybind("UP", GLFW_KEY_SPACE);
	keys.setKeybind("DOWN", GLFW_KEY_LEFT_SHIFT);
	mouse.setButtonBind("FLASHLIGHT", GLFW_MOUSE_BUTTON_LEFT);

	glUseProgram(objectShader);
	backpackModel.setTextures(objectShader);
	setUniform(objectShader, "u_projection", camera.projectionMatrix());
	setUniform(objectShader, "u_material.shininess", material.shininess);
	setUniform(objectShader, "u_material.emissionColor", material.emissionColor);
	
	glUseProgram(lightShader);
	setUniform(lightShader, "u_projection", camera.projectionMatrix());

	camera.position = {0.0f, 0.0f, 5.0f};
	lightSrcTransform.position = pointLightSrc.position;
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
		float currentTime = (float)glfwGetTime();
		float deltaTime = currentTime - prevTime;
		prevTime = currentTime;
		
		glfwPollEvents();
		keys.update();
		mouse.update();

		if(keys.keyPressed("FORWARD"))
			camera.position += -camera.behind() * CAMERA_SPEED * deltaTime;
		else if(keys.keyPressed("BACKWARD"))
			camera.position += camera.behind() * CAMERA_SPEED * deltaTime;
		if(keys.keyPressed("LEFT"))
			camera.position += -camera.right() * CAMERA_SPEED * deltaTime;
		else if(keys.keyPressed("RIGHT"))
			camera.position += camera.right() * CAMERA_SPEED * deltaTime;
		if(keys.keyPressed("UP"))
			camera.position += camera.up() * CAMERA_SPEED * deltaTime;
		else if(keys.keyPressed("DOWN"))
			camera.position += -camera.up() * CAMERA_SPEED * deltaTime;

		if(mouse.buttonPressed("FLASHLIGHT"))
		{
			coneLightSrc.diffuseColor = {1.0f, 1.0f, 1.0f, 1.0f};
			coneLightSrc.specularColor = {0.3f, 0.3f, 0.3f, 1.0f};
		}
		else
		{
			coneLightSrc.diffuseColor *= 0.0f;
			coneLightSrc.specularColor *= 0.0f;
		}

		glm::vec3 camUp = camera.up();
		glm::vec3 camRight = camera.right();

		float mouseMoveX = mouse.getMouseMovementX();
		if(mouseMoveX != 0)
			camera.rotateGlobal(camUp, -mouseMoveX * CAMERA_SENSITIVITY * deltaTime);

		float mouseMoveY = mouse.getMouseMovementY();
		if(mouseMoveY != 0)
			camera.rotateGlobal(camRight, mouseMoveY * CAMERA_SENSITIVITY * deltaTime);

		lightColor = glm::rotate(glm::mat4{1.0f}, 0.001f, {1.0f, 1.0f, 0.0f}) * lightColor;
		pointLightSrc.specularColor = lightColor;

		coneLightSrc.position = camera.position;
		coneLightSrc.direction = -camera.behind();

		glUseProgram(lightShader);
		setUniform(lightShader, "u_model", lightSrcTransform.matrix());
		setUniform(lightShader, "u_view", camera.viewMatrix());
		setUniform(lightShader, "u_color", lightColor);

		glUseProgram(objectShader);
		setUniform(objectShader, "u_view", camera.viewMatrix());
		setUniform(objectShader, "u_viewPosWorld", camera.position);
		setUniform(objectShader, "u_ambientColor", ambientColor);
		setUniform(objectShader, "u_pointLightSrc", pointLightSrc);
		setUniform(objectShader, "u_dirLightSrc", dirLightSrc);
		setUniform(objectShader, "u_coneLightSrc", coneLightSrc);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		setUniform(objectShader, "u_normalMatrix", glm::mat3(glm::transpose(glm::inverse(objectTransforms[0].matrix()))));
		setUniform(objectShader, "u_model", objectTransforms[0].matrix());
		backpackModel.draw(objectShader);

		glfwSwapBuffers(window);
	}
}