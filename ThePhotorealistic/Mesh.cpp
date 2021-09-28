#include "stdafx.h"
#include "Mesh.h"

Mesh::Mesh(ID3D11Device& device, ID3D11DeviceContext& deviceContext, std::vector<Vertex>& vertices, std::vector<unsigned int> indices)
	: mDeviceContext(deviceContext)
{
	// Create the vertex buffer
	mNumVertices = vertices.size();

	D3D11_BUFFER_DESC vbd = {};
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex) * vertices.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vinitData = {};
	vinitData.pSysMem = vertices.data();

	device.CreateBuffer(&vbd, &vinitData, mVertexBuffer.GetAddressOf());

	// Create the index buffer
	mNumIndices = indices.size();

	D3D11_BUFFER_DESC ibd = {};
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(unsigned int) * indices.size();
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA iinitData = {};
	iinitData.pSysMem = indices.data();

	device.CreateBuffer(&ibd, &iinitData, mIndexBuffer.GetAddressOf());
}

Mesh::Mesh(const Mesh& mesh) 
	: mDeviceContext(mesh.mDeviceContext)
{
	mVertexBuffer = mesh.mVertexBuffer;
	mIndexBuffer = mesh.mIndexBuffer;
	mNumVertices = mesh.mNumVertices;
	mNumIndices = mesh.mNumIndices;
}

void Mesh::Render()
{
	unsigned int stride = sizeof(Vertex);
	unsigned int offset = 0;
	mDeviceContext.IASetVertexBuffers(0, 1, GetVertexBuffer().GetAddressOf(), &stride, &offset);
	mDeviceContext.IASetIndexBuffer(GetIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);
	mDeviceContext.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Draw
	mDeviceContext.DrawIndexed(GetNumIndices(), 0, 0);
}
