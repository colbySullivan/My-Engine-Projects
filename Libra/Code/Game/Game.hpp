#pragma once
#include "App.hpp"
#include "GameCommon.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Game/Player.hpp"

class App;
class Entity;
class InputSystem;
class RandomNumberGenerator;
class XboxController;
class AudioSystem;
class Player;
class Map;

//-----------------------------------------------------------------------------------------------
enum Game_State
{
	GAMESTATE_ATTRACT,
	GAMESTATE_PLAY,
	GAMESTATE_INVALID,
	NUM_GAMESTATES
};

//------------------------------------------------------------------------------
enum SoundPriority
{
	PRIORITY_LOW,			// Shoot
	PRIORITY_MEDIUM,		// Hit/hurt / Alien death
	PRIORITY_HIGH,			// Respawn / Final death
	PRIORITY_MUSIC,			// Background music game and lobby
	NUM_SOUND_PRIORITIES
};

//-----------------------------------------------------------------------------------------------
class Game
{
	Map* m_currentMap = nullptr;

public:
	Game();
	~Game();

	void Startup();
	void Update( float deltaSeconds );
	void Render() const;
	void Shutdown();

	void HandleSound( SoundPlaybackID soundID, SoundPriority priority = PRIORITY_LOW, float soundDuration = 0.2f);

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

	// Entities
	Player* m_player = nullptr;

	// Camera
	Camera* m_worldCamera = nullptr;
	Camera* m_screenCamera = nullptr;

	// Audio
	SoundPlaybackID		m_endPlaybackID = MISSING_SOUND_ID;
	SoundPlaybackID		m_lobbyPlaybackID = MISSING_SOUND_ID;
	SoundPlaybackID		m_gameMusicPlaybackID = MISSING_SOUND_ID;
	SoundPlaybackID		m_currentSound = MISSING_SOUND_ID;
	SoundPlaybackID		m_roundChangeSound = MISSING_SOUND_ID;
	SoundPriority		m_currentSoundPriority = PRIORITY_LOW;
	float				m_soundDurationTimer = 0.f;
	SoundPlaybackID		m_shootSound = MISSING_SOUND_ID;
	float				m_shotSoundDurationTimer = 0.f;


private:
	void UpdateKeyboardInput( XboxController const& controller );

	void RenderUI() const;
	void RenderText( const char text[] , Vec2 pos, float height, Rgba8 color ) const;
	void RenderAttractMode() const;
	void RenderEntities() const;

	void UpdateCameras( float deltaSeconds );
	void UpdateAttractMode( float deltaSeconds );
	void UpdateEntities( float deltaSeconds );

	void LoadSounds();
	void UpdateBlackHole();

	App*			m_app = nullptr;
	// Black hole
	Vertex			m_blackHoleVerts[NUM_BLACK_HOLE_VERTS];
	Vertex			m_gameBlackHole[NUM_BLACK_HOLE_VERTS];
	int				m_roundBlackHoleAmount = 2;

	void RenderPauseSreen() const;
	void InitializePauseVerts();
	Vertex			m_pauseScreenVerts[NUM_PLAYER_VERTS];
};