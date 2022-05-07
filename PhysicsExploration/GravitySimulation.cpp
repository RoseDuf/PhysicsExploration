#include "GravitySimulation.h"

float GravitySimulation::u;
float GravitySimulation::v;
float GravitySimulation::s0;
float GravitySimulation::s;
Gravity* GravitySimulation::gravity;

void GravitySimulation::initializeShaders() const
{
	_shader = new Shader("../PhysicsExploration/planet_vertex.shader", "../PhysicsExploration/planet_fragment.shader");
	_lightShader = new Shader("../PhysicsExploration/light_vertex.shader", "../PhysicsExploration/light_fragment.shader");
	_depthShader = new Shader("../PhysicsExploration/shadow_vertex.shader", "../PhysicsExploration/shadow_fragment.shader");
}

GravitySimulation::GravitySimulation()
{
	u = 0.0f;
	v = 0.0f;
	s0 = 0.0f;
	s = 0.0f;
	gravity = new Gravity();
}

void GravitySimulation::initializeModels() const
{
	_plane = new Model("../External Resources/Models/plane.obj");
	_object = new Model("../External Resources/Models/planet_obj.obj");
}

GravitySimulation::~GravitySimulation()
{
	delete gravity;
}

void GravitySimulation::renderScene(const Shader& shader) const
{
	shader.setVector("spotLight.position", glm::vec4(_camera->GetPosition(), 1.0f));
	shader.setVector("spotLight.direction", _camera->GetFront());
	shader.setVector("view_position", _camera->GetPosition());
	updatePhysicsParameters();
	glm::mat4 proj_matrix = glm::mat4(1.0f);
	proj_matrix = glm::perspective(glm::radians(_camera->GetZoom()), (float)WIDTH / (float)HEIGHT, 0.1f, 100.f);
	shader.setMatrix4("proj_matrix", proj_matrix);

	glm::mat4 view_matrix = _camera->GetViewMatrix();
	shader.setMatrix4("view_matrix", view_matrix);

	// render the loaded model
	glm::mat4 model_matrix = glm::mat4(1.0f);
	model_matrix = glm::translate(model_matrix, glm::vec3(0.0f, -1.0f * s, 0.0f)); // translate it down so it's at the center of the scene
	model_matrix = glm::scale(model_matrix, glm::vec3(0.01f, 0.01f, 0.01f));	// it's a bit too big for our scene, so scale it down
	shader.setMatrix4("model_matrix", model_matrix);

	shader.setBool("hasDirLight", _hasDirLight);
	shader.setBool("hasPointLight", _hasPointLight);
	shader.setBool("hasSpotLight", _hasSpotLight);
}

void GravitySimulation::updatePhysicsParameters()
{
	v = gravity->calculateVelocity(u, _deltaTime);
	s = gravity->calculatePositionFromVelocity(s0, u, _deltaTime);
	u = v;
	s0 = s;
}
