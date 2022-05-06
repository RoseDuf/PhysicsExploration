#include "ModelLoading.h"
Shader* RenderingPipeline::_shader;
Shader* RenderingPipeline::_lightShader;
Shader* RenderingPipeline::_depthShader;
Model* RenderingPipeline::_plane;
Model* RenderingPipeline::_object;

ModelLoading::ModelLoading()
{

}

ModelLoading::~ModelLoading()
{

}

void ModelLoading::initializeShaders() const
{
	// 2. use our shader program when we want to render an object
	// Build and compile our shader program
	// Vertex shader.
	_shader = new Shader("../PhysicsExploration/vertex.shader", "../PhysicsExploration/fragment.shader");
	_lightShader = new Shader("../PhysicsExploration/light_vertex.shader", "../PhysicsExploration/light_fragment.shader");
	_depthShader = new Shader("../PhysicsExploration/shadow_vertex.shader", "../PhysicsExploration/shadow_fragment.shader");
}

void ModelLoading::initializeModels() const
{
	//Model object("../External Resources/Models/cat.obj");
	_plane = new Model("../External Resources/Models/plane.obj");
	_object = new Model("../External Resources/Models/backpack.obj");
}

void ModelLoading::renderScene(const Shader& shader) const
{
	shader.setVector("spotLight.position", glm::vec4(_camera->GetPosition(), 1.0f));
	shader.setVector("spotLight.direction", _camera->GetFront());

	shader.setVector("view_position", _camera->GetPosition());

	glm::mat4 proj_matrix = glm::mat4(1.0f);
	proj_matrix = glm::perspective(glm::radians(_camera->GetZoom()), (float)WIDTH / (float)HEIGHT, 0.1f, 100.f);
	shader.setMatrix4("proj_matrix", proj_matrix);

	glm::mat4 view_matrix = _camera->GetViewMatrix();
	shader.setMatrix4("view_matrix", view_matrix);

	// render the loaded model
	glm::mat4 model_matrix = glm::mat4(1.0f);
	model_matrix = glm::translate(model_matrix, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
	model_matrix = glm::scale(model_matrix, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
	shader.setMatrix4("model_matrix", model_matrix);

	shader.setBool("hasDirLight", _hasDirLight);
	shader.setBool("hasPointLight", _hasPointLight);
	shader.setBool("hasSpotLight", _hasSpotLight);
}
