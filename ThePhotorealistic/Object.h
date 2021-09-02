#pragma once

#include "D3DUtil.h"
#include <vector>

class Object
{
protected:
	struct Vertex
	{
		XMFLOAT3 Position;
		XMFLOAT4 Color;
	};
	struct ObjectBuffer
	{
		XMMATRIX World;
		XMMATRIX View;
		XMMATRIX Projection;
	};

public:
	virtual void Init(ID3D11Device& device);
	virtual void Render(ID3D11DeviceContext& deviceContext);

	virtual void SetPosition(float x, float y, float z);
	virtual void SetPosition(XMFLOAT3 position);
	virtual void SetScale(float x, float y, float z);
	virtual void SetScale(XMFLOAT3 scale);

protected:
	virtual void BuildGeometryBuffers(ID3D11Device& device) = 0;

protected:
	ComPtr<ID3D11Buffer> mObjectBuffer = nullptr;

	std::vector<Vertex> mVertices;
	std::vector<unsigned long> mIndices;
	ComPtr<ID3D11Buffer> mVertexBuffer = nullptr;
	ComPtr<ID3D11Buffer> mIndexBuffer = nullptr;
	unsigned int mVertexCount = 0;
	unsigned int mIndexCount = 0;

	XMFLOAT3 mPosition = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 mRotation = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 mScale = { 1.0f, 1.0f, 1.0f };
};
