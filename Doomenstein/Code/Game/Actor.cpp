#include "Game/Actor.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Game/Player.hpp"

//-----------------------------------------------------------------------------------------------
Actor::Actor( Game* owner, Vec3 start, Vec3 end, float radius, int numSlices )
	: m_radius( radius )
	, m_game( owner )
	, m_position( start )
	, m_start( start )
	, m_end( end )
{
	m_height = m_end.z - m_start.z;
	Vec3 startZeroed = Vec3( 0.f, 0.f, 0.f );
	Vec3 endZeroed = Vec3( 0.f, 0.f, m_height );
	AddVertsForCylinder3D( m_vertexes, startZeroed, endZeroed, radius, m_color, AABB2::ZERO_TO_ONE, numSlices );
}

//-----------------------------------------------------------------------------------------------
Actor::Actor( Game* owner, SpawnInfo spawnInfo )
	: m_game( owner )
	, m_position( spawnInfo.m_spawnLocation )
	, m_orientation( spawnInfo.m_actorOrientation )
{
	if ( spawnInfo.m_name == "Marine" )
	{
		CreatePlayer();
	}

	if ( spawnInfo.m_name == "Demon" )
	{
		CreateDemon();
	}

	if ( spawnInfo.m_name == "SpawnPoint" )
	{
		CreateSpawnPoint();
	}

	if ( spawnInfo.m_name == "PlasmaProjectile" )
	{
		CreateProjectile( spawnInfo.m_name );
	}
	
	InitializeWeapons();
}

Actor::Actor( ActorDefinition* ActorDef )
	: m_actorDef( ActorDef )
{
	Vec3 startZeroed = Vec3( 0.f, 0.f, 0.f );
	Vec3 endZeroed = Vec3( 0.f, 0.f, m_actorDef->m_physicsHeight );
	AddVertsForCylinder3D( m_vertexes, startZeroed, endZeroed, m_actorDef->m_physicsRadius, m_color, AABB2::ZERO_TO_ONE, 32 );
}

//-----------------------------------------------------------------------------------------------
Actor::~Actor()
{

}

//-----------------------------------------------------------------------------------------------
void Actor::Update( [[maybe_unused]] float deltaSeconds )
{
	if ( m_currentController )
	{
		m_currentController->Update( deltaSeconds );
	}

	if ( m_actorDef && m_actorDef->m_simulated && !m_isDead )
	{
		CheckIfShouldDie();
		UpdateMove();
	}

	if ( m_isDead )
	{
		UpdateDeathAnimation( deltaSeconds );
	}
}

//-----------------------------------------------------------------------------------------------
void Actor::Render() const
{
	if ( m_currentController != nullptr && m_currentController->IsPlayerControlled() )
	{
		return;
	}

	Mat44 modelToWorld = GetModelToWorldTransform();
	g_engine->m_render->BindShader( nullptr );

	Rgba8 tintColor = m_isDead ? Rgba8( 64, 64, 64, 255 ) : Rgba8( 255, 255, 255, 255 );
	g_engine->m_render->SetModelConstants( modelToWorld, tintColor );

	g_engine->m_render->m_desiredBlendMode = BlendMode::OPAQUE;
	g_engine->m_render->m_desiredRasterizerMode = RasterizerMode::WIREFRAME_CULL_BACK;
	g_engine->m_render->BindTexture( m_texture );
	g_engine->m_render->DrawVertexArray( ( int )m_vertexes.size(), m_vertexes.data() );

	Rgba8 solidColor = m_isDead ? Rgba8( m_modelColor.r / 4, m_modelColor.g / 4, m_modelColor.b / 4, 255 ) : m_modelColor;
	g_engine->m_render->SetModelConstants( modelToWorld, solidColor );
	g_engine->m_render->m_desiredRasterizerMode = RasterizerMode::SOLID_CULL_BACK;
	g_engine->m_render->DrawVertexArray( ( int )m_vertexes.size(), m_vertexes.data() );
}

//-----------------------------------------------------------------------------------------------
void Actor::UpdateMove()
{
	float deltaSeconds =  (float) g_engine->m_systemClock->GetDeltaSeconds();
	Vec3 dragForce = -m_velocity * m_actorDef->m_drag;
	m_acceleration += dragForce;
	m_velocity += m_acceleration * deltaSeconds;
	m_position += m_velocity * deltaSeconds;
	m_acceleration = Vec3( 0.f, 0.f, 0.f );
}

//------------------------------------------------------------------------------
void Actor::SetPosXY( float x, float y )
{
	m_position.x = x;
	m_position.y = y;
}

//------------------------------------------------------------------------------
Mat44 Actor::GetModelToWorldTransform() const
{
	Mat44 model = m_orientation.GetAsMatrix_IFwd_JLeft_KUp();
	model.SetTranslation3D( m_position );
	return model;
}

//-----------------------------------------------------------------------------------------------
void Actor::OnPossessed( Controller* newController )
{
	if ( m_currentController && m_currentController != newController )
	{
		m_savedAIController = m_currentController;
	}

	m_currentController = newController;
}

//------------------------------------------------------------------------------
void Actor::OnUnpossessed()
{
	m_currentController = nullptr;

	if ( m_savedAIController )
	{
		m_currentController = m_savedAIController;
		m_savedAIController = nullptr;
	}
}

bool Actor::IsPossessed() const
{
	return m_currentController != nullptr;
}

//------------------------------------------------------------------------------
void Actor::ApplyMovement( Vec3 localMoveDir, float speed, float deltaSeconds )
{
	if ( localMoveDir == Vec3( 0.f, 0.f, 0.f ) )
	{
		return;
	}

	Mat44 rotationMatrix = m_game->m_player->m_orientation.GetAsMatrix_IFwd_JLeft_KUp();

	Vec3 forward = rotationMatrix.GetIBasis3D();
	forward.z = 0.f;
	Vec3 left = rotationMatrix.GetJBasis3D();
	Vec3 up = rotationMatrix.GetKBasis3D();

	Vec3 worldDir = forward * localMoveDir.x + left * localMoveDir.y + up * localMoveDir.z;
	worldDir = worldDir.GetNormalized();

	m_position += worldDir * speed * deltaSeconds;
}

//-----------------------------------------------------------------------------------------------
void Actor::CreatePlayer()
{
	m_actorDef = ActorDefinition::GetByName( "Marine" );
	m_height = m_actorDef->m_physicsHeight;
	m_radius = m_actorDef->m_physicsRadius;
	m_health = m_actorDef->m_health;
	m_attackTimer = new Timer( 1.f );
	m_attackTimer->Start();

	Vec3 startZeroed = Vec3( 0.f, 0.f, 0.f );
	m_height = m_actorDef->m_physicsHeight;
	Vec3 endZeroed = Vec3( 0.f, 0.f, m_actorDef->m_physicsHeight );
	m_modelColor = Rgba8( 0, 255, 0 );
	AddVertsForCylinder3D( m_vertexes, startZeroed, endZeroed, m_actorDef->m_physicsRadius, Rgba8( 255, 255, 255 ), AABB2::ZERO_TO_ONE, 32);

	Vec3 forwardNormal = m_orientation.GetForwardDir_IFwd_JLeft_KUp().GetNormalized();
	Vec3 beakBase = Vec3( forwardNormal.x * m_actorDef->m_physicsRadius, forwardNormal.y * m_actorDef->m_physicsRadius, m_actorDef->m_eyeHeight );
	Vec3 beakTip = beakBase + forwardNormal * 0.1f;
	AddVertsForCone3D( m_vertexes, beakBase, beakTip, 0.1f, Rgba8( 255, 255, 255 ), AABB2::ZERO_TO_ONE, 32 );
}

//-----------------------------------------------------------------------------------------------
void Actor::CreateDemon()
{
	m_actorDef =  ActorDefinition::GetByName( "Demon");
	m_height = m_actorDef->m_physicsHeight;
	m_radius = m_actorDef->m_physicsRadius;
	m_health = m_actorDef->m_health;
	m_attackTimer = new Timer( 1.f );
	m_attackTimer->Start();

	Vec3 startZeroed = Vec3( 0.f, 0.f, 0.f );
	Vec3 endZeroed = Vec3( 0.f, 0.f, m_height );
	m_modelColor = Rgba8( 255, 0, 0 );
	AddVertsForCylinder3D( m_vertexes, startZeroed, endZeroed, m_actorDef->m_physicsRadius, Rgba8( 255, 255, 255 ), AABB2::ZERO_TO_ONE, 32 );
	
	Vec3 forwardNormal = Vec3( 1.f, 0.f, 0.f );
	Vec3 beakBase = Vec3( forwardNormal.x * m_actorDef->m_physicsRadius, forwardNormal.y * m_actorDef->m_physicsRadius, m_actorDef->m_eyeHeight );
	Vec3 beakTip = beakBase + forwardNormal * 0.1f;
	AddVertsForCone3D( m_vertexes, beakBase, beakTip, 0.1f, Rgba8( 255, 255, 255 ), AABB2::ZERO_TO_ONE, 32 );
}

//-----------------------------------------------------------------------------------------------
void Actor::CreateSpawnPoint()
{
	m_actorDef = ActorDefinition::GetByName( "SpawnPoint" );
	Vec3 startZeroed = Vec3( 0.f, 0.f, 0.f );
	Vec3 endZeroed = Vec3( 0.f, 0.f, 1.0f );
	m_modelColor = Rgba8( 255, 255, 0 );
	m_height = 0.f;           
	m_radius = 0.f;           
	m_canBePushed = false;    
}

//------------------------------------------------------------------------------
void Actor::CreateProjectile( std::string name )
{
	m_actorDef = ActorDefinition::GetByName( name );
	m_actorHandle = m_game->m_playerController->GetActorHandle();
	m_height = m_actorDef->m_physicsHeight;
	m_radius = m_actorDef->m_physicsRadius;
	m_health = 1;
	m_modelColor = Rgba8::BLUE;
	Vec3 startZeroed = Vec3( 0.f, 0.f, 0.f );
	Vec3 endZeroed = Vec3( 0.f, 0.f, m_height );
	AddVertsForCylinder3D( m_vertexes, startZeroed, endZeroed, m_radius, Rgba8( 255, 255, 255 ), AABB2::ZERO_TO_ONE, 32 );
}

//-----------------------------------------------------------------------------------------------
void Actor::MoveInDirection( const Vec3& direction, float speed )
{
	if ( m_isDead || !m_actorDef )
		return;

	Vec3 moveForce = direction * speed * m_actorDef->m_drag;
	AddForce( moveForce );
}

void Actor::CheckIfShouldDie()
{
	if ( m_health <= 0.f && m_actorDef->m_simulated )
	{
		m_isDead = true;
		m_deathAnimationTime = 0.f;

		if ( m_actorDef->m_eyeHeight > 0.f )
		{
			m_deathCameraStartHeight = m_actorDef->m_eyeHeight;
		}

		if ( !m_corpseTimer && m_actorDef->m_corpseLifetime > 0.f )
		{
			m_corpseTimer = new Timer( m_actorDef->m_corpseLifetime );
			m_corpseTimer->Start();
		}

		m_velocity = Vec3( 0.f, 0.f, 0.f );
		m_acceleration = Vec3( 0.f, 0.f, 0.f );
	}
}

//-----------------------------------------------------------------------------------------------
bool Actor::IsDead() const
{
	return m_isDead;
}

//-----------------------------------------------------------------------------------------------
void Actor::Attacked( float damage, Vec3 impulse )
{
	AddImpulse( impulse );
	m_health -= (int)damage;
}

//-----------------------------------------------------------------------------------------------
void Actor::AddForce( const Vec3& force )
{
	m_acceleration += force;
}

//-----------------------------------------------------------------------------------------------
void Actor::InitializeWeapons()
{
	if ( !m_actorDef )
	{
		return;
	}

	m_weapons.clear();
	
	for ( const std::string& weaponName : m_actorDef->m_weaponNames )
	{
		const WeaponDefinition* weaponDef = WeaponDefinition::GetByName( weaponName );
		if ( weaponDef )
		{
			m_weapons.push_back( weaponDef );
		}
	}

	if ( !m_weapons.empty() )
	{
		m_currentWeaponIndex = 0;
		m_weaponDef = m_weapons[0];
		
		if ( m_weaponRefireTimer )
		{
			delete m_weaponRefireTimer;
		}
		m_weaponRefireTimer = new Timer( m_weaponDef->m_refireTime );
		m_weaponRefireTimer->Start();
	}
}

//-----------------------------------------------------------------------------------------------
void Actor::UpdateDeathAnimation( float deltaSeconds )
{
	if ( !m_corpseTimer || !m_actorDef )
	{
		return;
	}

	m_deathAnimationTime += deltaSeconds;

	float elapsedFraction = (float)m_corpseTimer->GetElapsedFraction();
	unsigned char newAlpha = ( unsigned char )( 255.f * ( 1.f - elapsedFraction ) );
	m_modelColor.a = newAlpha;
	for ( Vertex& vertex : m_vertexes )
	{
		vertex.m_color.a = newAlpha;
	}
}

//-----------------------------------------------------------------------------------------------
void Actor::EquipWeapon( int weaponIndex )
{
	if ( weaponIndex < 0 || weaponIndex >= (int)m_weapons.size() )
		return;

	m_currentWeaponIndex = weaponIndex;
	m_weaponDef = m_weapons[weaponIndex];
	
	if ( m_weaponRefireTimer )
	{
		delete m_weaponRefireTimer;
	}
	m_weaponRefireTimer = new Timer( m_weaponDef->m_refireTime );
	m_weaponRefireTimer->Start();
}

//-----------------------------------------------------------------------------------------------
void Actor::EquipWeaponByName( const std::string& weaponName )
{
	for ( int i = 0; i < (int)m_weapons.size(); ++i )
	{
		if ( m_weapons[i]->m_name == weaponName )
		{
			EquipWeapon( i );
			return;
		}
	}
}

//-----------------------------------------------------------------------------------------------
const WeaponDefinition* Actor::GetCurrentWeapon() const
{
	return m_weaponDef;
}

//------------------------------------------------------------------------------
Controller* Actor::GetController() const
{
	return m_currentController;
}

bool Actor::IsReadyToDestroy() const
{
	if ( !m_isDead )
	{
		return false;
	}

	if ( !m_actorDef || m_actorDef->m_corpseLifetime <= 0.f )
	{
		return true;
	}

	if ( m_corpseTimer && m_corpseTimer->DecrementPeriodIfElapsed() )
	{
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------------------------
int Actor::GetCurrentWeaponIndex() const
{
	return m_currentWeaponIndex;
}

//-----------------------------------------------------------------------------------------------
int Actor::GetWeaponCount() const
{
	return (int)m_weapons.size();
}

//-----------------------------------------------------------------------------------------------
bool Actor::CanFireWeapon() const
{
	if ( !m_weaponDef || !m_weaponRefireTimer || m_isDead )
	{
		return false;
	}
	
	return m_weaponRefireTimer->HasPeriodElapsed();
}

//-----------------------------------------------------------------------------------------------
void Actor::FireWeapon()
{
	if ( !CanFireWeapon() )
	{
		return;
	}

	m_weaponRefireTimer->Start();
}

//-----------------------------------------------------------------------------------------------
void Actor::ResetWeaponTimer()
{
	if ( m_weaponRefireTimer )
	{
		m_weaponRefireTimer->Start();
	}
}

//------------------------------------------------------------------------------
void Actor::AddImpulse( const Vec3& impulse )
{
	m_velocity += impulse;
}

//------------------------------------------------------------------------------
void Actor::AttackedBy( Actor* attacker, float damage )
{
	Vec3 impulse = Vec3( 0.f, 0.f, 0.f );

	const WeaponDefinition* attackerWeapon = attacker->GetCurrentWeapon();
	if ( attackerWeapon )
	{
		if ( !attackerWeapon->m_projectileActorName.empty() )
		{
			const ActorDefinition* projectileActor = ActorDefinition::GetByName( attackerWeapon->m_projectileActorName );
			if ( projectileActor )
			{
				impulse = attacker->m_orientation.GetForwardDir_IFwd_JLeft_KUp().GetNormalized();
				impulse *= projectileActor->m_impulseOnCollide;
			}
		}
		else if ( attackerWeapon->m_meleeCount > 0 )
		{
			impulse = attacker->m_orientation.GetForwardDir_IFwd_JLeft_KUp().GetNormalized();
			impulse *= attackerWeapon->m_meleeImpulse;
		}
		else if ( attackerWeapon->m_rayCount > 0 )
		{
			impulse = attacker->m_orientation.GetForwardDir_IFwd_JLeft_KUp().GetNormalized();
			impulse *= attackerWeapon->m_rayImpulse;
		}
	}

	Attacked( damage, impulse );

	if ( m_currentController && attacker )
	{
		m_currentController->DamagedBy( attacker, damage );
	}
}

//------------------------------------------------------------------------------
float Actor::GetDeathCameraHeight() const
{
	float cameraFallTimer = 1.0f;
	float t = GetClamped( m_deathAnimationTime / cameraFallTimer, 0.f, 1.f );
	float currentHeight = Interpolate( m_deathCameraStartHeight, 0.1f, t );
	return currentHeight;
}