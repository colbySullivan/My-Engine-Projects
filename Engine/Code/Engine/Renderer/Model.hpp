#pragma once
#include "Engine/Core/Vertex_PCUTBN.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
#include <vector>

class VertexBuffer;
class IndexBuffer;

class Model 
{
	friend class Renderer;
public:
	~Model();
	void Render() const;

	Vec3 m_position = Vec3( 0.f, 0.f, 0.f );
	float m_scale = 1.f;
	Texture* m_texture = nullptr;
	std::vector<Vertex_PCUTBN> m_vertexes;
	std::vector<unsigned int>  m_indexes;
	VertexBuffer* m_vertexBuffer = nullptr;
	IndexBuffer* m_indexBuffer = nullptr;
	void CreateBuffersAndCopy();
private:
	Model() = default;


	std::string   m_filepath;
};