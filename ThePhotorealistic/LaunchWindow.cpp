#include "stdafx.h"
#include "ThePhotorealistic.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	ThePhotorealistic theApp(hInstance);
	if (!theApp.Init())
		return 0;

	return theApp.Run();
}
