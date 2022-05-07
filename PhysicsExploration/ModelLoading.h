#pragma once
#include "RenderingPipeline.h"
class ModelLoading : public RenderingPipeline
{
protected:
	
	static ModelLoading* _uniqueInstance;
	void initializeShaders() const override;
	void initializeModels() const override;
	void renderScene(const Shader& shader) const override;
public:
	ModelLoading();
	~ModelLoading();
	static ModelLoading* instance();
};

