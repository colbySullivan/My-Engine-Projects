#pragma once
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/IntVec2.hpp"

class Game;
extern Game* g_game;


constexpr float WORLD_SIZE_X = 20.f;
constexpr float WORLD_SIZE_Y = 10.f;

constexpr int MAP_SIZE_X = 50;
constexpr int MAP_SIZE_Y = 25;

constexpr float WORLD_CENTER_X = WORLD_SIZE_X / 2.f;
constexpr float WORLD_CENTER_Y = WORLD_SIZE_Y / 2.f;

constexpr float SCREEN_SIZE_X = 1600.f;
constexpr float SCREEN_SIZE_Y = 800.f;

constexpr float SCREEN_CENTER_X = SCREEN_SIZE_X / 2.f;
constexpr float SCREEN_CENTER_Y = SCREEN_SIZE_Y / 2.f;

constexpr float BULLET_LIFETIME_SECONDS = 2.0f;

constexpr int NUM_PLAYER_VERTS = 6;
constexpr int NUM_TURRET_VERTS = 6;

const IntVec2 STEP_EAST = IntVec2( 1, 0 );
const IntVec2 STEP_WEST = IntVec2( -1, 0 );
const IntVec2 STEP_NORTH = IntVec2( 0, 1 );
const IntVec2 STEP_SOUTH = IntVec2( 0, -1 );
const IntVec2 STEP_NE = IntVec2( 1, 1 ); 
const IntVec2 STEP_NW = IntVec2( -1, 1 );  
const IntVec2 STEP_SE = IntVec2( 1, -1 );  
const IntVec2 STEP_SW = IntVec2( -1, -1 ); 

constexpr float SHOOT_COOLDOWN_TIME = 1.0f;
constexpr float BULLET_SPEED = 8.0f;

constexpr float LEO_MAX_VIS = 10.f;
constexpr float SCORPIO_MAX_VIS = 10.f;

constexpr int PLAYER_HEALTH = 3;
constexpr int PLAYER_LIVES = 3;


//------------------------------------------------------------------------------
void DebugDrawRing(Vec2 const& center, float radius, float thickness, Rgba8 const& color);
void DebugDrawLine(Vec2 const& entity1Pos, Vec2 const& entity2Pos, float thickness, Rgba8 const& color);
