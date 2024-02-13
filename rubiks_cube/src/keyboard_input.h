#ifndef KEYBOARD_INPUT_H
#define KEYBOARD_INPUT_H

#include <unordered_map>
#include <GLFW/glfw3.h>

class KeyboardInput
{
private :
	struct KeyStatus
	{
		bool previous;
		bool current;
	};

	std::unordered_map<const char*, int> keyNameMap;
	std::unordered_map<int, KeyStatus> keyStatusMap;
	GLFWwindow* window;

public :
	KeyboardInput(GLFWwindow* glfwWindow);

	void setKeybind(const char* keyName, int glfwKey);
	void removeKeybind(const char* keyName);

	void update();

	bool keyJustPressed(const char* keyName);
	bool keyPressed(const char* keyName);
	bool keyJustReleased(const char* keyName);
	bool keyReleased(const char* keyName);

	bool anyKeyJustPressed();
	bool anyKeyPressed();
	bool anyKeyJustReleased();
	bool anyKeyReleased();
};

#endif