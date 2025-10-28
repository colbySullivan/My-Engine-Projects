#pragma once
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/IntVec2.hpp"

class Game;

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
constexpr float BULLET_SPEED = 50.f;
constexpr float BULLET_PHYSICS_RADIUS = 1.5f;
constexpr float BULLET_COSMETIC_RADIUS = 3.0f;
constexpr float BULLET_BASE_COOLDOWN = 0.1f;

constexpr int NUM_BLACK_HOLE_VERTS = 192;
constexpr int NUM_BLACK_HOLE_TRIS = 64;
constexpr int NUM_BLACK_HOLE_SIDES = 64;

constexpr int	NUM_VERTS = 6;

extern Game* g_game;

constexpr float PLAYER_PHYSICS_RADIUS = 0.5f;
constexpr float PLAYER_COSMETIC_RADIUS = 1.f;
constexpr float PLAYER_TURN_SPEED = 180.f;
constexpr float PLAYER_SPEED = 1.f;

const Rgba8 GRASS_COLOR = Rgba8(50, 100, 50, 255);
const Rgba8 STONE_COLOR = Rgba8(180, 180, 180, 255);

const IntVec2 STEP_EAST = IntVec2( 1, 0 );
const IntVec2 STEP_WEST = IntVec2( -1, 0 );
const IntVec2 STEP_NORTH = IntVec2( 0, 1 );
const IntVec2 STEP_SOUTH = IntVec2( 0, -1 );
const IntVec2 STEP_NE = IntVec2( 1, 1 ); 
const IntVec2 STEP_NW = IntVec2( -1, 1 );  
const IntVec2 STEP_SE = IntVec2( 1, -1 );  
const IntVec2 STEP_SW = IntVec2( -1, -1 ); 


//------------------------------------------------------------------------------
void DebugDrawRing(Vec2 const& center, float radius, float thickness, Rgba8 const& color);
void DebugDrawLine(Vec2 const& entity1Pos, Vec2 const& entity2Pos, float thickness, Rgba8 const& color);
