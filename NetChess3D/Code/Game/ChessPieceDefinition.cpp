#include "Game/ChessPieceDefinition.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Engine.hpp"

//-----------------------------------------------------------------------------------------------
std::map<std::string, ChessPieceDefinition> ChessPieceDefinition::s_definitions;

//-----------------------------------------------------------------------------------------------
ChessPieceDefinition::~ChessPieceDefinition()
{
	delete m_vbo;
	m_vbo = nullptr;
	delete m_ibo;
	m_ibo = nullptr;
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
	unsigned int vertexBufferSize = ( unsigned int )( m_vertexes.size() * sizeof( Vertex_PCUTBN ) );
	unsigned int vertexStride = ( unsigned int )sizeof( Vertex_PCUTBN );
	m_vbo = g_engine->m_render->CreateVertexBuffer( vertexBufferSize, vertexStride );

	unsigned int indexBufferSize = ( unsigned int )( m_indexes.size() * sizeof( unsigned int ) );
	if ( indexBufferSize > 0 )
	{
		m_ibo = g_engine->m_render->CreateIndexBuffer( indexBufferSize );
		g_engine->m_render->CopyCPUToGPU( m_indexes.data(), indexBufferSize, m_ibo );
	}

	g_engine->m_render->CopyCPUToGPU( m_vertexes.data(), vertexBufferSize, m_vbo );
}

void ChessPieceDefinition::CreateGeometryForRook()
{
	
	AddVertsForCylinder3D( m_vertexes, Vec3::ZERO, Vec3( 0.f, 0.f, 0.1f ), 0.25f, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32 );
	AddVertsForCylinder3D( m_vertexes, Vec3::ZERO, Vec3( 0.f, 0.f, 1.f ), 0.2f, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32 );
	AddVertsForCylinder3D( m_vertexes, Vec3( 0.f, 0.f, 0.9f ), Vec3( 0.f, 0.f, 1.f ), 0.25f, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32 );
	CreateBuffersAndCopy();
}

void ChessPieceDefinition::CreateGeometryForKnight()
{
	AddVertsForCylinder3D( m_vertexes, Vec3::ZERO, Vec3( 0.f, 0.f, 1.f ), 0.1f, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32 );
	CreateBuffersAndCopy();
}

void ChessPieceDefinition::CreateGeometryForBishop()
{
	AddVertsForCylinder3D( m_vertexes, Vec3::ZERO, Vec3( 0.f, 0.f, 1.f ), 0.2f, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32 );
	AddVertsForSphere3D( m_vertexes, Vec3( 0.f, 0.f, 1.f ), 0.25f, 32, 32, Rgba8::WHITE );
	CreateBuffersAndCopy();
}

void ChessPieceDefinition::CreateGeometryForPawn()
{
	AddVertsForCylinder3D( m_vertexes, Vec3::ZERO, Vec3( 0.f, 0.f, 1.f ), 0.25f, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32 );
	CreateBuffersAndCopy();
}

//-----------------------------------------------------------------------------------------------
void ChessPieceDefinition::CreateGeometryForKing()
{
	AddVertsForCylinder3D( m_vertexes, Vec3::ZERO, Vec3( 0.f, 0.f, 1.f ), 0.2f, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32 );
	AddVertsForCylinder3D( m_vertexes, Vec3( 0.f, 0.f, 0.9f ), Vec3( 0.f, 0.f, 1.f ), 0.25f, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32 );
	//AABB3 topHat = AABB3( Vec3( -0.05f, -0.05f, 1.f ), Vec3( 0.05f, 0.05f, 1.2f ) );
	//AddVertsForAABB3D( m_vertexes, m_indexes, topHat, Rgba8::RED, AABB2::ZERO_TO_ONE ); // #todo In order to use this all shapes need to be indexed
	CreateBuffersAndCopy();
}

//-----------------------------------------------------------------------------------------------
void ChessPieceDefinition::CreateGeometryForQueen()
{
	AddVertsForCylinder3D( m_vertexes, Vec3::ZERO, Vec3( 0.f, 0.f, 1.f ), 0.1f, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32 );
	CreateBuffersAndCopy();
}