#pragma once
#include "App.hpp"
#include "GameCommon.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Core/Clock.hpp"

class App;
class Entity;
class InputSystem;
class RandomNumberGenerator;
class XboxController;
class AudioSystem;
class BitmapFont;
class Clock;
class Player;
class Prop;

//-----------------------------------------------------------------------------------------------
enum Game_State
{
	GAMESTATE_ATTRACT,
	GAMESTATE_PLAY,
	GAMESTATE_INVALID,
	NUM_GAMESTATES
};

//-----------------------------------------------------------------------------------------------
class Game
{

public:
	Game();
	~Game();

	void Startup();
	void Update();
	void Render() const;
	void Shutdown();

	// Game State
	bool				m_isQuitting = false;
	bool				m_isPaused = false;
	bool				m_isSlowMo = false;
	bool				m_pauseAfterNextUpdate = false;
	bool				g_drawDebug = false;
	bool				m_playingEndSound = false;

	// Utility
	int					m_roundNumber = 1;
	float				m_alphaTimer = 3;
	float				m_roundEndTimer = 3;
	float				m_camShakeAmount;
	float				m_shipAnimationTimer = 0.0f;
	Game_State			m_currentGameState = GAMESTATE_ATTRACT;
	Game_State			m_nextGameState  = GAMESTATE_ATTRACT;
	float 				m_roundTime;	
	float				m_bestRoundTime = 0.f;
	float				m_spawnBuffer;

	// Textures
	BitmapFont* g_testFont = nullptr;

	// Clock
	Clock* m_gameClock = nullptr;

	// Player and props
	Player* m_player = nullptr;
	Prop*	m_box = nullptr;


private:
	void UpdateKeyboardInput( XboxController const& controller );

	void RenderUI() const;
	void RenderText( const char text[] , Vec2 pos, float height, Rgba8 color ) const;
	void UpdateCameras( float deltaSeconds );

	void UpdateAttractMode( float deltaSeconds );
	void RenderAttractMode( ) const;

	void UpdateBlackHole();

	App*			m_app = nullptr;
	Camera*			m_worldCamera = nullptr;
	Camera*			m_screenCamera = nullptr;
	// Black hole
	Vertex			m_blackHoleVerts[NUM_BLACK_HOLE_VERTS];
	Vertex			m_gameBlackHole[NUM_BLACK_HOLE_VERTS];
	int				m_roundBlackHoleAmount = 2;
};