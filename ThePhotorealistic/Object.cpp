#include "Camera.h"
#include "Object.h"

Object::Object(ID3D11Device& device) : mTexture(device), mMaterial(XMFLOAT4(0.5f, 0.5f, 0.5f, 0.1f), XMFLOAT3(0.05f, 0.05f, 0.05f), 0.1f)
{
	// Build Matrix Constant Buffers
	D3D11_BUFFER_DESC objectBufferDesc = {};
	objectBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	objectBufferDesc.ByteWidth = sizeof(ObjectMatrix);
	objectBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	objectBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	objectBufferDesc.MiscFlags = 0;
	objectBufferDesc.StructureByteStride = 0;
	device.CreateBuffer(&objectBufferDesc, NULL, mObjectBuffer.GetAddressOf());

	// Build Material Constant Buffers
	D3D11_BUFFER_DESC materialBufferDesc = {};
	materialBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	materialBufferDesc.ByteWidth = sizeof(Material);
	materialBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	materialBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	materialBufferDesc.MiscFlags = 0;
	materialBufferDesc.StructureByteStride = 0;
	device.CreateBuffer(&materialBufferDesc, NULL, mMaterialBuffer.GetAddressOf());
}

void Object::Render(ID3D11DeviceContext& deviceContext)
{
	XMMATRIX world = XMMatrixIdentity();
	world *= XMMatrixScaling(mScale.x, mScale.y, mScale.z);
	world *= XMMatrixRotationRollPitchYaw(mRotation.x, mRotation.y, mRotation.z);
	world *= XMMatrixTranslation(mPosition.x, mPosition.y, mPosition.z);
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

	// Set PS constant buffer.
	deviceContext.Map(mMaterialBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	Material* materialBufferPtr = reinterpret_cast<Material*>(mappedResource.pData);
	materialBufferPtr->DiffuseAlbedo = mMaterial.DiffuseAlbedo;
	materialBufferPtr->Fresnel = mMaterial.Fresnel;
	materialBufferPtr->Roughness = mMaterial.Roughness;

	deviceContext.Unmap(mMaterialBuffer.Get(), 0);

	bufferNumber = 0;
	deviceContext.PSSetConstantBuffers(bufferNumber, 1, mMaterialBuffer.GetAddressOf());

	// Set texture.
	deviceContext.PSSetShaderResources(0, 1, mTexture.GetShaderResourceView().GetAddressOf());

	// Input Assembler
	UINT stride = sizeof(Vertex);	
	UINT offset = 0;
	deviceContext.IASetVertexBuffers(0, 1, mVertexBuffer.GetAddressOf(), &stride, &offset);
	deviceContext.IASetIndexBuffer(mIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	deviceContext.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Draw
	deviceContext.DrawIndexed(mIndexCount, 0, 0);
}

void Object::SetPosition(float x, float y, float z)
{
	mPosition.x = x;
	mPosition.y = y;
	mPosition.z = z;
}

void Object::SetPosition(XMFLOAT3 position)
{
	mPosition = position;
}

void Object::SetScale(float x, float y, float z)
{
	mScale.x = x;
	mScale.y = y;
	mScale.z = z;
}

void Object::SetScale(XMFLOAT3 scale)
{
	mScale = scale;
}

void Object::SetMaterial(XMFLOAT4 diffuseAlbedo, XMFLOAT3 fresnel, float roughness)
{
	mMaterial.DiffuseAlbedo = diffuseAlbedo;
	mMaterial.Fresnel = fresnel;
	mMaterial.Roughness = roughness;
}

void Object::SetTexture(ID3D11Device& device, const WCHAR* filename)
{
	mTexture = Texture(device, filename);
}
