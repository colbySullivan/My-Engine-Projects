#pragma once
#include "App.hpp"
#include "GameCommon.hpp"
#include "Game/Player.hpp"
#include "Game/Tile.hpp"
#include "Game/ShopItemCard.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Engine/Core/Vertex.hpp"

class App;
class Entity;
class InputSystem;
class XboxController;
class AudioSystem;
class Player;
class Map;
class BitmapFont;
class Clock;
class UIButton2D;
//class EventArgs;
struct MapDef;

//-----------------------------------------------------------------------------------------------
enum Game_State
{
	GAMESTATE_ATTRACT,
	GAMESTATE_CHARACTER_SELECT,
	GAMESTATE_PLAY,
	GAMESTATE_ITEM,
	NUM_GAMESTATES,
	PURGATORY = -1
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
	std::vector<Vertex>	m_pauseScreenVerts;
	Vertex				m_winLoseScreen[NUM_PLAYER_VERTS];
	SpriteSheet*		m_tilesSpriteSheet;
	SpriteSheet*		m_explosionSpriteSheet;
	SpriteAnimDefinition* m_tilesSpriteSheetAnim;
	Vec2				m_textOffset = Vec2(0.f, 0.f);
	float				m_frameTime = 0.0f;

	// Buttons
	Vec2						m_mouseScreenWindowPosition;
	Vec2						m_mouseWorldWindowPosition;

	// Items
	std::vector<ShopItemCard*> m_shopCards;

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

	Clock*				m_gameClock = nullptr;
	Timer*				m_roundTimer = nullptr;
	float				m_roundLimit;

	float				m_soundDurationTimer = 0.f;
	float				m_shotSoundDurationTimer = 0.f;

	std::string			m_playerDefChosen;

private:
	App* m_app = nullptr;

	void UpdateKeyboardInput( XboxController const& controller );

	void RenderUI() const;
	void RenderUIButtons() const;
	void RenderAttractMode() const;
	void RenderCharacterSelectMode() const;
	void RenderItemMode() const;
	void RenderEntities() const;
	void RenderPauseSreen() const;
	void RenderWinLoseSreen( Texture* texture ) const;

	void UpdateUIButtons();
	void UpdateEntities( float deltaSeconds );
	void UpdateCharacterSelectMode( float deltaSeconds );
	void UpdateItemMode( float deltaSeconds );

	void LoadSounds();
	void InitializePauseVerts();
	void InitializeWinLoseVerts();
	void MovePlayerToNewMap();
	void LoadTextures();
	MapDef CreateMapDef( IntVec2 dimensions, std::string fillTile, std::string edgeTile, float hordeTimer, float bigBoyTimer, float difficultyMultiplier );
	void ConstructMapFromXML();
	SpriteAnimDefinition* m_tilesSpriteSheetAnimPingPong;
	void UpdateMousePosition();
	void InitializeButtonsAndEvents();
	void InitializeDefinitions();
	void InitializeShopCards();
	// Events
	static bool AdvanceGameMode( EventArgs& args );
	static bool CharacterSelect( EventArgs& args );
	static bool BuyItem( EventArgs& args );
};