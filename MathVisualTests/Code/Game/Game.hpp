#pragma once
#include "App.hpp"
#include "GameCommon.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/Vertex.hpp"

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
	~Game();

	void Startup();
	void Update( float deltaSeconds );
	void Render() const;
	void Shutdown();

	// Game State
	bool				m_isQuitting = false;
	bool				m_isPaused = false;
	bool				m_isSlowMo = false;
	bool				m_pauseAfterNextUpdate = false;
	bool				g_drawDebug = false;

private:
	void UpdateKeyboardInput( XboxController const& controller );

	void RenderText( const char text[] , Vec2 pos, float height, Rgba8 color ) const;
	void UpdateCameras( float deltaSeconds );

	App*			m_app = nullptr;
	Camera*			m_worldCamera = nullptr;
	Camera*			m_screenCamera = nullptr;
	std::vector<Vertex> m_lineVerts;
};