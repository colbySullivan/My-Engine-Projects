#pragma once

#include "Game/Controller.hpp"
#include "Game/WeaponDefinition.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/Timer.hpp"
#include "Engine/Renderer/Texture.hpp"


//-----------------------------------------------------------------------------------------------
class PlayerController : public Controller
{
public:
	PlayerController( Map* map, Camera* camera );
	virtual ~PlayerController();

	virtual void Update( float deltaSeconds ) override;
	virtual void RenderUI() const override;
	void UpdateInput( float deltaSeconds );
	void UpdateCamera( float deltaSeconds );

	Camera* GetCamera() const;
	Vec3 GetRaycastDirection() const;
	Vec3 GetCameraPosition() const;

	bool		m_isFreeFlyMode;
	bool		m_canProcessPossessInput = true;

private:
	void HandleActorInput( float deltaSeconds );
	void HandleFreeFlyInput( float deltaSeconds );
	void ProcessMovementInput( float deltaSeconds );
	void ProcessLookInput( float deltaSeconds );
	void ToggleCameraMode();
	void PossessNextActor();
	void ProcessWeaponChangeInput();
	void FireRaycastWeapon( Actor* actor, const WeaponDefinition* weaponDef, const Vec3& direction );
	Actor* FindActorAtPosition( const Vec3& position );

	void RenderWeaponUI() const;
private:
	Camera* m_camera;
	Vec3		m_freeFlyCameraPosition;
	EulerAngles m_freeFlyCameraOrientation;
	Actor*		m_hitActor;
	Texture* m_hudTexture = nullptr;
	Texture* m_reticleTexture = nullptr;

	static constexpr float MOUSE_SENSITIVITY = 0.075f;
	static constexpr float FREE_FLY_SPEED = 2.f;
	static constexpr float FREE_FLY_SPRINT_SPEED = 15.f;
	static constexpr float MAX_CAMERA_PITCH = 85.f;
};