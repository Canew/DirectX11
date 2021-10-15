#include "stdafx.h"
#include "Plane.h"

Plane::Plane(ID3D11Device& device, ID3D11DeviceContext& deviceContext) : Object(device, deviceContext)
{
	// Create vertex buffer
	std::vector<Vertex> vertices =
	{
		{ XMFLOAT3(-1.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(-1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 0.0f, +1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(-1.0f, +1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(+1.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(+1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(+1.0f, 0.0f, +1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(+1.0f, +1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
	};

	// Create the index buffer
	std::vector<unsigned int> indices =
	{
		0, 1, 2,
		1, 3, 2,
	};

	mMeshes.push_back(Mesh(device, deviceContext, vertices, indices));
}