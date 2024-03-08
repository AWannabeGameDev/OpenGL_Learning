#include "application.h"
#include <utility/models.h>
#include <glm/gtc/type_ptr.hpp>
#include <ctime>

static constexpr glm::vec2 texCoords[24] =
{
	{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f},
	{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f},
	{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f},
	{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f},
	{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f},
	{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}
};

float randrange(float min, float max)
{
	return ((float)rand() * (max - min) / RAND_MAX) + min;
}

Application::Application() :
	WINDOW_WIDTH{1280}, WINDOW_HEIGHT{720}, WINDOW_TITLE{"LIGHTING"}, 
	window{initialize(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, 4, 6)},

	keys{window},
	prevTime{0.0f},

	lightShader{createShaderProgram("../res/light_vert_shader.glsl", "../res/light_frag_shader.glsl")},
	lightColor{1.0f, 1.0f, 1.0f, 1.0f},
	CAMERA_SPEED{7.0f},

	objectShader{createShaderProgram("../res/obj_vert_shader.glsl", "../res/obj_frag_shader.glsl")},
	material{loadTexture("../res/container_diffuse.png"), loadTexture("../res/container_specular.png"), 32.0f,
			 loadTexture("../res/black.png"), {0.7f, 0.7f, 0.7f, 1.0f}},

	ambientColor{0.1f, 0.1f, 0.1f, 1.0f},
	pointLightSrc{{2.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, lightColor, 1.0f, 0.14f, 0.07f},
	dirLightSrc{{1.0f, -1.0f, -1.0f}, {0.4f, 0.4f, 0.4f, 1.0f}, {0.2f, 0.2f, 0.2f, 1.0f}},

	camera{glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f},

	objectTransforms{}, lightSrcTransform{}
{
	glDebugMessageCallback(glDebugCallback, nullptr);
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glEnable(GL_DEPTH_TEST);

	keys.setKeybind("FORWARD", GLFW_KEY_W);
	keys.setKeybind("BACKWARD", GLFW_KEY_S);
	keys.setKeybind("LEFT", GLFW_KEY_A);
	keys.setKeybind("RIGHT", GLFW_KEY_D);

	glGenBuffers(1, &cubeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(models::cube::vertices) + sizeof(models::cube::normals) + sizeof(texCoords), 
				 nullptr, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(models::cube::vertices), models::cube::vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(models::cube::vertices), sizeof(models::cube::normals), models::cube::normals);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(models::cube::vertices) + sizeof(models::cube::normals), 
					sizeof(texCoords), texCoords);

	glGenBuffers(1, &cubeEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(models::cube::indices), models::cube::indices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &cubeVAO);
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
	glBindVertexBuffer(0, cubeVBO, 0, sizeof(glm::vec3));
	glBindVertexBuffer(1, cubeVBO, sizeof(models::cube::vertices), sizeof(glm::vec3));
	glBindVertexBuffer(2, cubeVBO, sizeof(models::cube::vertices) + sizeof(models::cube::normals), sizeof(glm::vec2));

	glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(0, 0);
	glEnableVertexAttribArray(0);

	glVertexAttribFormat(1, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(1, 1);
	glEnableVertexAttribArray(1);

	glVertexAttribFormat(2, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(2, 2);
	glEnableVertexAttribArray(2);	

	auto createMap = [&](unsigned int& id, int index, const TextureData& map, int format) -> void
		{
			glGenTextures(1, &id);
			glActiveTexture(GL_TEXTURE0 + index);
			glBindTexture(GL_TEXTURE_2D, id);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, format,
						 map.width, map.height, 0,
						 format, GL_UNSIGNED_BYTE, map.data);
		};

	unsigned int diffuseMapID;
	createMap(diffuseMapID, DIFFUSE_MAP_INDEX, material.diffuseMap, GL_RGBA);

	unsigned int specularMapID;
	createMap(specularMapID, SPECULAR_MAP_INDEX, material.specularMap, GL_RGBA);

	unsigned int emmisionMapID;
	createMap(emmisionMapID, EMISSION_MAP_INDEX, material.emissionMap, GL_RGB);

	glUseProgram(objectShader);
	setUniform(objectShader, "u_projection", camera.projectionMatrix());
	setUniform(objectShader, "u_material.diffuseMap", DIFFUSE_MAP_INDEX);
	setUniform(objectShader, "u_material.specularMap", SPECULAR_MAP_INDEX);
	setUniform(objectShader, "u_material.emissionMap", EMISSION_MAP_INDEX);
	setUniform(objectShader, "u_material.shininess", material.shininess);
	setUniform(objectShader, "u_material.emissionColor", material.emissionColor);
	glUseProgram(lightShader);
	setUniform(lightShader, "u_projection", camera.projectionMatrix());

	camera.position = {0.0f, 0.0f, 5.0f};
	
	srand((unsigned int)std::time(0));
	for(Transform& transform : objectTransforms)
	{
		transform.position = {randrange(-10.0f, 10.0f), randrange(-10.0f, 10.0f), randrange(-10.0f, 10.0f)};
		transform.rotation = glm::angleAxis(randrange(glm::radians(-180.0f), glm::radians(180.0f)),
											glm::normalize(glm::vec3{randrange(-1.0f, 1.0f), 
																	 randrange(-1.0f, 1.0f), 
																	 randrange(-1.0f, 1.0f)}));
	}

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

		if(keys.keyPressed("FORWARD"))
			camera.position += -camera.behind() * CAMERA_SPEED * deltaTime;
		else if(keys.keyPressed("BACKWARD"))
			camera.position += camera.behind() * CAMERA_SPEED * deltaTime;
		if(keys.keyPressed("LEFT"))
			camera.position += -camera.right() * CAMERA_SPEED * deltaTime;
		else if(keys.keyPressed("RIGHT"))
			camera.position += camera.right() * CAMERA_SPEED * deltaTime;

		lightColor = glm::rotate(glm::mat4{1.0f}, 0.001f, {1.0f, 1.0f, 0.0f}) * lightColor;
		pointLightSrc.specularColor = lightColor;

		glClear(GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(cubeVAO);

		glUseProgram(lightShader);
		setUniform(lightShader, "u_model", lightSrcTransform.matrix());
		setUniform(lightShader, "u_view", camera.viewMatrix());
		setUniform(lightShader, "u_color", lightColor);
		glDrawElements(GL_TRIANGLES, models::cube::NUM_INDICES, GL_UNSIGNED_INT, (const void*)0);

		glUseProgram(objectShader);
		setUniform(objectShader, "u_view", camera.viewMatrix());
		setUniform(objectShader, "u_pointLightSrc.position", pointLightSrc.position);
		setUniform(objectShader, "u_ambientColor", ambientColor);
		setUniform(objectShader, "u_pointLightSrc.diffuseColor", pointLightSrc.diffuseColor);
		setUniform(objectShader, "u_pointLightSrc.specularColor", pointLightSrc.specularColor);
		setUniform(objectShader, "u_pointLightSrc.attenConst", pointLightSrc.attenConst);
		setUniform(objectShader, "u_pointLightSrc.attenLinear", pointLightSrc.attenLinear);
		setUniform(objectShader, "u_pointLightSrc.attenQuad", pointLightSrc.attenQuad);
		setUniform(objectShader, "u_dirLightSrc.direction", dirLightSrc.direction);
		setUniform(objectShader, "u_dirLightSrc.diffuseColor", dirLightSrc.diffuseColor);
		setUniform(objectShader, "u_dirLightSrc.specularColor", dirLightSrc.specularColor);
		setUniform(objectShader, "u_viewPosWorld", camera.position);

		for(Transform& transform : objectTransforms)
		{
			setUniform(objectShader, "u_normalMatrix", glm::mat3(glm::transpose(glm::inverse(transform.matrix()))));
			setUniform(objectShader, "u_model", transform.matrix());
			glDrawElements(GL_TRIANGLES, models::cube::NUM_INDICES, GL_UNSIGNED_INT, (const void*)0);
		}

		glfwSwapBuffers(window);
	}
}