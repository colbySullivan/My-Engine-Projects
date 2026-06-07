#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in VERY few places (and .CPPs only)
#include <math.h>
#include <cassert>
#include <crtdbg.h>
//#include "PlayerShip.hpp"
//
//
//PlayerShip* g_ship1 = nullptr;
//PlayerShip* g_ship2 = nullptr;
//PlayerShip* g_ship3 = nullptr;
//-----------------------------------------------------------------------------------------------
// #SD1ToDo: Eventually, we'll remove most OpenGL references out of Main_Win32.cpp
// Both of the following lines will eventually move to the top of Engine/Renderer/Renderer.cpp
//
#include <gl/gl.h>					// Include basic OpenGL constants and function declarations
#pragma comment( lib, "opengl32" )	// Link in the OpenGL32.lib static library
#include <Engine/Math/Vec2.hpp>
#include <Engine/Core/Vertex.hpp>
#include "App.hpp"
#include <Engine/Core/Engine.hpp>
#include "Engine/Input/InputSystem.hpp"

//-----------------------------------------------------------------------------------------------
// #SD1ToDo: Later we will move this useful macro to a more central place, e.g. Engine/Core/EngineCommon.hpp
//
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


