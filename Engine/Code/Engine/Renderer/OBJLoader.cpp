#define TINYOBJLOADER_IMPLEMENTATION
#include "ThirdParty/TinyOBJLoader/tiny_obj_loader.h"

#include "OBJLoader.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

//------------------------------------------------------------------------------
OBJModel* LoadOBJFromFile( const char* filePath, Renderer* renderer )
{
	std::string basePath = filePath;
	basePath = basePath.substr( 0, basePath.find_last_of( "/\\" ) + 1 );

	tinyobj::attrib_t                attrib;
	std::vector<tinyobj::shape_t>    shapes;
	std::vector<tinyobj::material_t> materials;
	std::string                      err;

	bool loaded = tinyobj::LoadObj( &attrib, &shapes, &materials, &err, filePath, basePath.c_str(), true );

	if ( !loaded )
	{
		ERROR_AND_DIE( "LoadOBJFromFile: tinyobj failed to load file." );
	}

	std::vector<Vertex>   verts;
	std::vector<uint32_t> indices;

	for ( tinyobj::shape_t const& shape : shapes )
	{
		for ( tinyobj::index_t const& idx : shape.mesh.indices )
		{
			Vertex v;

			v.m_position.x = attrib.vertices[3 * idx.vertex_index + 0];
			v.m_position.y = attrib.vertices[3 * idx.vertex_index + 1];
			v.m_position.z = attrib.vertices[3 * idx.vertex_index + 2];

			if ( idx.texcoord_index >= 0 )
			{
				v.m_uvTexCoords.x = attrib.texcoords[2 * idx.texcoord_index + 0];
				v.m_uvTexCoords.y = 1.f - attrib.texcoords[2 * idx.texcoord_index + 1];
			}

			v.m_color = Rgba8( 255, 255, 255, 255 );

			indices.push_back( ( uint32_t )verts.size() );
			verts.push_back( v );
		}
	}

	unsigned int vboSize = ( unsigned int )( verts.size() * sizeof( Vertex ) );
	unsigned int iboSize = ( unsigned int )( indices.size() * sizeof( uint32_t ) );

	VertexBuffer* vbo = renderer->CreateVertexBuffer( vboSize, sizeof( Vertex ) );
	IndexBuffer* ibo = renderer->CreateIndexBuffer( iboSize );

	renderer->CopyCPUToGPU( verts.data(), vboSize, vbo );
	renderer->CopyCPUToGPU( indices.data(), iboSize, ibo );

	OBJModel* model = new OBJModel();
	model->m_vbo = vbo;
	model->m_ibo = ibo;
	model->m_indexCount = ( unsigned int )indices.size();

	return model;
}