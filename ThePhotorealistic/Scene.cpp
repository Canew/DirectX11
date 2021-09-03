#include "Camera.h"
#include "Scene.h"
#include "Object.h"
#include "Cube.h"
#include "Grid.h"

Scene::Scene(ID3D11Device& device)
{
    CreateConstantBuffer(device);

    std::unique_ptr<Object> pCube = std::make_unique<Cube>(device, 1.0f, 1.0f, 1.0f);
    pCube->SetPosition(0.0f, 10.0f, 0.0f);
    pCube->SetMaterial(XMFLOAT4(0.5f, 0.5f, 0.5f, 0.1f), XMFLOAT3(0.05f, 0.05f, 0.05f), 0.1f);

    std::unique_ptr<Object> pGrid = std::make_unique<Grid>(device, 300.0f, 300.0f, 100, 100);
    pGrid->SetTexture(device, L"Texture/grass.dds");
    pCube->SetMaterial(XMFLOAT4(0.5f, 0.5f, 0.5f, 0.1f), XMFLOAT3(0.05f, 0.05f, 0.05f), 0.1f);
    pGrid->SetPosition(0.0f, -5.0f, 0.0f);

    mObjectList.push_back(std::move(pCube));
    mObjectList.push_back(std::move(pGrid));
}

void Scene::Render(ID3D11DeviceContext& deviceContext)
{
    SetConstantBuffer(deviceContext);

    for (auto& Object : mObjectList)
    {
        Object->Render(deviceContext);
    }
}

void Scene::CreateConstantBuffer(ID3D11Device& device)
{
    // Build Pass Constant Buffers
    D3D11_BUFFER_DESC passBufferDesc = {};
    passBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    passBufferDesc.ByteWidth = sizeof(Pass);
    passBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    passBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    passBufferDesc.MiscFlags = 0;
    passBufferDesc.StructureByteStride = 0;
    device.CreateBuffer(&passBufferDesc, NULL, mPassBuffer.GetAddressOf());
}

void Scene::SetConstantBuffer(ID3D11DeviceContext& deviceContext)
{
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    deviceContext.Map(mPassBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

    Pass* passBufferPtr = reinterpret_cast<Pass*>(mappedResource.pData);
    passBufferPtr->AmbientLight = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
    passBufferPtr->EyePosition = XMFLOAT3(Camera::GetInstance()->GetPosition().x, Camera::GetInstance()->GetPosition().y, Camera::GetInstance()->GetPosition().z);
    passBufferPtr->Lights[0].Direction = { 0.57735f, -0.57735f, 0.57735f };
    passBufferPtr->Lights[0].Strength = { 0.6f, 0.6f, 0.6f };

    deviceContext.Unmap(mPassBuffer.Get(), 0);

    unsigned int bufferNumber = 1;
    deviceContext.PSSetConstantBuffers(bufferNumber, 1, mPassBuffer.GetAddressOf());
}
