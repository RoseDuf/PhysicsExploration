#include "ModelLoading.h"
#include "PhysicsSimulationsFactory.h"
using namespace std;

int main()
{	
	int selectedSimulation = 9999;
	int exitCondition = -9;
	PhysicsSimulationsFactory* physicsSimulationFactory = new PhysicsSimulationsFactory;
	 while (exitCondition != selectedSimulation)
	{
		RenderingPipeline* renderingPipeline = physicsSimulationFactory->requestPhysicsSimulation(selectedSimulation);
		selectedSimulation = renderingPipeline->runAppTemplate();
		delete renderingPipeline;
	}
	return 0;
}