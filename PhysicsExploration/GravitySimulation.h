#pragma once
#include "RenderingPipeline.h"
#include "Gravity.h"
class GravitySimulation : public RenderingPipeline
{
private:
	static float u; // current velocity
	static float v; // velocity for next time frame
	static float s0; // currrent position/height of an object or cube
	static float s;  // position for next time frame
	static Gravity* gravity;
	static void updatePhysicsParameters();

protected:
	void initializeShaders() const override;
	void initializeModels() const override;
	void renderScene(const Shader& shader) const override;

public:
	GravitySimulation();
	~GravitySimulation();
	
	
};

