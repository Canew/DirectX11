#pragma once

#include "D3DUtil.h"
#include "Vertex.h"

class Mesh
{
public:
	Mesh(ID3D11Device& device, ID3D11DeviceContext& deviceContext, std::vector<Vertex>& vertices, std::vector<unsigned int> indices);
	Mesh(const Mesh& mesh);
	void Render();

	ComPtr<ID3D11Buffer> GetVertexBuffer() { return mVertexBuffer; }
	ComPtr<ID3D11Buffer> GetIndexBuffer() { return mIndexBuffer; }
	unsigned int GetNumVertices() { return mNumVertices; }
	unsigned int GetNumIndices() { return mNumIndices; }

private:
	ComPtr<ID3D11Buffer> mVertexBuffer = nullptr;
	ComPtr<ID3D11Buffer> mIndexBuffer = nullptr;
	unsigned int mNumVertices = 0;
	unsigned int mNumIndices = 0;

	ID3D11DeviceContext& mDeviceContext;
};

