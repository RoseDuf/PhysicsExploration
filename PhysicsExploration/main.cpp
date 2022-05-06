#include "ModelLoading.h"
using namespace std;

int main()
{	
	RenderingPipeline* renderingPipeline = new ModelLoading;
	renderingPipeline->runAppTemplate();
	delete renderingPipeline; 
	return 0;
}