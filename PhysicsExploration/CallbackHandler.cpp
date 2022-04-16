#include "CallbackHandler.h"

CallbackHandler* CallbackHandler::instance;

void CallbackHandler::framebuffer_size_callback(GLFWwindow* window, int width, int height) {}
void CallbackHandler::key_callback(GLFWwindow* window, int key, int scancode, int action, int mod) {}
void CallbackHandler::cursor_pos_callback(GLFWwindow* window, double xpos, double ypos) {}
void CallbackHandler::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {}