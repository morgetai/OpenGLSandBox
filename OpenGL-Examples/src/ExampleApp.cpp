//#include <GLCore.h>
#include <GLCore/Core/Application.h>
#include "ExampleLayer.h"

using namespace GLCore;

int main()
{
	Application app("ExampleApp");
	app.PushLayer(new ExampleLayer());
	app.Run();
}