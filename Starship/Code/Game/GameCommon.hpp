#pragma once
#include <Engine/Math/MathUtils.hpp>
#include <Engine/Core/Rgba8.hpp>

constexpr int	NUM_STARTING_ASTEROIDS = 6;
constexpr int	MAX_ASTEROIDS = 32;
constexpr int	MAX_BULLETS = 100;
constexpr int	MAX_BEETLES = 20;
constexpr int	MAX_WASP = 20;
constexpr int	MAX_DEBRIS = 500;

constexpr float WORLD_SIZE_X = 200.f;
constexpr float WORLD_SIZE_Y = 100.f;

constexpr float WORLD_CENTER_X = WORLD_SIZE_X / 2.f;
constexpr float WORLD_CENTER_Y = WORLD_SIZE_Y / 2.f;

constexpr float SCREEN_SIZE_X = 1600.f;
constexpr float SCREEN_SIZE_Y = 800.f;

constexpr float SCREEN_CENTER_X = SCREEN_SIZE_X / 2.f;
constexpr float SCREEN_CENTER_Y = SCREEN_SIZE_Y / 2.f;

constexpr float ASTEROID_SPEED = 10.f;
constexpr float BEETLE_SPEED = 12.f;

constexpr float WASP_ACCELERATION = 10.f;
constexpr float WASP_MAX_SPEED = 30.f;

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
constexpr float BULLET_COOLDOWN = 0.1f;

constexpr float PLAYER_SHIP_ACCELERATION = 30.f;
constexpr float PLAYER_SHIP_TURN_SPEED = 300.f;
constexpr float PLAYER_SHIP_PHYSICS_RADIUS = 1.75f;
constexpr float PLAYER_SHIP_COSMETIC_RADIUS = 2.25f;
constexpr int	NUM_VERTS = 6;

constexpr float	SHAKE_TRAUMA_AMOUNT = 1;

constexpr int NUM_BLACK_HOLE_VERTS = 192;
constexpr int NUM_BLACK_HOLE_TRIS = 64;
constexpr int NUM_BLACK_HOLE_SIDES = 64;

constexpr int NEW_MAX_STARS = 500;
constexpr int STAR_PER_VERTS = 3;
constexpr int NUM_STAR_VERTS = NEW_MAX_STARS * STAR_PER_VERTS;

constexpr int MAX_INTERACTABLES = 20;
constexpr float INTERACTABLES_PHYSICS_RADIUS = 1.2f;
constexpr float INTERACTABLES_COSMETIC_RADIUS = 2.0f;


void DebugDrawRing(Vec2 const& center, float radius, float thickness, Rgba8 const& color);
void DebugDrawLine(Vec2 const& entity1Pos, Vec2 const& entity2Pos, float thickness, Rgba8 const& color);
