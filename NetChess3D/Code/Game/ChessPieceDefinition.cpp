#include "Game/ChessPieceDefinition.hpp"
#include "Engine/Core/XmlUtils.hpp"

//-----------------------------------------------------------------------------------------------
std::map<std::string, ChessPieceDefinition> ChessPieceDefinition::s_definitions;

//-----------------------------------------------------------------------------------------------
static void LoadPiecesDefsFromFile( char const* filePath )
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