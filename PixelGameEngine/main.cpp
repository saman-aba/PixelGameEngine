#include <iostream>
#include "GaluEngine.h"

int main()
{
	GaluEngine engine;

	if (engine.ConstructConsole(256, 240, 4, 4))
		engine.Start();

	engine.Start();

	return 0;
}