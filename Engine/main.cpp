#include "globaldef.h"
#include <windows.h>
#include "Engine.h"

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nShowCmd)
{
	Engine * myEngine = new Engine(hInstance);
	if (!myEngine->initialize_engine())
	{
		delete myEngine;
		return 0;
	}

	delete myEngine;
	return 0;
}
