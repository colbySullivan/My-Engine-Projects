#include "Game/ChessPieceDefinition.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Engine.hpp"

//-----------------------------------------------------------------------------------------------
std::map<std::string, ChessPieceDefinition> ChessPieceDefinition::s_definitions;

//-----------------------------------------------------------------------------------------------
ChessPieceDefinition::~ChessPieceDefinition()
{
	delete m_vboPlayerOne;
	m_vboPlayerOne = nullptr;
	delete m_iboPlayerOne;
	m_iboPlayerOne = nullptr;

	delete m_vboPlayerTwo;
	m_vboPlayerTwo = nullptr;
	delete m_iboPlayerTwo;
	m_iboPlayerTwo = nullptr;
}

//-----------------------------------------------------------------------------------------------
void ChessPieceDefinition::LoadPiecesDefsFromFile( char const* filePath )
{
	XmlDocument doc;
	doc.LoadFile( filePath );

	XmlElement* rootElement = doc.RootElement();
	if ( !rootElement )
	{
		return;
	}

	XmlElement* tileElem = rootElement->FirstChildElement( "ChessPieceDefinition" );
	XmlUtils xml;

	while ( tileElem )
	{
		std::string name = xml.ParseXmlAttribute( *tileElem, "name", std::string( "Unnamed" ) );
		ChessPieceDefinition& def = ChessPieceDefinition::s_definitions[name];
		def.m_name = name;
		def.m_symbol = xml.ParseXmlAttribute( *tileElem, "symbol", ' ' );
		def.m_type = def.GetTypeFromString( name );
		def.m_shaderPath = xml.ParseXmlAttribute( *tileElem, "shader", "default");

		switch ( def.m_type )
		{
		case King:
			def.CreateGeometryForKing();
			break;
		case Queen:
			def.CreateGeometryForQueen();
			break;
		case Rook:
			def.CreateGeometryForRook();
			break;
		case Knight:
			def.CreateGeometryForKnight();
			break;
		case Bishop:
			def.CreateGeometryForBishop();
			break;
		case Pawn:
			def.CreateGeometryForPawn();
			break;
		default:
			break;
		}

		tileElem = tileElem->NextSiblingElement( "ChessPieceDefinition" );
	}
}

//-----------------------------------------------------------------------------------------------
void ChessPieceDefinition::InitializeChessPieceDefs()
{
	s_definitions.clear();
	LoadPiecesDefsFromFile( "Data/Definitions/ChessPieceDefinitions.xml" );
}

//-----------------------------------------------------------------------------------------------
const ChessPieceDefinition* ChessPieceDefinition::GetByName( const std::string& name )
{
	std::map<std::string, ChessPieceDefinition>::iterator it = s_definitions.find( name );
	if ( it == s_definitions.end() )
	{
		return nullptr;
	}

	const ChessPieceDefinition& foundDefinition = it->second;
	return &foundDefinition;
}

//-----------------------------------------------------------------------------------------------
ChessPieceType ChessPieceDefinition::GetTypeFromString( const std::string& typeName )
{
	if ( typeName == "Pawn" )   return ChessPieceType::Pawn;
	if ( typeName == "Rook" )   return ChessPieceType::Rook;
	if ( typeName == "Knight" ) return ChessPieceType::Knight;
	if ( typeName == "Bishop" ) return ChessPieceType::Bishop;
	if ( typeName == "Queen" )  return ChessPieceType::Queen;
	if ( typeName == "King" )   return ChessPieceType::King;

	return ChessPieceType::Count;
}

//-----------------------------------------------------------------------------------------------
void ChessPieceDefinition::CreateBuffersAndCopy()
{
	if ( m_shaderPath != "default" )
	{
		m_shader = g_engine->m_render->CreateOrGetShader( m_shaderPath.c_str(), VertexType::VERTEX_PCUTBN );
	}

	unsigned int vertexBufferSize = ( unsigned int )( m_vertexes.size() * sizeof( Vertex_PCUTBN ) );
	unsigned int vertexStride = ( unsigned int )sizeof( Vertex_PCUTBN );
	m_vboPlayerOne = g_engine->m_render->CreateVertexBuffer( vertexBufferSize, vertexStride );
	m_vboPlayerTwo = g_engine->m_render->CreateVertexBuffer( vertexBufferSize, vertexStride );

	unsigned int indexBufferSize = ( unsigned int )( m_indexes.size() * sizeof( unsigned int ) );
	if ( indexBufferSize > 0 )
	{
		m_iboPlayerOne = g_engine->m_render->CreateIndexBuffer( indexBufferSize );
		m_iboPlayerTwo = g_engine->m_render->CreateIndexBuffer( indexBufferSize );
		g_engine->m_render->CopyCPUToGPU( m_indexes.data(), indexBufferSize, m_iboPlayerOne );
		g_engine->m_render->CopyCPUToGPU( m_indexes.data(), indexBufferSize, m_iboPlayerTwo );
	}

	g_engine->m_render->CopyCPUToGPU( m_vertexes.data(), vertexBufferSize, m_vboPlayerOne );
	g_engine->m_render->CopyCPUToGPU( m_vertexes.data(), vertexBufferSize, m_vboPlayerTwo );
}

void ChessPieceDefinition::CreateGeometryForRook()
{
	AddVertsForCylinder3D( m_vertexes, m_indexes, Vec3::ZERO, Vec3( 0.f, 0.f, 0.15f ), 0.3f, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32 );
	AddVertsForCylinder3D( m_vertexes, m_indexes, Vec3( 0.f, 0.f, 0.15f ), Vec3( 0.f, 0.f, 0.8f ), 0.2f, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32 );
	AddVertsForCylinder3D( m_vertexes, m_indexes, Vec3( 0.f, 0.f, 0.8f ), Vec3( 0.f, 0.f, 0.9f ), 0.25f, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32 );
	float topHatHeight = 1.05f;
	AABB3 box1 = AABB3( Vec3( 0.15f, -0.05f, 0.9f ), Vec3( 0.25f, 0.05f, topHatHeight ) );
	AABB3 box2 = AABB3( Vec3( -0.25f, -0.05f, 0.9f ), Vec3( -0.15f, 0.05f, topHatHeight ) );
	AABB3 box3 = AABB3( Vec3( -0.05f, 0.15f, 0.9f ), Vec3( 0.05f, 0.25f, topHatHeight ) );
	AABB3 box4 = AABB3( Vec3( -0.05f, -0.25f, 0.9f ), Vec3( 0.05f, -0.15f, topHatHeight ) );
	AddVertsForAABB3D( m_vertexes, m_indexes, box1, Rgba8::WHITE, AABB2::ZERO_TO_ONE );
	AddVertsForAABB3D( m_vertexes, m_indexes, box2, Rgba8::WHITE, AABB2::ZERO_TO_ONE );
	AddVertsForAABB3D( m_vertexes, m_indexes, box3, Rgba8::WHITE, AABB2::ZERO_TO_ONE );
	AddVertsForAABB3D( m_vertexes, m_indexes, box4, Rgba8::WHITE, AABB2::ZERO_TO_ONE );
	CreateBuffersAndCopy();
}

void ChessPieceDefinition::CreateGeometryForKnight()
{
	AddVertsForCylinder3D( m_vertexes, m_indexes, Vec3::ZERO, Vec3( 0.f, 0.f, 0.15f ), 0.25f, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32 );
	AddVertsForCylinder3D( m_vertexes, m_indexes, Vec3( 0.f, 0.f, 0.15f ), Vec3( 0.f, 0.f, 0.7f ), 0.12f, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32 );
	AABB3 head = AABB3( Vec3( -0.08f, -0.15f, 0.6f ), Vec3( 0.08f, 0.2f, 0.9f ) );
	AddVertsForAABB3D( m_vertexes, m_indexes, head, Rgba8::WHITE, AABB2::ZERO_TO_ONE );
	AABB3 ears = AABB3( Vec3( -0.05f, 0.05f, 0.85f ), Vec3( 0.05f, 0.15f, 0.95f ) );
	AddVertsForAABB3D( m_vertexes, m_indexes, ears, Rgba8::WHITE, AABB2::ZERO_TO_ONE );
	CreateBuffersAndCopy();
}

void ChessPieceDefinition::CreateGeometryForBishop()
{
	AddVertsForCylinder3D( m_vertexes, m_indexes, Vec3::ZERO, Vec3( 0.f, 0.f, 0.15f ), 0.25f, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32 );
	AddVertsForCylinder3D( m_vertexes, m_indexes, Vec3( 0.f, 0.f, 0.15f ), Vec3( 0.f, 0.f, 0.5f ), 0.18f, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32 );
	AddVertsForCylinder3D( m_vertexes, m_indexes, Vec3( 0.f, 0.f, 0.5f ), Vec3( 0.f, 0.f, 0.8f ), 0.15f, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 64 );
	AddVertsForSphere3D( m_vertexes, m_indexes, Vec3( 0.f, 0.f, 0.95f ), 0.18f, 16, 16, Rgba8::WHITE );
	AABB3 slit = AABB3( Vec3( -0.03f, -0.2f, 0.9f ), Vec3( 0.03f, 0.2f, 0.95f ) );
	AddVertsForAABB3D( m_vertexes, m_indexes, slit, Rgba8::WHITE, AABB2::ZERO_TO_ONE );
	CreateBuffersAndCopy();
}

void ChessPieceDefinition::CreateGeometryForPawn()
{
	AddVertsForCylinder3D( m_vertexes, m_indexes, Vec3::ZERO, Vec3( 0.f, 0.f, 0.1f ), 0.25f, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32 );
	AddVertsForCylinder3D( m_vertexes, m_indexes, Vec3( 0.f, 0.f, 0.1f ), Vec3( 0.f, 0.f, 0.6f ), 0.15f, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32 );
	AddVertsForSphere3D( m_vertexes, m_indexes, Vec3( 0.f, 0.f, 0.75f ), 0.15f, 32, 32, Rgba8::WHITE );
	CreateBuffersAndCopy();
}

//-----------------------------------------------------------------------------------------------
void ChessPieceDefinition::CreateGeometryForKing()
{
	AddVertsForCylinder3D( m_vertexes, m_indexes, Vec3::ZERO, Vec3( 0.f, 0.f, 0.15f ), 0.3f, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32 );
	AddVertsForCylinder3D( m_vertexes, m_indexes, Vec3( 0.f, 0.f, 0.15f ), Vec3( 0.f, 0.f, 0.7f ), 0.2f, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32 );
	AddVertsForCylinder3D( m_vertexes, m_indexes, Vec3( 0.f, 0.f, 0.7f ), Vec3( 0.f, 0.f, 0.8f ), 0.22f, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32 );
	AddVertsForCylinder3D( m_vertexes, m_indexes, Vec3( 0.f, 0.f, 0.8f ), Vec3( 0.f, 0.f, 1.0f ), 0.18f, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32 );
	AddVertsForSphere3D( m_vertexes, m_indexes, Vec3( 0.f, 0.f, 1.1f ), 0.15f, 16, 16, Rgba8::WHITE );
	AABB3 crossVertical = AABB3( Vec3( -0.03f, -0.03f, 1.15f ), Vec3( 0.03f, 0.03f, 1.35f ) );
	AddVertsForAABB3D( m_vertexes, m_indexes, crossVertical, Rgba8::WHITE, AABB2::ZERO_TO_ONE );
	AABB3 crossHorizontal = AABB3( Vec3( -0.12f, -0.03f, 1.22f ), Vec3( 0.12f, 0.03f, 1.28f ) );
	AddVertsForAABB3D( m_vertexes, m_indexes, crossHorizontal, Rgba8::WHITE, AABB2::ZERO_TO_ONE );
	CreateBuffersAndCopy();
}

//-----------------------------------------------------------------------------------------------
void ChessPieceDefinition::CreateGeometryForQueen()
{
	AddVertsForCylinder3D( m_vertexes, m_indexes, Vec3::ZERO, Vec3( 0.f, 0.f, 0.15f ), 0.3f, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32 );
	AddVertsForCylinder3D( m_vertexes, m_indexes, Vec3( 0.f, 0.f, 0.15f ), Vec3( 0.f, 0.f, 0.7f ), 0.2f, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32 );
	AddVertsForCylinder3D( m_vertexes, m_indexes, Vec3( 0.f, 0.f, 0.7f ), Vec3( 0.f, 0.f, 0.85f ), 0.23f, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32 );
	AddVertsForCylinder3D( m_vertexes, m_indexes, Vec3( 0.f, 0.f, 0.85f ), Vec3( 0.f, 0.f, 1.0f ), 0.15f, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32 );
	float crownRadius = 0.2f;
	float crownHeight = 1.0f;
	int numPoints = 5;
	for ( int i = 0; i < numPoints; ++i )
	{
		float angle = ( 360.f / numPoints ) * i;
		float x = crownRadius * CosDegrees( angle );
		float y = crownRadius * SinDegrees( angle );
		AddVertsForSphere3D( m_vertexes, m_indexes, Vec3( x, y, crownHeight ), 0.08f, 12, 12, Rgba8::WHITE );
	}

	AddVertsForSphere3D( m_vertexes, m_indexes, Vec3( 0.f, 0.f, 1.15f ), 0.1f, 12, 12, Rgba8::WHITE );
	CreateBuffersAndCopy();
}