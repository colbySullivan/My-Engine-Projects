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

		def.CreateGeometryForKing();

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
void ChessPieceDefinition::CreateGeometryForKing()
{
	AddVertsForCylinder3D( m_vertexes, Vec3::ZERO, Vec3(0.f, 0.f, 1.f), 0.1f, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32 );

	unsigned int vboSize = ( unsigned int )( m_vertexes.size() * sizeof( Vertex_PCUTBN ) );
	unsigned int iboSize = ( unsigned int )( m_vertexes.size() * sizeof( unsigned int ) );

	m_vbo = g_engine->m_render->CreateVertexBuffer( vboSize, sizeof( Vertex_PCUTBN ) );

	g_engine->m_render->CopyCPUToGPU( m_vertexes.data(), vboSize, m_vbo );
}