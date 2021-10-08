#include "stdafx.h"
#include "Sky.h"
#include "Camera.h"
#include "Sphere.h"

Sky::Sky(ID3D11Device& device, ID3D11DeviceContext& deviceContext, float skySphereRadius)
	: Object(device, deviceContext)
{
	Sphere sphere(device, deviceContext, skySphereRadius, 30, 30);
}

void Sky::Render(ID3D11DeviceContext& deviceContext)
{
	// center Sky about eye in world space
	XMFLOAT3 eyePos = Camera::GetInstance()->GetPosition();
	XMMATRIX world = XMMatrixTranslation(eyePos.x, eyePos.y, eyePos.z);
	XMMATRIX view = Camera::GetInstance()->GetView();
	XMMATRIX projection = Camera::GetInstance()->GetProjection();

	world = XMMatrixTranspose(world);
	view = XMMatrixTranspose(view);
	projection = XMMatrixTranspose(projection);

	// Set VS constant buffer
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	deviceContext.Map(mObjectBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	ObjectMatrix* objectBufferPtr = reinterpret_cast<ObjectMatrix*>(mappedResource.pData);
	objectBufferPtr->World = world;
	objectBufferPtr->View = view;
	objectBufferPtr->Projection = projection;

	deviceContext.Unmap(mObjectBuffer.Get(), 0);

	unsigned int bufferNumber = 0;
	deviceContext.VSSetConstantBuffers(bufferNumber, 1, mObjectBuffer.GetAddressOf());

	// Set texture.
	deviceContext.PSSetShaderResources(0, 1, mTexture.GetShaderResourceView().GetAddressOf());

	// Input Assembler
	UINT stride = sizeof(XMFLOAT3);
	UINT offset = 0;
	for (unsigned int i = 0; i < mMeshes.size(); ++i)
	{
		deviceContext.IASetVertexBuffers(0, 1, mMeshes[i].GetVertexBuffer().GetAddressOf(), &stride, &offset);
		deviceContext.IASetIndexBuffer(mMeshes[i].GetIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);
		deviceContext.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		deviceContext.DrawIndexed(mMeshes[i].GetNumIndices(), 0, 0);
	}
}