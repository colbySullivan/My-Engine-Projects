#include "Game/ChessPieceDefinition.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Engine.hpp"

//-----------------------------------------------------------------------------------------------
std::map<std::string, ChessPieceDefinition> ChessPieceDefinition::s_definitions;

//-----------------------------------------------------------------------------------------------
ChessPieceDefinition::~ChessPieceDefinition()
{
	//if ( m_vboPlayerOne )
	//{
	//	delete m_vboPlayerOne;
	//	m_vboPlayerOne = nullptr;
	//}

	//if ( m_vboPlayerTwo )
	//{
	//	delete m_vboPlayerTwo;
	//	m_vboPlayerTwo = nullptr;
	//}

	//delete m_iboPlayerOne;
	//m_iboPlayerOne = nullptr;

	//delete m_iboPlayerTwo;
	//m_iboPlayerTwo = nullptr; // #TODO i belive this is handled by model now
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

		std::string modelOnePath = xml.ParseXmlAttribute( *tileElem, "modelOne", "" );
		std::string modelTwoPath = xml.ParseXmlAttribute( *tileElem, "modelTwo", "" );
		def.m_playerOneDef.m_modelScale = xml.ParseXmlAttribute( *tileElem, "modelOneScale", 1.f );
		def.m_playerTwoDef.m_modelScale = xml.ParseXmlAttribute( *tileElem, "modelTwoScale", 1.f );
		def.m_playerOneDef.m_modelRotationX = xml.ParseXmlAttribute( *tileElem, "modelOneRotationX", 0.f );
		def.m_playerOneDef.m_modelRotationY = xml.ParseXmlAttribute( *tileElem, "modelOneRotationY", 0.f );
		def.m_playerTwoDef.m_modelRotationX = xml.ParseXmlAttribute( *tileElem, "modelTwoRotationX", 0.f );
		def.m_playerTwoDef.m_modelRotationY = xml.ParseXmlAttribute( *tileElem, "modelTwoRotationY", 0.f );
		def.m_playerOneDef.m_modeOffset = xml.ParseXmlAttribute( *tileElem, "modelOneOffset", Vec3::ZERO );
		def.m_playerTwoDef.m_modeOffset = xml.ParseXmlAttribute( *tileElem, "modelTwoOffset", Vec3::ZERO );

		if ( !modelOnePath.empty() )
		{
			def.m_playerOneDef.m_model = g_engine->m_render->CreateOrGetModelFromFile( modelOnePath.c_str() );
			def.m_playerOneDef.m_model->CreateBuffersAndCopy();
		}
		if ( !modelTwoPath.empty() )
		{
			def.m_playerTwoDef.m_model = g_engine->m_render->CreateOrGetModelFromFile( modelTwoPath.c_str() );
			def.m_playerTwoDef.m_model->CreateBuffersAndCopy();
		}

		std::string textureOnePath = xml.ParseXmlAttribute( *tileElem, "textureOne", "" );
		std::string textureTwoPath = xml.ParseXmlAttribute( *tileElem, "textureTwo", "" );
		if ( !textureOnePath.empty() ) def.m_playerOneDef.m_texture = g_engine->m_render->CreateOrGetTextureFromFile( textureOnePath.c_str() );
		if ( !textureTwoPath.empty() ) def.m_playerTwoDef.m_texture = g_engine->m_render->CreateOrGetTextureFromFile( textureTwoPath.c_str() );

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

	XmlElement* tileElemTextures = rootElement->FirstChildElement( "Textures" );

	for ( auto const& name : s_definitions )
	{
		ChessPieceDefinition& def = ChessPieceDefinition::s_definitions[name.first];

		std::string texturePath = xml.ParseXmlAttribute( *tileElemTextures, "texturePlayerOne", "Data/Textures/TestUV.png" );
		if ( !def.m_playerOneDef.m_texture ) def.m_playerOneDef.m_texture = g_engine->m_render->CreateOrGetTextureFromFile(texturePath.c_str());

		std::string normalTexturePath = xml.ParseXmlAttribute( *tileElemTextures, "normal_texture_PlayerOne", "Data/Textures/TestUV.png" );
		def.m_playerTwoDef.m_normalTexture = g_engine->m_render->CreateOrGetTextureFromFile( normalTexturePath.c_str() );

		std::string sgaTexturePath = xml.ParseXmlAttribute( *tileElemTextures, "sga_PlayerOne", "Data/Textures/TestUV.png" );
		def.m_playerTwoDef.m_sgaTexture = g_engine->m_render->CreateOrGetTextureFromFile( sgaTexturePath.c_str() );

		texturePath = xml.ParseXmlAttribute( *tileElemTextures, "texturePlayerTwo", "Data/Textures/TestUV.png" );
		if ( !def.m_playerTwoDef.m_texture ) def.m_playerTwoDef.m_texture= g_engine->m_render->CreateOrGetTextureFromFile( texturePath.c_str() );

		normalTexturePath = xml.ParseXmlAttribute( *tileElemTextures, "normal_texture_PlayerTwo", "Data/Textures/TestUV.png" );
		def.m_playerTwoDef.m_normalTexture = g_engine->m_render->CreateOrGetTextureFromFile( normalTexturePath.c_str() );

		sgaTexturePath = xml.ParseXmlAttribute( *tileElemTextures, "sga_PlayerTwo", "Data/Textures/TestUV.png" );
		def.m_playerTwoDef.m_sgaTexture = g_engine->m_render->CreateOrGetTextureFromFile( sgaTexturePath.c_str() );
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
		m_shader = g_engine->m_render->CreateOrGetShader( m_shaderPath.c_str(), VertexType::VERTEX_PCUTBN );

	// Player One 
	if ( m_playerOneDef.m_model )
	{
		m_playerOneDef.m_vbo = m_playerOneDef.m_model->m_vertexBuffer;
		m_playerOneDef.m_ibo = m_playerOneDef.m_model->m_indexBuffer;
		m_playerOneDef.m_indexCount = ( unsigned int )m_playerOneDef.m_model->m_indexes.size();
	}
	else
	{
		unsigned int vertexBufferSize = ( unsigned int )( m_vertexes.size() * sizeof( Vertex_PCUTBN ) );
		unsigned int indexBufferSize = ( unsigned int )( m_indexes.size() * sizeof( unsigned int ) );
		m_playerOneDef.m_vbo = g_engine->m_render->CreateVertexBuffer( vertexBufferSize, ( unsigned int )sizeof( Vertex_PCUTBN ) );
		g_engine->m_render->CopyCPUToGPU( m_vertexes.data(), vertexBufferSize, m_playerOneDef.m_vbo );
		if ( indexBufferSize > 0 )
		{
			m_playerOneDef.m_ibo = g_engine->m_render->CreateIndexBuffer( indexBufferSize );
			g_engine->m_render->CopyCPUToGPU( m_indexes.data(), indexBufferSize, m_playerOneDef.m_ibo );
			m_playerOneDef.m_indexCount = ( unsigned int )m_indexes.size();
		}
	}

	// Player Two 
	if ( m_playerTwoDef.m_model )
	{
		m_playerTwoDef.m_vbo = m_playerTwoDef.m_model->m_vertexBuffer;
		m_playerTwoDef.m_ibo = m_playerTwoDef.m_model->m_indexBuffer;
		m_playerTwoDef.m_indexCount = ( unsigned int )m_playerTwoDef.m_model->m_indexes.size();
	}
	else
	{
		unsigned int vertexBufferSize = ( unsigned int )( m_vertexes.size() * sizeof( Vertex_PCUTBN ) );
		unsigned int indexBufferSize = ( unsigned int )( m_indexes.size() * sizeof( unsigned int ) );
		m_playerTwoDef.m_vbo = g_engine->m_render->CreateVertexBuffer( vertexBufferSize, ( unsigned int )sizeof( Vertex_PCUTBN ) );
		g_engine->m_render->CopyCPUToGPU( m_vertexes.data(), vertexBufferSize, m_playerTwoDef.m_vbo );
		if ( indexBufferSize > 0 )
		{
			m_playerTwoDef.m_ibo = g_engine->m_render->CreateIndexBuffer( indexBufferSize );
			g_engine->m_render->CopyCPUToGPU( m_indexes.data(), indexBufferSize, m_playerTwoDef.m_ibo );
			m_playerTwoDef.m_indexCount = ( unsigned int )m_indexes.size();
		}
	}
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
	//AddVertsForCylinder3D( m_vertexes, m_indexes, Vec3::ZERO, Vec3( 0.f, 0.f, 0.1f ), 0.25f, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32 );
	//AddVertsForCylinder3D( m_vertexes, m_indexes, Vec3( 0.f, 0.f, 0.1f ), Vec3( 0.f, 0.f, 0.6f ), 0.15f, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32 );
	//AddVertsForSphere3D( m_vertexes, m_indexes, Vec3( 0.f, 0.f, 0.75f ), 0.15f, 32, 32, Rgba8::WHITE );
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