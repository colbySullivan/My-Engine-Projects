#pragma once
#include "App.hpp"
#include "GameCommon.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Game/Player.hpp"
#include "Game/Tile.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"


class App;
class Entity;
class InputSystem;
class RandomNumberGenerator;
class XboxController;
class AudioSystem;
class Player;
class Map;
class BitmapFont;
struct MapDef;

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

enum MapRenderMode
{
	REGULAR_MAP_MODE,
	HEAT_MAP_MODE,
	SOLID_MAP_MODE,
	NUM_MAP_MODES
};

//-----------------------------------------------------------------------------------------------
class Game
{
public:
	Game();
	~Game();

	Map* m_currentMap = nullptr;
	Map* m_nextMap = nullptr;
	int m_currentMapNumber = 0;
	BitmapFont* g_testFont = nullptr;

	void Startup();
	void Update( float deltaSeconds );
	void Render() const;
	void Shutdown();

	void HandleSound( SoundPlaybackID soundID, SoundPriority priority = PRIORITY_LOW, float soundDuration = 0.2f);

	// Game State
	bool				m_isQuitting = false;
	bool				m_isPaused = false;
	bool				m_isSlowMo = false;
	bool				m_isFastMo = false;
	bool				m_pauseAfterNextUpdate = false;
	bool				g_drawDebug = false;
	bool				m_playingEndSound = false;
	bool				m_hasWon = false;
	bool				m_endGame = false;

	// Utility
	int					m_roundNumber = 1;
	float				m_alphaTimer = 3;
	float				m_roundEndTimer = 3;
	float				m_camShakeAmount;
	float				m_shipAnimationTimer = 0.0f;
	Game_State			m_currentGameState = GAMESTATE_ATTRACT;
	Game_State			m_nextGameState  = GAMESTATE_ATTRACT;
	MapRenderMode		m_mapRenderMode = REGULAR_MAP_MODE;
	float 				m_endGameDelayTimer = 3.0f;	
	float				m_bestRoundTime = 0.f;
	float				m_spawnBuffer;
	std::vector<Map*>	m_maps = {};
	Vertex				m_pauseScreenVerts[NUM_PLAYER_VERTS];
	Vertex				m_winLoseScreen[NUM_PLAYER_VERTS];
	SpriteSheet*		m_tilesSpriteSheet;
	SpriteSheet*		m_explosionSpriteSheet;
	SpriteAnimDefinition* m_tilesSpriteSheetAnim;
	Vec2				m_textOffset = Vec2(0.f, 0.f);
	float				m_frameTime = 0.0f;




	// Entities
	Texture* m_playerBodyTexture;
	Texture* m_playerTurretTexture;
	Texture* m_scorpioBodyTexture;
	Texture* m_scorpioTurretTexture;
	Texture* m_leoBodyTexture;
	Texture* m_ariesBodyTexture;
	Texture* m_goodBulletTexture;
	Texture* m_badBulletTexture;
	Texture* m_endWinScreen;
	Texture* m_startScreen;
	Texture* m_endLoseScreen;


	// Camera
	Camera* m_worldCamera = nullptr;
	Camera* m_screenCamera = nullptr;

	// Audio
	SoundPlaybackID		m_endPlaybackID = MISSING_SOUND_ID;
	SoundPlaybackID		m_lobbyPlaybackID = MISSING_SOUND_ID;
	SoundPlaybackID		m_gameMusicPlaybackID = MISSING_SOUND_ID;
	SoundPlaybackID		m_currentSound = MISSING_SOUND_ID;
	SoundPlaybackID		m_roundChangeSound = MISSING_SOUND_ID;
	SoundPlaybackID		m_shootSound = MISSING_SOUND_ID;
	SoundPlaybackID		m_enemyDied = MISSING_SOUND_ID;
	SoundPlaybackID		m_bulletBounce = MISSING_SOUND_ID;
	SoundPlaybackID		m_enemyHit = MISSING_SOUND_ID;
	SoundPlaybackID		m_playerHit = MISSING_SOUND_ID;
	SoundPlaybackID		m_victorySound = MISSING_SOUND_ID;
	SoundPlaybackID		m_lossSound = MISSING_SOUND_ID;


	SoundPriority		m_currentSoundPriority = PRIORITY_LOW;
	float				m_soundDurationTimer = 0.f;
	float				m_shotSoundDurationTimer = 0.f;


private:
	App* m_app = nullptr;

	void UpdateKeyboardInput( XboxController const& controller );

	void RenderUI() const;
	void RenderText( const char text[] , Vec2 pos, float height, Rgba8 color ) const;
	void RenderAttractMode() const;
	void RenderEntities() const;
	void RenderPauseSreen() const;
	void RenderWinLoseSreen( Texture* texture ) const;

	void UpdateCameras( float deltaSeconds );
	void UpdateAttractMode( float deltaSeconds );
	void UpdateEntities( float deltaSeconds );

	void LoadSounds();
	void InitializePauseVerts();
	void InitializeWinLoseVerts();
	void MovePlayerToNewMap();
	void PlayerPortalEndConditionCheck();
	void LoadTextures();
	MapDef CreateMapDef( IntVec2 dimensions, std::string fillTile, std::string edgeTile, std::string sprinkleTile1, std::string sprinkleTile2, std::string barrierTile );
	void ConstructMapFromXML();
};