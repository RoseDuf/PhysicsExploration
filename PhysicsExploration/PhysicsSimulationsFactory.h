#pragma once
#include "RenderingPipeline.h"
#include "ModelLoading.h"
#include "GravitySimulation.h"
class PhysicsSimulationsFactory
{
public:
	PhysicsSimulationsFactory();
	RenderingPipeline* requestPhysicsSimulation(int selectedItem);
};

