#include <iostream>
#include "GaluEngine.h"

int main()
{
	GaluEngine engine;

	engine.ConstructConsole(160, 100, 8, 8);
	engine.Start();

	return 0;
}