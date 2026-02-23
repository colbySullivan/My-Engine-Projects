#include "Entity.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Math/Mat44.hpp"


//-----------------------------------------------------------------------------------------------
Entity::Entity( Game* owner )
{
	m_game = owner;
}

//-----------------------------------------------------------------------------------------------
Entity::~Entity()
{

}

//------------------------------------------------------------------------------
Mat44 Entity::GetModelToWorldTransform() const
{
	Mat44 model = m_orientation.GetAsMatrix_IFwd_JLeft_KUp();
	model.SetTranslation3D( m_position );
	return model;
}
