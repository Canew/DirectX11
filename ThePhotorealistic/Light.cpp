#include "Light.h"

Light::Light(ID3D11Device& device)
{
	// Build Constant Buffers
	D3D11_BUFFER_DESC lightBufferDesc = {};
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBuffer);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	device.CreateBuffer(&lightBufferDesc, NULL, mLightBuffer.GetAddressOf());
}

void Light::SetConstantBuffer(ID3D11DeviceContext& deviceContext)
{
	// Set PS Constant Buffer
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	deviceContext.Map(mLightBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	LightBuffer* lightBufferPtr = reinterpret_cast<LightBuffer*>(mappedResource.pData);
	lightBufferPtr->Strength = mStrength;
	lightBufferPtr->FalloffStart = mFalloffStart;
	lightBufferPtr->Direction = mDirection;
	lightBufferPtr->FalloffEnd = mFalloffEnd;
	lightBufferPtr->Position = mPosition;
	lightBufferPtr->SpotPower = mSpotPower;

	deviceContext.Unmap(mLightBuffer.Get(), 0);

	unsigned int bufferNumber = 1;
	deviceContext.PSSetConstantBuffers(bufferNumber, 1, mLightBuffer.GetAddressOf());
}
