#include "Camera.h"

#pragma region constructors

Camera::Camera()
{
	position = POSITION;
	world_up = glm::vec3(0, 1, 0);
	yaw = -90.0f;
	pitch = 0.0f;

	direction = glm::normalize(TARGET - position);
	right = glm::normalize(glm::cross(world_up, direction));
	up = glm::cross(direction, right);

	movementSpeed = SPEED;
	mouseSensitivity = SENSITIVITY;
	zoom = ZOOM;
}

Camera::Camera(glm::vec3 position, glm::vec3 world_up, glm::vec3 target, float yaw, float pitch, float movementSpeed, float mouseSensitivity, float zoom)
{
	this->position = position;
	this->world_up = world_up;
	this->target = target;
	this->yaw = yaw;
	this->pitch = pitch;
	this->movementSpeed = movementSpeed;
	this->mouseSensitivity = mouseSensitivity;
	this->zoom = zoom;

	direction = glm::normalize(this->target - this->position);
	right = glm::normalize(glm::cross(this->world_up, direction));
	up = glm::cross(direction, right);
}

#pragma endregion

glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(position, position + direction, up);
}

void Camera::ProcessKeyboard(Camera_Movement dir, float deltaTime)
{
	const float cameraSpeed = movementSpeed * deltaTime; // adjust accordingly

	//WASD buttons to move the camera
	if (dir == FORWARD) //W moves forward
		position += cameraSpeed * direction;

	if (dir == BACKWARD) //S moves backwards
		position -= cameraSpeed * direction;

	if (dir == RIGHT) //D moves right
		position += cameraSpeed * right;

	if (dir == LEFT) //A moves left
		position -= cameraSpeed * right;

	if (dir == UP) //A moves left
		position += cameraSpeed * glm::cross(right, direction);

	if (dir == DOWN) //A moves left
		position += cameraSpeed * glm::cross(-right, direction);

	////Left and Right key rotate camera
	//if (key == GLFW_KEY_RIGHT) //left arrow rotates the camera left about the up vector
	//	direction = glm::mat3(glm::rotate(glm::radians(0.8f), -up)) * direction;

	//if (key == GLFW_KEY_LEFT) //right arrow rotates the camera right about the up vector
	//	direction = glm::mat3(glm::rotate(glm::radians(0.8f), up)) * direction;

	//glm::vec3 rotate_around_x = glm::cross(up, direction); //cross product
	//if (key == GLFW_KEY_UP) { //up arrow rotates the camera upwards about the right vector
	//	glm::mat4 rotation_matrix(1);
	//	rotation_matrix = glm::rotate(rotation_matrix, -(8 / 180.0f), rotate_around_x);
	//	direction = glm::mat3(rotation_matrix) * direction;
	//	up = glm::mat3(rotation_matrix) * up;
	//}

	//if (key == GLFW_KEY_DOWN) { //down arrow rotates the camera downwards about the right vector
	//							//cam_dir = glm::mat3(glm::rotate(glm::radians(0.8f), rotate_around_x)) * cam_dir;
	//	glm::mat4 rotation_matrix(1);
	//	rotation_matrix = glm::rotate(rotation_matrix, (8 / 180.0f), rotate_around_x);
	//	direction = glm::mat3(rotation_matrix) * direction;
	//	up = glm::mat3(rotation_matrix) * up;
	//}

}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
	xoffset *= mouseSensitivity;
	yoffset *= mouseSensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (constrainPitch)
	{
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;
	}

	// update Front, Right and Up Vectors using the updated Euler angles
	updateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset)
{
	zoom -= (float)yoffset;
	if (zoom < 1.0f)
		zoom = 1.0f;
	if (zoom > 45.0f)
		zoom = 45.0f;
}

void Camera::updateCameraVectors()
{
	// calculate the new Front vector
	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction = glm::normalize(front);
	// also re-calculate the Right and Up vector
	right = glm::normalize(glm::cross(direction, world_up));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	up = glm::normalize(glm::cross(right, direction));
}