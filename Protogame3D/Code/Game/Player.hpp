#pragma once

#include "Engine/Math/Vec2.hpp"
#include "Game/Entity.hpp"
#include "Engine/Core/Vertex.hpp"

//------------------------------------------------------------------------------

class XboxController;
class Camera;

class Player : public Entity
{
public:
	Player( Game* owner );
	~Player();
	virtual void Update( float deltaSeconds ) override;
	virtual void Render() const override;

	void UpdateKeyboardAndControllerInput( float deltaSeconds, XboxController const& controller );
	void PrintControlsToDevConsole();
	void ApplyMovement( Vec3 const& localMoveDir, float speed, float deltaSeconds );

	std::vector<Vertex> m_vertexes;
	Rgba8				m_color = Rgba8( 255, 255, 255 );
	Texture*			m_texture = nullptr;
	Camera*				m_worldCamera = nullptr;
private:
	void SetUpCamera();
};