#pragma once
#include "Engine/Core/Vertex_PCUTBN.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
#include <vector>

class VertexBuffer;
class IndexBuffer;

class Model {
	friend class Renderer;
public:
	~Model();
	void Render() const;

	Vec3 m_position = Vec3( 0.f, 0.f, 0.f );
	std::vector<Vertex_PCUTBN> m_vertexes;
	std::vector<unsigned int>  m_indexes;

private:
	Model() = default;
	void CreateBuffersAndCopy();

	VertexBuffer* m_vertexBuffer = nullptr;
	IndexBuffer* m_indexBuffer = nullptr;
	std::string   m_filepath;
};