#pragma once
#include "App.hpp"
#include "GameCommon.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Renderer/OBJLoader.hpp"
#include "Game/Map.hpp"
#include "PlayerController.hpp"

class App;
class Actor;
class InputSystem;
class RandomNumberGenerator;
class XboxController;
class AudioSystem;
class BitmapFont;
class Clock;
class Player;
class Prop;
class Timer;
class Map;
struct MapSpawnInfo;

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
	float				m_shipAnimationTimer = 0.0f;
	Game_State			m_currentGameState = GAMESTATE_ATTRACT;
	Game_State			m_nextGameState  = GAMESTATE_ATTRACT;
	float 				m_roundTime = 0.f;
	float				m_bestRoundTime = 0.f;

	// Textures
	BitmapFont* g_testFont = nullptr;

	// Clock
	Clock* m_gameClock = nullptr;

	// Player and props
	Player* m_player = nullptr;
	std::vector<Prop*> m_props;
	bool m_controlPlayerMode = false;

	// Maps
	std::vector<Map*> m_maps = {};
	std::vector<MapSpawnInfo*> m_spawnInfo = {};
	Map* m_currentMap = nullptr;
	Map* m_nextMap = nullptr;
	int m_currentMapNumber = 0;
	Texture* m_teemoTexture = nullptr;
	OBJModel* m_teemoModel = nullptr;
	PlayerController* m_playerController = nullptr;

private:
	void UpdateKeyboardInput( XboxController const& controller );
	void DebugInput();

	void RenderUI() const;
	void RenderText( const char text[] , Vec2 pos, float height, Rgba8 color ) const;

	void UpdateAttractMode( float deltaSeconds );
	void RenderAttractMode( ) const;

	void UpdateBlackHole();

	void CreateProps();

	App*			m_app = nullptr;
	Camera*			m_screenCamera = nullptr;
	Texture*		m_testTexture = nullptr;
	// Black hole
	Vertex			m_blackHoleVerts[NUM_BLACK_HOLE_VERTS];
	Vertex			m_gameBlackHole[NUM_BLACK_HOLE_VERTS];
	int				m_roundBlackHoleAmount = 2;
	void CreateMapsFromDef();

	Vec3 GetRaycastOrigin() const;
	Vec3 GetRaycastDirection() const;
	Mat44 GetRaycastTransform() const;

};