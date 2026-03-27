#pragma once

#include "Engine/Math/Vec2.hpp"
#include "Game/Entity.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Renderer/Texture.hpp"

//------------------------------------------------------------------------------

class XboxController;
class Camera;

class Player
{
public:
	Player( Game* owner );
	~Player();
	virtual void Update( float deltaSeconds );
	virtual void Render() const;

	void UpdateKeyboardAndControllerInput( float deltaSeconds, XboxController const& controller );
	void PrintControlsToDevConsole();
	void ApplyMovement( Vec3 const& localMoveDir, float speed, float deltaSeconds );
	virtual Mat44 GetModelToWorldTransform() const;


	std::vector<Vertex> m_vertexes;
	Rgba8				m_color = Rgba8( 255, 255, 255 );
	Texture*			m_texture = nullptr;
	Camera*				m_worldCamera = nullptr;
	Game*				m_game = nullptr;
	Vec3				m_position;
	Vec3				m_velocity;
	EulerAngles			m_orientation;
	EulerAngles			m_angularVelocity;
private:
	void SetUpCamera();
};