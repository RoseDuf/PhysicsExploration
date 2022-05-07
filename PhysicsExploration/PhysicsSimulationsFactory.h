#pragma once
#include "RenderingPipeline.h"
#include "ModelLoading.h"
class PhysicsSimulationsFactory
{
public:
	PhysicsSimulationsFactory();
	RenderingPipeline* requestPhysicsSimulation(int selectedItem);
};

