#include "App.h"
using namespace std;

int main()
{
	App* application = new App();

	application->runApp();

	delete application;
	return 0;
}