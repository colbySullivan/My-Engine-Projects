#pragma once
#include "App.hpp"
#include "GameCommon.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Core/Clock.hpp"
#include "Game/ChessPiece.hpp"
#include "Game/ChessBoard.hpp"

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
class Timer;
class ChessPiece;
class ChessBoard;

//-----------------------------------------------------------------------------------------------
enum Game_State
{
	GAMESTATE_ATTRACT,
	GAMESTATE_PLAY,
	GAMESTATE_INVALID,
	NUM_GAMESTATES
};

struct CameraMode 
{
	std::string modeName;
	bool freeCamera = false;
	Vec3 position;
	EulerAngles orientation;
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
	void RemoveChessPiece( ChessPiece* piece );
	void RestartGame();
	void ChangePlayerCamera( int player );

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
	Timer*				m_cubeBlinkTimer;

	// Textures
	BitmapFont* g_testFont = nullptr;

	// Clock
	Clock* m_gameClock = nullptr;
	// Player and props
	Player* m_player = nullptr;
	std::vector<Prop*> m_props;
	std::vector<ChessPiece*> m_chessPieces;
	ChessBoard* m_chessBoard;
	Camera* m_screenCamera = nullptr;

	// CameraModes
	std::vector<CameraMode*> m_cameraModes;
	CameraMode* m_currentCameraMode = nullptr;
	int m_currentCameraNum = 0;
	std::string m_modeName = "Mode 0: Normal rendering";
	bool        m_showImGui = false;

private:
	void UpdateImGui();
	void UpdateKeyboardInput( XboxController const& controller );
	void DebugInput();

	void RenderUI() const;
	void RenderText( const char text[] , Vec2 pos, float height, Rgba8 color ) const;

	void UpdateAttractMode( float deltaSeconds );
	void RenderAttractMode( ) const;

	void UpdateBlackHole();

	void CreateProps();
	void CreateDebugRenderObjects();

	void CreateChessPieces();

	App* m_app = nullptr;
	Texture*		m_testTexture = nullptr;
	// Black hole
	Vertex			m_blackHoleVerts[NUM_BLACK_HOLE_VERTS];
	Vertex			m_gameBlackHole[NUM_BLACK_HOLE_VERTS];
	int				m_roundBlackHoleAmount = 2;
	void CreateCameraModes();
	static bool Command_BeginGame( [[maybe_unused]] EventArgs& args );

	void CreateImGuiStyle();

};