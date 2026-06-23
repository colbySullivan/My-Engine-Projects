#include "Engine/Renderer/Model.hpp"
#include "Engine/Core/Engine.hpp"

//------------------------------------------------------------------------------
Model::~Model()
{
	delete m_vertexBuffer;
	delete m_indexBuffer;
	m_vertexBuffer = nullptr;
	m_indexBuffer = nullptr;
}

//------------------------------------------------------------------------------
void Model::Render() const
{
	Texture* testTex = g_engine->m_render->CreateOrGetTextureFromFile( "Data/Textures/TestUV.png" );
	g_engine->m_render->BindTexture( testTex );
	g_engine->m_render->SetModelConstants( Mat44::MakeTranslation3D( m_position ), Rgba8::WHITE );
	g_engine->m_render->SetRasterizerMode( RasterizerMode::SOLID_CULL_BACK );
	g_engine->m_render->DrawIndexBuffer( m_vertexBuffer, m_indexBuffer, (unsigned int)m_indexes.size() );
}

//------------------------------------------------------------------------------
void Model::CreateBuffersAndCopy()
{
	unsigned int vertexBufferSize = ( unsigned int )( m_vertexes.size() * sizeof( Vertex_PCUTBN ) );
	unsigned int vertexStride = ( unsigned int )sizeof( Vertex_PCUTBN );
	m_vertexBuffer = g_engine->m_render->CreateVertexBuffer( vertexBufferSize, vertexStride );

	unsigned int indexBufferSize = ( unsigned int )( m_indexes.size() * sizeof( unsigned int ) );
	m_indexBuffer = g_engine->m_render->CreateIndexBuffer( indexBufferSize );

	g_engine->m_render->CopyCPUToGPU( m_vertexes.data(), vertexBufferSize, m_vertexBuffer );
	g_engine->m_render->CopyCPUToGPU( m_indexes.data(), indexBufferSize, m_indexBuffer );
}