#include "PhysicsSimulationsFactory.h"

PhysicsSimulationsFactory::PhysicsSimulationsFactory()
{
}

RenderingPipeline* PhysicsSimulationsFactory::requestPhysicsSimulation(int selectedItem)
{
    switch (selectedItem)
    {
    case 1:
        return new ModelLoading();
        break;
    default:
        return new ModelLoading();
    }
}
