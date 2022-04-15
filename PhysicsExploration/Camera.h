#ifndef CAMERA_H
#define CAMERA_H

#include "stdafx.h"

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 200.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;
const glm::vec3 TARGET = glm::vec3(0.0f, 0.0f, 0.0f);
const glm::vec3 POSITION = glm::vec3(0.0f, 0.0f, 0.0f);
const glm::vec3 WORLD_UP = glm::vec3(0.0f, 1.0f, 0.0f);

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

class Camera
{
public:
	Camera();
	Camera(glm::vec3 position = POSITION, glm::vec3 world_up = WORLD_UP, glm::vec3 target = TARGET, float yaw = YAW, float pitch = PITCH, float movementSpeed = SPEED, float mouseSensitivity = SENSITIVITY, float zoom = ZOOM);
	glm::mat4 GetViewMatrix();
	void ProcessKeyboard(Camera_Movement dir, float deltaTime);
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
	void ProcessMouseScroll(float yoffset);
	inline float GetZoom() const { return zoom; }
	inline glm::vec3 GetPosition() const { return position; }
	inline glm::vec3 GetFront() const { return direction; }

private:
	void updateCameraVectors();

	glm::vec3 position;
	glm::vec3 target;
	glm::vec3 direction; //direction means what the camera is looking at
	glm::vec3 world_up;
	glm::vec3 right;
	glm::vec3 up; //up defines where the top of the camera is directing towards
	float yaw;
	float pitch;
	float movementSpeed;
	float mouseSensitivity;
	float zoom;
};

#endif 