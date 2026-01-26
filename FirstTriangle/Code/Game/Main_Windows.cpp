#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in VERY few places (and .CPPs only)
#include <math.h>
#include <cassert>
#include <crtdbg.h>
#include <gl/gl.h>					// Include basic OpenGL constants and function declarations
#pragma comment( lib, "opengl32" )	// Link in the OpenGL32.lib static library
#include "App.hpp"
#include <Engine/Core/Engine.hpp>

#define UNUSED(x) (void)(x);

App* g_theApp = nullptr;

//------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE applicationInstanceHandle, HINSTANCE, LPSTR commandLineString, int)
{
	UNUSED(applicationInstanceHandle);
	UNUSED(commandLineString);

	g_theApp = new App();

	// Program main loop; keep running frames until it's time to quit
	while (!g_theApp->IsQuitting())
	{
		g_theApp->RunFrame();
	}

	delete g_theApp;
	g_theApp = nullptr;

	return 0;
}


