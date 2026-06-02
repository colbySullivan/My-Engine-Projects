#include "Game/ChessBoard.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/VertexUtils.hpp"

//-----------------------------------------------------------------------------------------------
ChessBoard::ChessBoard()
{
	CreateBoardGeometry();
	CreateBuffersAndCopy();
}

//-----------------------------------------------------------------------------------------------
ChessBoard::~ChessBoard()
{
	delete m_vbo;
	m_vbo = nullptr;
	delete m_ibo;
	m_ibo = nullptr;

	m_vertexes.clear();
}

//-----------------------------------------------------------------------------------------------
void ChessBoard::Update()
{

}

//-----------------------------------------------------------------------------------------------
void ChessBoard::Render() const
{
	Mat44 modelMatrix = Mat44::MakeTranslation3D( m_position );
	g_engine->m_render->SetModelConstants( modelMatrix, m_color );
	g_engine->m_render->m_desiredBlendMode = BlendMode::OPAQUE;
	g_engine->m_render->m_desiredRasterizerMode = RasterizerMode::SOLID_CULL_BACK;
	unsigned int indexCount = ( unsigned int )m_indexes.size();
	g_engine->m_render->DrawIndexBuffer( m_vbo, m_ibo, indexCount );
}

//-----------------------------------------------------------------------------------------------
void ChessBoard::CreateBuffersAndCopy()
{
	unsigned int vertexBufferSize = ( unsigned int )( m_vertexes.size() * sizeof( Vertex_PCUTBN ) );
	unsigned int vertexStride = ( unsigned int )sizeof( Vertex_PCUTBN );
	m_vbo = g_engine->m_render->CreateVertexBuffer( vertexBufferSize, vertexStride );

	unsigned int indexBufferSize = ( unsigned int )( m_indexes.size() * sizeof( unsigned int ) );
	m_ibo = g_engine->m_render->CreateIndexBuffer( indexBufferSize );

	g_engine->m_render->CopyCPUToGPU( m_vertexes.data(), vertexBufferSize, m_vbo );
	g_engine->m_render->CopyCPUToGPU( m_indexes.data(), indexBufferSize, m_ibo );
}

//-----------------------------------------------------------------------------------------------
void ChessBoard::CreateBoardGeometry()
{
	Vec3 bl = Vec3( 0.f, 0.f, 0.f );
	Vec3 br = Vec3( 1.f, 0.f, 0.f );
	Vec3 tr = Vec3( 1.f, 1.f, 0.f );
	Vec3 tl = Vec3( 0.f, 1.f, 0.f );
	for (int row = 0; row < 8 ; ++row)
	{
		for ( int column = 0; column < 8; ++column )
		{
			Rgba8 color = Rgba8( 120, 120, 255, 255 );
			if ( ( row + column ) % 2 == 0 )
			{
				color = Rgba8( 120, 255, 120, 120 );
			}
			bl = Vec3( 0.f + row, 0.f + column, 0.f );
			br = Vec3( 1.f + row, 0.f + column, 0.f );
			tr = Vec3( 1.f + row, 1.f + column, 0.f );
			tl = Vec3( 0.f + row, 1.f + column, 0.f );
			AddVertsForQuad3D( m_vertexes, m_indexes, bl, br, tr, tl, color, AABB2::ZERO_TO_ONE );
		}
	}
}