#include "Engine/Renderer/OBJLoader.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Vertex_PCUTBN.hpp"
#include "Engine/Renderer/Model.hpp"
#include <unordered_map>

//------------------------------------------------------------------------------
void OBJLoader::Load( std::string const& filepath, Model& model )
{
	// Read and save file contents
	std::string fileContent;
	FileReadToString( fileContent, filepath );

	std::vector<Vec3> positions;
	std::vector<Vec2> uvs;
	std::vector<Vec3> normals;
	std::vector<Vertex_PCUTBN> vertices;
	std::vector<unsigned int> indices;
	std::unordered_map<std::string, int> savedVertices;
	int indexCount = 0;

	// Get file by line
	Strings lines;
	lines = SplitStringOnDelimiter( fileContent, '\n' );
	for ( std::string line : lines )
	{
		if ( !line.empty() )
		{
			// Windows sometimes has this in the last line
			if ( line.back() == '\r' ) line.pop_back();
			Strings words;
			words = SplitStringOnDelimiter( line, ' ' );

			// Position vertices
			if ( words[0] == "v" )
			{
				std::vector<float> xyzFloats;
				for (int wordIndex = 1; wordIndex < words.size() ; wordIndex++)
				{
					std::string word = words[wordIndex];
					// Tabs and spaces can exist in between vertices
					if ( !word.empty() )
					{
						xyzFloats.push_back( std::stof( word ) );
					}
				}

				// Insure v line was only 3 floats (xyz)
				if ( xyzFloats.size() == 3 )
				{
					positions.push_back( Vec3( xyzFloats[0], xyzFloats[1], xyzFloats[2] ) );
				}
			}

			// Texture coordinates
			if ( words[0] == "vt" )
			{
				std::vector<float> uvFloats;
				for (int wordIndex = 1; wordIndex < words.size() ; wordIndex++)
				{
					std::string word = words[wordIndex];
					// Tabs and spaces can exist in between vertices
					if ( !word.empty() )
					{
						uvFloats.push_back( std::stof( word ) );
					}
				}

				// Insure vt line was only 2 floats (uv)
				if ( uvFloats.size() == 3 )
				{
					// Reflect V across the middle
					uvs.push_back( Vec2( uvFloats[0], 1.0f - uvFloats[1] ) );
				}
			}

			// Normals
			if ( words[0] == "vn" )
			{
				std::vector<float> normalFloats;
				for ( int wordIndex = 1; wordIndex < words.size(); wordIndex++ )
				{
					std::string word = words[wordIndex];
					// Tabs and spaces can exist in between vertices
					if ( !word.empty() )
					{
						normalFloats.push_back( std::stof( word ) );
					}
				}

				// Insure v line was only 3 floats (xyz)
				if ( normalFloats.size() == 3 )
				{
					normals.push_back( Vec3( normalFloats[0], normalFloats[1], normalFloats[2] ) );
				}
			}

			// Create Vertices
			if ( words[0] == "f" )
			{
				Strings verticesSplitOnSpace;
				verticesSplitOnSpace = SplitStringOnDelimiter( line, ' ' );
				for (int vertexIndex = 1; vertexIndex < verticesSplitOnSpace.size() ; vertexIndex++)
				{
					std::string vertCouple = verticesSplitOnSpace[vertexIndex];
					if ( !vertCouple.empty() )
					{
						Strings posTanNormal;
						posTanNormal = SplitStringOnDelimiter( vertCouple, '/' );

						// OBJ index starts at 1
						int pi = std::stoi( posTanNormal[0] ) - 1;
						int ui = std::stoi( posTanNormal[1] ) - 1;
						int ni = std::stoi( posTanNormal[2] ) - 1;

						// Find duplicate vertices (vertCouple)
						auto it = savedVertices.find( vertCouple );
						if ( it == savedVertices.end() )
						{
							vertices.push_back( Vertex_PCUTBN( positions[pi], Rgba8::WHITE, uvs[ui], Vec3(), Vec3(), normals[ni] ) );
							savedVertices.insert( { vertCouple, indexCount } );
							indices.push_back( indexCount );
							indexCount++;
						}

						// Vertex duplicate
						else
						{
							indices.push_back( it->second );
						}
					}
				}
			}
		}
	}

	model.m_vertexes = vertices;
	model.m_indexes = indices;
}	
