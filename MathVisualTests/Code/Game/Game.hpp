#pragma once
#include "Game/GameCommon.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Renderer/Camera.hpp"

class App;
class Entity;
class InputSystem;
class RandomNumberGenerator;
class XboxController;
class AudioSystem;

//-----------------------------------------------------------------------------------------------
class Game
{

public:
	Game(App* m_app);
	virtual ~Game();

	virtual void Startup();
	virtual void Update( float deltaSeconds ) = 0;
	virtual void Render() const = 0;
	void Shutdown();

	// Game State
	bool				m_isQuitting = false;
	bool				m_isPaused = false;
	bool				m_isSlowMo = false;
	bool				m_pauseAfterNextUpdate = false;
	bool				g_drawDebug = false;
	Camera*				m_worldCamera = nullptr;
	//Camera*				m_screenCamera = nullptr;

	void UpdateCameras( float deltaSeconds );
	void UpdateKeyboardInput();
	void RenderGameText( GameType g_gameMode ) const;
private:
	
	void RenderText( const char text[] , Vec2 pos, float height, Rgba8 color ) const;
	App* m_app = nullptr;
	std::vector<Vertex> m_lineVerts;
};