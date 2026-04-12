#pragma once

#include "Game/Controller.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Renderer/Camera.hpp"


//-----------------------------------------------------------------------------------------------
class AiController : public Controller
{
public:
	AiController( Map* map, Camera* camera );
	virtual ~AiController();

	virtual void Update( float deltaSeconds ) override;

	void HandleMovement( float deltaSeconds );

	Vec3 GetRaycastDirection() const;

private:
	Vec3		m_freeFlyCameraPosition;
	EulerAngles m_freeFlyCameraOrientation;
	EulerAngles m_orientation;

	static constexpr float MOUSE_SENSITIVITY = 0.075f;
	static constexpr float FREE_FLY_SPEED = 2.f;
	static constexpr float FREE_FLY_SPRINT_SPEED = 15.f;
	static constexpr float MAX_CAMERA_PITCH = 85.f;
};