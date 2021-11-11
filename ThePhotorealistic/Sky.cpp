#include "stdafx.h"
#include "Sky.h"
#include "Camera.h"
#include "Sphere.h"

Sky::Sky(ID3D11Device& device, ID3D11DeviceContext& deviceContext, float skySphereRadius)
	: Object(device, deviceContext)
{
	unsigned int sliceCount = 30;
	unsigned int stackCount = 30;
	std::vector<XMFLOAT3> vertices;
	std::vector<unsigned int> indices;

	//
	// Compute the vertices stating at the top pole and moving down the stacks.
	//

	// Poles: note that there will be texture coordinate distortion as there is
	// not a unique point on the texture map to assign to the pole when mapping
	// a rectangular texture onto a sphere.
	XMFLOAT3 topVertex(0.0f, +skySphereRadius, 0.0f);
	XMFLOAT3 bottomVertex(0.0f, -skySphereRadius, 0.0f);

	vertices.push_back(topVertex);

	float phiStep = XM_PI / stackCount;
	float thetaStep = 2.0f * XM_PI / sliceCount;

	// Compute vertices for each stack ring (do not count the poles as rings).
	for (UINT i = 1; i <= stackCount - 1; ++i)
	{
		float phi = i * phiStep;

		// Vertices of ring.
		for (UINT j = 0; j <= sliceCount; ++j)
		{
			float theta = j * thetaStep;

			XMFLOAT3 v;

			// spherical to cartesian
			v.x = skySphereRadius * sinf(phi) * cosf(theta);
			v.y = skySphereRadius * cosf(phi);
			v.z = skySphereRadius * sinf(phi) * sinf(theta);

			vertices.push_back(v);
		}
	}

	vertices.push_back(bottomVertex);

	//
	// Compute indices for top stack.  The top stack was written first to the vertex buffer
	// and connects the top pole to the first ring.
	//

	for (UINT i = 1; i <= sliceCount; ++i)
	{
		indices.push_back(0);
		indices.push_back(i + 1);
		indices.push_back(i);
	}

	//
	// Compute indices for inner stacks (not connected to poles).
	//

	// Offset the indices to the index of the first vertex in the first ring.
	// This is just skipping the top pole vertex.
	UINT baseIndex = 1;
	UINT ringVerTexCoordount = sliceCount + 1;
	for (UINT i = 0; i < stackCount - 2; ++i)
	{
		for (UINT j = 0; j < sliceCount; ++j)
		{
			indices.push_back(baseIndex + i * ringVerTexCoordount + j);
			indices.push_back(baseIndex + i * ringVerTexCoordount + j + 1);
			indices.push_back(baseIndex + (i + 1) * ringVerTexCoordount + j);

			indices.push_back(baseIndex + (i + 1) * ringVerTexCoordount + j);
			indices.push_back(baseIndex + i * ringVerTexCoordount + j + 1);
			indices.push_back(baseIndex + (i + 1) * ringVerTexCoordount + j + 1);
		}
	}

	//
	// Compute indices for bottom stack.  The bottom stack was written last to the vertex buffer
	// and connects the bottom pole to the bottom ring.
	//

	// South pole vertex was added last.
	UINT southPoleIndex = (UINT)vertices.size() - 1;

	// Offset the indices to the index of the first vertex in the last ring.
	baseIndex = southPoleIndex - ringVerTexCoordount;

	for (UINT i = 0; i < sliceCount; ++i)
	{
		indices.push_back(southPoleIndex);
		indices.push_back(baseIndex + i);
		indices.push_back(baseIndex + i + 1);
	}

	mVertices = vertices;
	mIndices = indices;

	// Create the vertex buffer
	D3D11_BUFFER_DESC vbd = {};
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(XMFLOAT3) * mVertices.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vinitData = {};
	vinitData.pSysMem = mVertices.data();

	device.CreateBuffer(&vbd, &vinitData, mVertexBuffer.GetAddressOf());

	// Create the index buffer
	D3D11_BUFFER_DESC ibd = {};
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(unsigned int) * mIndices.size();
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA iinitData = {};
	iinitData.pSysMem = mIndices.data();

	device.CreateBuffer(&ibd, &iinitData, mIndexBuffer.GetAddressOf());
}

void Sky::Render(ID3D11DeviceContext& deviceContext)
{
	// Set shader before render.
	GetShaderClass()->SetShader(deviceContext);

	// center Sky about eye in world space
	XMFLOAT3 eyePos = Camera::GetInstance()->GetPosition();
	XMMATRIX world = XMMatrixTranslation(eyePos.x, eyePos.y, eyePos.z);
	XMMATRIX view = Camera::GetInstance()->GetView();
	XMMATRIX projection = Camera::GetInstance()->GetProjection();

	world = XMMatrixTranspose(world);
	view = XMMatrixTranspose(view);
	projection = XMMatrixTranspose(projection);

	// Set VS constant buffer
	// Per Object
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	deviceContext.Map(mObjectBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	ObjectMatrix* objectBufferPtr = reinterpret_cast<ObjectMatrix*>(mappedResource.pData);
	objectBufferPtr->World = world;

	deviceContext.Unmap(mObjectBuffer.Get(), 0);

	unsigned int bufferNumber = 0;
	deviceContext.VSSetConstantBuffers(bufferNumber, 1, mObjectBuffer.GetAddressOf());

	// Set texture.
	deviceContext.PSSetShaderResources(0, 1, mDiffuseMap->GetShaderResourceView().GetAddressOf());

	// Render sky.
	unsigned int stride = sizeof(XMFLOAT3);
	unsigned int offset = 0;
	mDeviceContext.IASetVertexBuffers(0, 1, mVertexBuffer.GetAddressOf(), &stride, &offset);
	mDeviceContext.IASetIndexBuffer(mIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	mDeviceContext.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Draw
	mDeviceContext.DrawIndexed(mIndices.size(), 0, 0);
}