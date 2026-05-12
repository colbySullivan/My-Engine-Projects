#include "Game/Actor.hpp"
#include "Game/PowerUpDefinitions.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Game/Player.hpp"
#include "Game/SpriteAnimationGroupDefinition.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Engine/Core/EngineCommon.hpp"

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

    else if ( spawnInfo.m_name == "Demon" )
    {
        CreateDemon();
    }

    else if ( spawnInfo.m_name == "SpawnPoint" )
    {
        CreateSpawnPoint();
    }
    else if ( spawnInfo.m_name == "PackAPunch" )
    {
        CreatePackAPunch();
    }
    
    else if ( spawnInfo.m_name == "DemonSpawner" )
    {
        CreateDemonSpawner();
	}

    else
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
	delete m_attackTimer;
	m_attackTimer = nullptr;

	delete m_animTimer;
	m_animTimer = nullptr;

	delete m_weaponRefireTimer;
	m_weaponRefireTimer = nullptr;

	delete m_weaponAnimTimer;
	m_weaponAnimTimer = nullptr;

	delete m_corpseTimer;
	m_corpseTimer = nullptr;

	delete m_spawnTimer;
	m_spawnTimer = nullptr;

	delete m_currentSpriteSheet;
	m_currentSpriteSheet = nullptr;

	delete m_weaponSpriteSheet;
	m_weaponSpriteSheet = nullptr;

	delete m_currentWeaponAnim;
	m_currentWeaponAnim = nullptr;

	m_currentController = nullptr;
	m_savedAIController = nullptr;

	m_texture = nullptr;
}

//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
void Actor::Update( [[maybe_unused]] float deltaSeconds )
{
    if ( deltaSeconds <= 0.f )
    {
        return;
	}

	if ( IsDemonSpawner() )
	{
		UpdateSpawner( deltaSeconds );
	}

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
        return;
    }

    if ( m_isAttacking && m_weaponAnimTimer && m_weaponAnimTimer->HasPeriodElapsed() )
    {
        m_isAttacking = false;
        UpdateWeaponAnimation();
        SetCurrentAnimGroup( "Walk" );
    }

    if ( m_animTimer && m_animTimer->DecrementPeriodIfElapsed() )
    {
        if ( m_isAttacking && !m_weaponAnimTimer )
        {
            m_isAttacking = false;
        }

        if ( !m_isAttacking && !m_isDead )
        {
            SetCurrentAnimGroup( "Walk" );
        }
    }
}

//-----------------------------------------------------------------------------------------------
void Actor::Render( Camera* playerCamera ) const
{
    if ( m_currentSpriteSheet )
    {
        RenderAnimSprite( playerCamera );
    }
    else
    {
        Mat44 modelToWorld = GetModelToWorldTransform();
        g_engine->m_render->BindShader( nullptr );

        Rgba8 tintColor = m_isDead ? Rgba8( 64, 64, 64, 255 ) : Rgba8( 255, 255, 255, 255 );
        g_engine->m_render->SetModelConstants( modelToWorld, tintColor );

        g_engine->m_render->m_desiredBlendMode = BlendMode::OPAQUE;
        g_engine->m_render->m_desiredRasterizerMode = RasterizerMode::WIREFRAME_CULL_BACK;
        g_engine->m_render->BindTexture( m_texture );
        g_engine->m_render->DrawVertexArray( ( int )m_vertexes.size(), m_vertexes.data() );
        g_engine->m_render->BindShader( nullptr );

        Rgba8 solidColor = m_isDead ? Rgba8( m_modelColor.r / 4, m_modelColor.g / 4, m_modelColor.b / 4, 255 ) : m_modelColor;
        g_engine->m_render->SetModelConstants( modelToWorld, solidColor );
        g_engine->m_render->m_desiredRasterizerMode = RasterizerMode::SOLID_CULL_BACK;
        g_engine->m_render->DrawVertexArray( ( int )m_vertexes.size(), m_vertexes.data() );
    }
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

//-----------------------------------------------------------------------------------------------
bool Actor::IsPossessed() const
{
    return m_currentController != nullptr;
}

//-----------------------------------------------------------------------------------------------
void Actor::LoadActorSounds()
{
    std::string soundPath = m_actorDef->m_sounds[0].m_filePath;
    g_engine->m_audio->CreateOrGetSound( soundPath, FMOD_3D );
    soundPath = m_actorDef->m_sounds[1].m_filePath;
    g_engine->m_audio->CreateOrGetSound( soundPath, FMOD_3D ); 
}

//-----------------------------------------------------------------------------------------------
void Actor::LoadWeaponSounds()
{
    for ( const WeaponDefinition* weapon : m_weapons )
    {
        for ( const WeaponSoundDefinition& soundDef : weapon->m_sounds )
        {
            g_engine->m_audio->CreateOrGetSound( soundDef.m_filePath, FMOD_3D );
        }
    }
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
    LoadActorSounds();

    const SpriteAnimationDefinition* spriteAnimDef = SpriteAnimationDefinition::GetByName( "Marine" );
    if ( spriteAnimDef )
    {
        m_spriteAnimationDef = spriteAnimDef;
        m_currentAnimGroup = &spriteAnimDef->m_animationGroups[0];
        const char* spriteSheetPath = spriteAnimDef->m_spriteSheetPath.c_str();
        Texture* spriteSheetTexture = g_engine->m_render->CreateOrGetTextureFromFile( spriteSheetPath );
        m_currentSpriteSheet = new SpriteSheet( *spriteSheetTexture, spriteAnimDef->m_cellCount );
    }

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
    LoadActorSounds();

    const SpriteAnimationDefinition* spriteAnimDef = SpriteAnimationDefinition::GetByName( "Demon" );
    if ( spriteAnimDef )
    {
        m_spriteAnimationDef = spriteAnimDef;
        m_currentAnimGroup = &spriteAnimDef->m_animationGroups[0];
        const char* spriteSheetPath = spriteAnimDef->m_spriteSheetPath.c_str();
        Texture* spriteSheetTexture = g_engine->m_render->CreateOrGetTextureFromFile( spriteSheetPath );
        m_currentSpriteSheet = new SpriteSheet( *spriteSheetTexture, spriteAnimDef->m_cellCount );
    }

    Vec3 startZeroed = Vec3( 0.f, 0.f, 0.f );
    Vec3 endZeroed = Vec3( 0.f, 0.f, m_height );
    m_modelColor = Rgba8::WHITE;
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
    //m_actorHandle = m_game->m_playerController->GetActorHandle();

    const SpriteAnimationDefinition* spriteAnimDef = SpriteAnimationDefinition::GetByName( name );
    if ( spriteAnimDef )
    {
        m_spriteAnimationDef = spriteAnimDef;
        m_currentAnimGroup = &spriteAnimDef->m_animationGroups[0];
        const char* spriteSheetPath = spriteAnimDef->m_spriteSheetPath.c_str();
        Texture* spriteSheetTexture = g_engine->m_render->CreateOrGetTextureFromFile( spriteSheetPath );
        m_currentSpriteSheet = new SpriteSheet( *spriteSheetTexture, spriteAnimDef->m_cellCount );
    }

    m_height = m_actorDef->m_physicsHeight;
    m_radius = m_actorDef->m_physicsRadius;
    m_health = 1;
    m_modelColor = Rgba8::WHITE;
    Vec3 startZeroed = Vec3( 0.f, 0.f, 0.f );
    Vec3 endZeroed = Vec3( 0.f, 0.f, m_height );
    AddVertsForCylinder3D( m_vertexes, startZeroed, endZeroed, m_radius, Rgba8( 255, 255, 255 ), AABB2::ZERO_TO_ONE, 32 );
}

//------------------------------------------------------------------------------
void Actor::CreatePackAPunch()
{
	m_actorDef = ActorDefinition::GetByName( "PackAPunch" );
	//m_actorHandle = m_game->m_playerController->GetActorHandle();

	const SpriteAnimationDefinition* spriteAnimDef = SpriteAnimationDefinition::GetByName( "PackAPunch" );
	if ( spriteAnimDef )
	{
		m_spriteAnimationDef = spriteAnimDef;
		m_currentAnimGroup = &spriteAnimDef->m_animationGroups[0];
		const char* spriteSheetPath = spriteAnimDef->m_spriteSheetPath.c_str();
		Texture* spriteSheetTexture = g_engine->m_render->CreateOrGetTextureFromFile( spriteSheetPath );
		m_currentSpriteSheet = new SpriteSheet( *spriteSheetTexture, spriteAnimDef->m_cellCount );
	}

	m_height = m_actorDef->m_physicsHeight;
	m_radius = m_actorDef->m_physicsRadius;
 	m_modelColor = Rgba8::WHITE;
	m_canBePushed = false;
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
    if ( m_health <= 0.f && !m_isDead )
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

        if ( m_animTimer )
        {
            delete m_animTimer;
            m_animTimer = nullptr;
        }

        //if ( m_currentController && m_currentController->IsPlayerControlled() )
        //{
            //PlayerController* player = static_cast< PlayerController* >( m_currentController );
   //         //player->m_deathCount++;
			//if ( m_lastAttacker && m_lastAttacker->m_currentController && m_lastAttacker->m_currentController->IsPlayerControlled() )
			//{
   //             player->m_killCount++;
			//}
        //}

        if ( m_lastAttacker && m_lastAttacker->m_currentController )
		{
            PlayerController* player = static_cast< PlayerController* >( m_lastAttacker->m_currentController );
            if ( player )
            {
			    player->AddGold( m_actorDef->m_goldOnDeath );
            }
        }
		TryToPlaySound( g_engine->m_audio->CreateOrGetSound( m_actorDef->GetSoundByName("Death"), FMOD_3D ) );
		if ( m_owner && ( m_actorDef->m_dieOnCollide || m_actorDef->m_dieOnSpawn ) )
		{
			DebugAddWorldBillboardText( Stringf( "%1.1f", m_actorDef->m_damageOnCollide.m_min ), m_position, 0.05f, Vec2( 0.5f, 0.5f ), 0.2f, Rgba8( 255, 0, 0, 255 ), Rgba8( 255, 255, 255, 255 ) );
		}
        SetCurrentAnimGroup( "Death" );
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
    //AddImpulse( impulse );
    m_health -= (int)damage;
    
    if ( m_health <= 0 )
    {
        return;
    }
    
    if ( !m_isDead )
    {
        TryToPlaySound( g_engine->m_audio->CreateOrGetSound( m_actorDef->GetSoundByName("Hurt"), FMOD_3D ) );
		SetCurrentAnimGroup( "Hurt" );
    }
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
		LoadWeaponSounds();
        m_currentWeaponIndex = 0;
        m_weaponDef = m_weapons[0];
        
        if ( m_weaponRefireTimer )
        {
            delete m_weaponRefireTimer;
        }
        m_weaponRefireTimer = new Timer( m_weaponDef->m_refireTime );
        m_weaponRefireTimer->Start();
        UpdateWeaponAnimation();
    }
}

//-----------------------------------------------------------------------------------------------
void Actor::UpdateDeathAnimation( float deltaSeconds )
{
    if ( !m_corpseTimer || !m_actorDef )
    {
        return;
    }

    if ( !m_currentAnimGroup || m_currentAnimGroup->m_name != "Death" )
    {
        SetCurrentAnimGroup( "Death" );
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
void Actor::RenderAnimSprite( Camera* playerCamera ) const
{
    if ( !m_spriteAnimationDef || !m_currentAnimGroup )
    {
        return;
    }

    const DirectionalAnimInfo* dirAnim = GetDirectionalAnimForCamera( m_currentAnimGroup, playerCamera );

    if ( dirAnim )
    {
        int startFrame = dirAnim->startFrame;
        int endFrame = dirAnim->endFrame;
        float secondsPerFrame = m_currentAnimGroup->m_secondsPerFrame;

        Mat44 modelToWorld = GetModelToWorldTransform();
        Rgba8 solidColor = m_isDead ? Rgba8( m_modelColor.r / 4, m_modelColor.g / 4, m_modelColor.b / 4, 255 ) : m_modelColor;
        g_engine->m_render->SetModelConstants( modelToWorld, solidColor );
        g_engine->m_render->m_desiredBlendMode = BlendMode::OPAQUE;
        g_engine->m_render->BindTexture( m_texture );
        g_engine->m_render->BindShader( nullptr );
        g_engine->m_render->m_desiredRasterizerMode = RasterizerMode::SOLID_CULL_BACK;
        const SpriteAnimDefinition* currentAnim = new SpriteAnimDefinition( *m_currentSpriteSheet, startFrame, endFrame, secondsPerFrame, m_currentAnimGroup->m_playbackMode );

        float animElapsedTime = ( float )( g_engine->m_systemClock->GetTotalSeconds() - m_currentAnimStartTime );
        const SpriteDefinition& actorSprite = currentAnim->GetSpriteDefAtTime( animElapsedTime );

        std::vector<Vertex> billboardVerts;

        Vec2 actorUVsMin, actorUVsMax;
        actorSprite.GetUVs( actorUVsMin, actorUVsMax );

        Vec2 size = m_spriteAnimationDef->m_size;
        Vec2 pivot = m_spriteAnimationDef->m_pivot;

        float spriteBottomZ = m_position.z;
        float spriteCenterZ = spriteBottomZ + ( size.y * pivot.y );
        Vec3 spriteCenter = Vec3( m_position.x, m_position.y, spriteCenterZ );

        float halfWidth = size.x * 0.5f;
        float halfHeight = size.y * 0.5f;

        float verticalOffset = ( 0.5f - pivot.y ) * size.y;
        Vec3 mins3D = Vec3( 0.f, -halfWidth, -halfHeight + verticalOffset );
        Vec3 maxs3D = Vec3( 0.f, halfWidth, halfHeight + verticalOffset );
        AABB3 billboardBox( mins3D, maxs3D );

        AABB2 explosionUVs( actorUVsMin, actorUVsMax );
        AddVertsForAABB3D( billboardVerts, billboardBox, Rgba8( 255, 255, 255 ), explosionUVs );

        BillboardType billboardType = m_spriteAnimationDef->m_billboardType;
        Mat44 billboardMat = GetBillboardTransform( billboardType, playerCamera->GetCameraToWorldTransform(), spriteCenter );
        g_engine->m_render->SetModelConstants( billboardMat, Rgba8::WHITE );
        g_engine->m_render->BindTexture( &m_currentSpriteSheet->GetTexture() );
        g_engine->m_render->DrawVertexArray( billboardVerts );
        g_engine->m_render->BindTexture( nullptr );

        delete currentAnim;
    }
}

//-----------------------------------------------------------------------------------------------
void Actor::EquipWeapon( int weaponIndex )
{
    if ( weaponIndex < 0 || weaponIndex >= (int)m_weapons.size() )
        return;
    
    m_isAttacking = false;

    if ( m_weaponAnimTimer )
    {
        delete m_weaponAnimTimer;
        m_weaponAnimTimer = nullptr;
    }

    UpdateWeaponAnimation();
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
    if ( !m_weaponDef )
    {
        return;
    }

    m_isAttacking = true;
    UpdateWeaponAnimation();

    const WeaponAnimationDefinition* attackAnim = m_weaponDef->GetAnimationByName( "Attack" );
    if ( attackAnim )
    {
        float animDuration = ( attackAnim->m_endFrame - attackAnim->m_startFrame + 1 ) * attackAnim->m_secondsPerFrame;
        if ( !m_weaponAnimTimer )
        {
            m_weaponAnimTimer = new Timer( animDuration );
        }
        else
        {
            delete m_weaponAnimTimer;
            m_weaponAnimTimer = new Timer( animDuration );
        }
        m_weaponAnimTimer->Start();
    }

    if ( !CanFireWeapon() )
    {
        return;
    }

    m_weaponRefireTimer->Start();

    if ( !m_isDead )
    {
        const WeaponSoundDefinition* soundDef = m_weaponDef->GetSoundByName("Fire");
		TryToPlaySound( g_engine->m_audio->CreateOrGetSound( soundDef->m_filePath, FMOD_3D ), soundDef->m_volume );
        SetCurrentAnimGroup( "Attack" );
    }
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

	float modifiedDamage = damage * m_damageMultiplier;

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
    m_lastAttacker = attacker;

    Attacked( modifiedDamage, impulse );

    if ( m_currentController && attacker )
    {
        m_currentController->DamagedBy( attacker, modifiedDamage );
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

void Actor::EquipNextWeapon()
{
    if ( m_weapons.empty() )
        return;

    UpdateWeaponAnimation();
    int nextIndex = ( m_currentWeaponIndex + 1 ) % m_weapons.size();
    EquipWeapon( nextIndex );
}

void Actor::EquipPreviousWeapon()
{
    if ( m_weapons.empty() )
        return;
    UpdateWeaponAnimation();
    int prevIndex = ( m_currentWeaponIndex - 1 + (int)m_weapons.size() ) % (int)m_weapons.size();
    EquipWeapon( prevIndex );
}

const DirectionalAnimInfo* Actor::GetDirectionalAnimForCamera( const SpriteAnimationGroupDefinition* animGroup, Camera* playerCamera ) const
{
    if ( !animGroup || animGroup->m_directionalAnims.empty() || !m_map || !playerCamera )
    {
        return nullptr;
    }

    if ( animGroup->m_directionalAnims.size() == 1 )
    {
        return &animGroup->m_directionalAnims[0];
    }

    Vec3 cameraPos = playerCamera->GetPosition();
    Vec3 cameraToActor = m_position - cameraPos;
    cameraToActor.z = 0.f;
    cameraToActor = cameraToActor.GetNormalized();

    Mat44 actorToWorld = m_orientation.GetAsMatrix_IFwd_JLeft_KUp();
    Mat44 worldToActor = actorToWorld.GetOrthonormalInverse();
    Vec3 localViewDir = worldToActor.TransformVectorQuantity3D( cameraToActor );

    const DirectionalAnimInfo* bestMatchAnim = nullptr;
    float bestDot = -9999999.0f;

    for ( const DirectionalAnimInfo& dirAnim : animGroup->m_directionalAnims )
    {
        Vec3 animDir = dirAnim.direction;
        animDir = animDir.GetNormalized();

        float dot = DotProduct3D( localViewDir, animDir );
        if ( dot > bestDot )
        {
            bestDot = dot;
            bestMatchAnim = &dirAnim;
        }
    }

    return bestMatchAnim;
}

//-----------------------------------------------------------------------------------------------
void Actor::TryToPlaySound( SoundID sound, float volume )
{
    if ( sound == MISSING_SOUND_ID )
    {
        return;
    }
    // #Todo this is not working properly so I left a temp fix
	//bool soundPlaying = g_engine->m_audio->IsPlaying( sound );
 //   if ( !soundPlaying )
 //   {
	//	g_engine->m_audio->StartSoundAt( sound, WorldToFMOD( m_position ) );
	//}
	g_engine->m_audio->StartSoundAt( sound, WorldToFMOD( m_position ), false, volume );

}

void Actor::CreateDemonSpawner()
{
	m_actorDef = ActorDefinition::GetByName( "DemonSpawner" );
	if ( !m_actorDef )
	{
		return;
	}

	m_height = m_actorDef->m_physicsHeight;
	m_radius = m_actorDef->m_physicsRadius;
	m_health = m_actorDef->m_health;
	m_canBePushed = false;

	Vec3 startZeroed = Vec3( 0.f, 0.f, 0.f );
	Vec3 endZeroed = Vec3( 0.f, 0.f, m_height );
	m_modelColor = Rgba8( 255, 0, 0, 200 );
	AddVertsForCylinder3D( m_vertexes, startZeroed, endZeroed, m_radius, m_modelColor, AABB2::ZERO_TO_ONE, 32 );

	m_isSpawnerActive = true;
	m_currentRound = 1;
	StartNewRound();
}

//-----------------------------------------------------------------------------------------------
void Actor::UpdateWeaponAnimation()
{
    if ( !m_weaponDef )
    {
        return;
    }

    const WeaponAnimationDefinition* targetAnim = nullptr;
    
    if ( m_isAttacking )
    {
        targetAnim = m_weaponDef->GetAnimationByName( "Attack" );
    }
    else
    {
        targetAnim = m_weaponDef->GetAnimationByName( "Idle" );
    }
    
    if ( !targetAnim )
    {
        return;
    }

    if ( !m_weaponSpriteSheet || m_weaponSpriteSheet->GetTexture().GetImageFilePath() != targetAnim->m_spriteSheet )
    {
        Texture* weaponTexture = g_engine->m_render->CreateOrGetTextureFromFile( targetAnim->m_spriteSheet.c_str() );
        if ( m_weaponSpriteSheet )
        {
            delete m_weaponSpriteSheet;
        }
        m_weaponSpriteSheet = new SpriteSheet( *weaponTexture, targetAnim->m_cellCount );
    }

    SpriteAnimPlaybackType playbackType = m_isAttacking ? SpriteAnimPlaybackType::ONCE : SpriteAnimPlaybackType::LOOP;
    
    if ( m_currentWeaponAnim )
    {
        delete m_currentWeaponAnim;
    }
    
    m_currentWeaponAnim = new SpriteAnimDefinition( *m_weaponSpriteSheet, targetAnim->m_startFrame, targetAnim->m_endFrame, targetAnim->m_secondsPerFrame, playbackType );
    m_weaponAnimStartTime = g_engine->m_systemClock->GetTotalSeconds();
}

//-----------------------------------------------------------------------------------------------
void Actor::SetCurrentAnimGroup( const std::string& groupName )
{
    if ( !m_spriteAnimationDef )
    {
        return;
    }

    for ( int i = 0; i < m_spriteAnimationDef->m_animationGroups.size(); ++i )
    {
        if ( m_spriteAnimationDef->m_animationGroups[i].m_name == groupName )
        {
            m_currentAnimGroup = &m_spriteAnimationDef->m_animationGroups[i];
            m_currentAnimStartTime = g_engine->m_systemClock->GetTotalSeconds();

            if ( !m_currentAnimGroup->m_directionalAnims.empty() )
            {
                const DirectionalAnimInfo& dirAnim = m_currentAnimGroup->m_directionalAnims[0];
                int startFrame = dirAnim.startFrame;
                int endFrame = dirAnim.endFrame;
                float animDuration = ( endFrame - startFrame + 1 ) * m_currentAnimGroup->m_secondsPerFrame;

                if ( !m_animTimer )
                {
                    m_animTimer = new Timer( animDuration );
                }
                else
                {
                    delete m_animTimer;
                    m_animTimer = new Timer( animDuration );
                }
                m_animTimer->Start();
            }

            return;
        }
    }
}

//-----------------------------------------------------------------------------------------------
void Actor::GetCurrentAnimTimer( Camera* playerCamera )
{
    const DirectionalAnimInfo* dirAnim = GetDirectionalAnimForCamera( m_currentAnimGroup, playerCamera );

    if ( dirAnim )
    {
        int startFrame = dirAnim->startFrame;
        int endFrame = dirAnim->endFrame;
        float animDuration = ( endFrame - startFrame + 1 ) * m_currentAnimGroup->m_secondsPerFrame;
        if ( !m_animTimer )
        {
            m_animTimer = new Timer( animDuration );
        }
        else
        {
            delete m_animTimer;
            m_animTimer = new Timer( animDuration );
        }
        m_animTimer->Start();
    }
}

//-----------------------------------------------------------------------------------------------
bool Actor::IsPackAPunchMachine() const
{
	return m_actorDef && m_actorDef->m_name == "PackAPunch";
}

//-----------------------------------------------------------------------------------------------
bool Actor::IsGunChest() const
{
	return m_actorDef && m_actorDef->m_name == "AmmoKit";
}

//-----------------------------------------------------------------------------------------------
float Actor::GetDistanceToActor( const Actor* other ) const
{
	if ( !other )
	{
		return 99999.f;
	}
	return GetDistance3D( m_position, other->m_position );
}

//-----------------------------------------------------------------------------------------------
bool Actor::TryUpgradeCurrentWeapon()
{
	if ( m_weapons.empty() || !m_weaponDef )
	{
		return false;
	}

	std::string currentWeaponName = m_weaponDef->m_name;
	std::string papWeaponName = "PAP" + currentWeaponName;

	const WeaponDefinition* papWeaponDef = WeaponDefinition::GetByName( papWeaponName );

	if ( !papWeaponDef )
	{
		return false;
	}

	m_weapons[m_currentWeaponIndex] = papWeaponDef;
	m_weaponDef = papWeaponDef;

	if ( m_weaponRefireTimer )
	{
		delete m_weaponRefireTimer;
	}
	m_weaponRefireTimer = new Timer( m_weaponDef->m_refireTime );
	m_weaponRefireTimer->Start();

	UpdateWeaponAnimation();
	LoadWeaponSounds();

	return true;
}

//------------------------------------------------------------------------------
bool Actor::TryToGetNewWeapon()
{
	m_weapons.clear();
    //const WeaponDefinition* weaponDef = WeaponDefinition::GetByName( "PAPShotgunRifle" );
    const WeaponDefinition* weaponDef = WeaponDefinition::GetRandomWeapon();
	if ( weaponDef )
	{
		m_weapons.push_back( weaponDef );
	}
	if ( !m_weapons.empty() )
	{
		LoadWeaponSounds();
		m_currentWeaponIndex = 0;
		m_weaponDef = m_weapons[0];

		if ( m_weaponRefireTimer )
		{
			delete m_weaponRefireTimer;
		}
		m_weaponRefireTimer = new Timer( m_weaponDef->m_refireTime );
		m_weaponRefireTimer->Start();
		UpdateWeaponAnimation();
	}
    return true;
}

//-----------------------------------------------------------------------------------------------
void Actor::StartNewRound()
{
	m_demonsToSpawnThisRound = 2 + ( ( m_currentRound - 1 ) * 3 );
	m_demonsSpawnedThisRound = 0;

	m_spawnDelaySeconds = 1.0f - ( m_currentRound * 0.1f );
	if ( m_spawnDelaySeconds < 0.5f )
	{
		m_spawnDelaySeconds = 0.5f;
	}

	if ( m_spawnTimer )
	{
		delete m_spawnTimer;
	}
	m_spawnTimer = new Timer( m_spawnDelaySeconds );
	m_spawnTimer->Start();
}

//-----------------------------------------------------------------------------------------------
void Actor::UpdateSpawner( [[maybe_unused]] float deltaSeconds )
{
	if ( !m_isSpawnerActive || !m_map )
	{
		return;
	}

	if ( IsRoundComplete() )
	{
		m_currentRound++;
        m_map->StartPickingPowerUp();
		StartNewRound();
		return;
	}

	if ( m_demonsSpawnedThisRound >= m_demonsToSpawnThisRound )
	{
		return;
	}

	int aliveDemonsCount = 0;
	for ( Actor* actor : m_map->m_actorVector )
	{
		if ( actor && actor->m_actorDef &&
			actor->m_actorDef->m_name == "Demon" &&
			!actor->m_isDead )
		{
			aliveDemonsCount++;
		}
	}

	if ( aliveDemonsCount >= m_maxDemonsAliveAtOnce )
	{
		return;
	}

	if ( m_spawnTimer && m_spawnTimer->HasPeriodElapsed() )
	{
		SpawnDemon();
		m_demonsSpawnedThisRound++;
		m_spawnTimer->Start();
	}
}

//-----------------------------------------------------------------------------------------------
void Actor::SpawnDemon()
{
	if ( !m_map )
	{
		return;
	}

	Vec3 spawnOffset = Vec3(
		g_rng->RollRandomFloatInRange( -0.5f, 0.5f ),
		g_rng->RollRandomFloatInRange( -0.5f, 0.5f ),
		0.0f
	);

	SpawnInfo demonSpawn;
	demonSpawn.m_name = "Demon";
	demonSpawn.m_spawnLocation = m_position + spawnOffset;
	demonSpawn.m_actorOrientation = EulerAngles( g_rng->RollRandomFloatInRange( 0.f, 360.f ), 0.f, 0.f );

	m_map->SpawnActor( demonSpawn );
}

//-----------------------------------------------------------------------------------------------
bool Actor::IsDemonSpawner() const
{
	return m_actorDef && m_actorDef->m_name == "DemonSpawner";
}

//-----------------------------------------------------------------------------------------------
int Actor::GetCurrentRound() const
{
	return m_currentRound;
}

//-----------------------------------------------------------------------------------------------
int Actor::GetDemonsRemainingThisRound() const
{
	return m_demonsToSpawnThisRound - m_demonsSpawnedThisRound;
}

//-----------------------------------------------------------------------------------------------
bool Actor::IsRoundComplete() const
{
	if ( !m_map )
	{
		return false;
	}

	if ( m_demonsSpawnedThisRound < m_demonsToSpawnThisRound )
	{
		return false;
	}

	for ( Actor* actor : m_map->m_actorVector )
	{
		if ( actor && actor->m_actorDef &&
			actor->m_actorDef->m_name == "Demon" &&
			!actor->m_isDead )
		{
			return false;
		}
	}

	return true;
}

//------------------------------------------------------------------------------
void Actor::ApplyPowerUp( const std::string& powerUpName )
{
	const PowerUpDefinition* powerUpDef = PowerUpDefinition::GetByName( powerUpName );
	if ( !powerUpDef )
	{
		return;
	}

	float percentageBoost = g_rng->RollRandomFloatInRange( powerUpDef->m_percentage.m_min, powerUpDef->m_percentage.m_max );
	float multiplier = 1.0f + ( percentageBoost / 100.0f );

	switch ( powerUpDef->m_type )
	{
	    case PowerUpType::SPEED_BOOST:
	    {
		    if ( m_actorDef )
		    {
                m_speedMultiplier *= multiplier;
				g_engine->m_console->AddLine( DevConsole::INFO_MAJOR_COLOR, Stringf( "Speed boost = %.2f", m_speedMultiplier ) );
		    }
		    break;
	    }

	    case PowerUpType::SHOOT_SPEED:
	    {
		    if ( m_weaponRefireTimer )
		    {
			    float newRefireTime = m_weaponDef->m_refireTime / multiplier;
			    delete m_weaponRefireTimer;
			    m_weaponRefireTimer = new Timer( newRefireTime );
				m_weaponRefireTimer->Start();
				g_engine->m_console->AddLine( DevConsole::INFO_MAJOR_COLOR, Stringf( "Shoot speed boost = %.2f", newRefireTime ) );
		    }
		    break;
	    }

	    case PowerUpType::HEALTH_BOOST:
	    {
		    m_health = ( int )( m_health * multiplier );
			g_engine->m_console->AddLine( DevConsole::INFO_MAJOR_COLOR, Stringf( "Health boost = %.2f", multiplier ) );
		    break;
	    }

        case PowerUpType::DAMAGE_BOOST:
        {
			m_damageMultiplier = percentageBoost / 100.0f;
			g_engine->m_console->AddLine( DevConsole::INFO_MAJOR_COLOR, Stringf( "Damage boost = %.2f", m_damageMultiplier ) );
		    break;
        }
    }
}