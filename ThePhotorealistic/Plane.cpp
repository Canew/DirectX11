#include "Plane.h"

Plane::Plane(ID3D11Device& device) : Object(device)
{
	// Create vertex buffer
	mVertices =
	{
		{ XMFLOAT3(-1.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(-1.0f, -1.0f) },
		{ XMFLOAT3(-1.0f, 0.0f, +1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(-1.0f, +1.0f) },
		{ XMFLOAT3(+1.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(+1.0f, -1.0f) },
		{ XMFLOAT3(+1.0f, 0.0f, +1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(+1.0f, +1.0f) },
	};

	mVertexCount = static_cast<unsigned int>(mVertices.size());

	D3D11_BUFFER_DESC vbd = {};
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex) * mVertexCount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vinitData = {};
	vinitData.pSysMem = mVertices.data();

	device.CreateBuffer(&vbd, &vinitData, mVertexBuffer.GetAddressOf());

	// Create the index buffer
	mIndices = {
		0, 1, 2,
		1, 3, 2,
	};

	mIndexCount = static_cast<unsigned int>(mIndices.size());

	D3D11_BUFFER_DESC ibd = {};
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * mIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA iinitData = {};
	iinitData.pSysMem = mIndices.data();

	device.CreateBuffer(&ibd, &iinitData, mIndexBuffer.GetAddressOf());
}