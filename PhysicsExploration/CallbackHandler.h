#pragma once

#include "includes.h"

class CallbackHandler
{
public:

	static CallbackHandler* instance;
	virtual void setApp() { instance = this; }

	virtual void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	virtual void key_callback(GLFWwindow* window, int key, int scancode, int action, int mod);
	virtual void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);
	virtual void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	static void framebuffer_size_callback_dispatch(GLFWwindow* window, int width, int height)
	{
		if (instance)
			instance->framebuffer_size_callback(window, width, height);
	}
	static void key_callback_dispatch(GLFWwindow* window, int key, int scancode, int action, int mod)
	{
		if (instance)
			instance->key_callback(window, key, scancode, action, mod);
	}
	static void cursor_pos_callback_dispatch(GLFWwindow* window, double xpos, double ypos)
	{
		if (instance)
			instance->cursor_pos_callback(window, xpos, ypos);
	}
	static void scroll_callback_dispatch(GLFWwindow* window, double xoffset, double yoffset)
	{
		if (instance)
			instance->scroll_callback(window, xoffset, yoffset);
	}
};

