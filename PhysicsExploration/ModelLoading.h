#pragma once
#include "RenderingPipeline.h"
class ModelLoading : public RenderingPipeline
{
public:
	ModelLoading();
	~ModelLoading();
	void initializeShaders() const override;
	void initializeModels() const override;
	void renderScene(const Shader& shader) const override;
};

