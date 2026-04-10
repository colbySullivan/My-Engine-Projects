#include "Game/AiController.hpp"

AiController::AiController( Map* map, Camera* camera )
	: Controller( map )
	, m_freeFlyCameraPosition( 0.f, 0.f, 0.f )
	, m_freeFlyCameraOrientation( 0.f, 0.f, 0.f )
{

}

AiController::~AiController()
{

}

void AiController::Update( float deltaSeconds )
{

}
