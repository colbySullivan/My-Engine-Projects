#pragma once
#include <Engine/Math/MathUtils.hpp>
#include <Engine/Core/Rgba8.hpp>

constexpr int	NUM_STARTING_ASTEROIDS = 3;
constexpr int	MAX_ASTEROIDS = 200;
constexpr int	MAX_BULLETS = 10000;
constexpr int	MAX_BEETLES = 200;
constexpr int	MAX_WASP = 200;
constexpr int	MAX_DEBRIS = 5000;

constexpr float WORLD_SIZE_X = 1600.f;
constexpr float WORLD_SIZE_Y = 800.f;

constexpr float WORLD_CENTER_X = WORLD_SIZE_X / 2.f;
constexpr float WORLD_CENTER_Y = WORLD_SIZE_Y / 2.f;

constexpr float SCREEN_SIZE_X = 1600.f;
constexpr float SCREEN_SIZE_Y = 800.f;

constexpr float SCREEN_CENTER_X = SCREEN_SIZE_X / 2.f;
constexpr float SCREEN_CENTER_Y = SCREEN_SIZE_Y / 2.f;

constexpr float ASTEROID_SPEED = 10.f;
constexpr float BEETLE_SPEED = 15.f;

constexpr float WASP_ACCELERATION = 5.f;
constexpr float WASP_MAX_SPEED = 20.f;

constexpr float ASTEROID_PHYSICS_RADIUS = 1.6f;
constexpr float ASTEROID_COSMETIC_RADIUS = 2.0f;

constexpr float BEETLE_PHYSICS_RADIUS = 1.2f;
constexpr float BEETLE_COSMETIC_RADIUS = 2.0f;

constexpr float WASP_PHYSICS_RADIUS = 1.2f;
constexpr float WASP_COSMETIC_RADIUS = 2.0f;

constexpr float DEBRIS_PHYSICS_RADIUS = 0.6f;
constexpr float DEBRIS_COSMETIC_RADIUS = 3.0f;
constexpr float DEBRIS_LIFETIME_SECONDS = 2.0f;

constexpr float BULLET_LIFETIME_SECONDS = 2.0f;
constexpr float BULLET_SPEED = 50.f;
constexpr float BULLET_PHYSICS_RADIUS = 1.5f;
constexpr float BULLET_COSMETIC_RADIUS = 3.0f;
constexpr float BULLET_BASE_COOLDOWN = 0.1f;

constexpr float PLAYER_SHIP_ACCELERATION = 30.f;
constexpr float PLAYER_SHIP_TURN_SPEED = 300.f;
constexpr float PLAYER_SHIP_PHYSICS_RADIUS = 1.75f;
constexpr float PLAYER_SHIP_COSMETIC_RADIUS = 2.25f;
constexpr int	NUM_VERTS = 6;
constexpr float SHIP_INVINCIBILITY_TIME = 2.0f;
constexpr float SHIP_ANIMATION_DURATION = 5.0f;

constexpr float	SHAKE_TRAUMA_AMOUNT = 1;

constexpr int NUM_BLACK_HOLE_VERTS = 192;
constexpr int NUM_BLACK_HOLE_TRIS = 64;
constexpr int NUM_BLACK_HOLE_SIDES = 64;

constexpr int NEW_MAX_STARS = 500;
constexpr int STAR_PER_VERTS = 3;
constexpr int NUM_STAR_VERTS = NEW_MAX_STARS * STAR_PER_VERTS;

constexpr int MAX_INTERACTABLES = 200;
constexpr float INTERACTABLES_PHYSICS_RADIUS = 6.0f;
constexpr float INTERACTABLES_COSMETIC_RADIUS = 8.0f;
constexpr float POWERUP_DELAY = 1.0f;

constexpr float	ROUND_BUFFER = 12.f;

constexpr float MOUSE_SENSITIVITY = 0.075f;
constexpr int ROLL_RATE = 90;
constexpr int PITCH_RATE = 120;
constexpr int YAW_RATE = 120;
constexpr int MOVE_SPEED = 1;

const IntVec2 STEP_EAST = IntVec2( 1, 0 );
const IntVec2 STEP_WEST = IntVec2( -1, 0 );
const IntVec2 STEP_NORTH = IntVec2( 0, 1 );
const IntVec2 STEP_SOUTH = IntVec2( 0, -1 );
const IntVec2 STEP_NE = IntVec2( 1, 1 );
const IntVec2 STEP_NW = IntVec2( -1, 1 );
const IntVec2 STEP_SE = IntVec2( 1, -1 );
const IntVec2 STEP_SW = IntVec2( -1, -1 );


const float PACK_A_PUNCH_COST = 750.f;

//------------------------------------------------------------------------------
enum PowerUp
{
	BulletSpeed1,
	BulletSpeed2,
	BulletSpeed3,
	BulletCount1,
	BulletCount2,
	BulletCount3,
	Num_PowerUps
};

//------------------------------------------------------------------------------
void DebugDrawRing(Vec2 const& center, float radius, float thickness, Rgba8 const& color);
void DebugDrawLine(Vec2 const& entity1Pos, Vec2 const& entity2Pos, float thickness, Rgba8 const& color);
