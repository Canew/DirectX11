#include "stdafx.h"
#include "Camera.h"
#include "BasicShader.h"
#include "SkyShader.h"
#include "RenderState.h"
#include "Scene.h"
#include "Object.h"
#include "Cube.h"
#include "Sphere.h"
#include "Geosphere.h"
#include "Cylinder.h"
#include "Plane.h"
#include "Grid.h"
#include "Hill.h"
#include "Sky.h"

Scene::Scene(ID3D11Device& device, ID3D11DeviceContext& deviceContext)
    : mDevice(device), mDeviceContext(deviceContext)
{
    CreateConstantBuffer(device);

    // Hills
    for (int i = -1; i <= 1; ++i)
    {
        for (int j = -1; j <= 1; ++j)
        {
            if (i == 0 && j == 0)
                continue;

            auto pHill = std::make_unique<Hill>(device, deviceContext, 300.0f, 300.0f, 300, 300);
            pHill->SetTexture(device, L"Texture/gravel_diffuse.dds");
            pHill->SetMaterial(XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f), XMFLOAT3(0.05f, 0.05f, 0.05f), 0.1f);
            pHill->SetPosition(i * 300.0f, -10.0f, j * 300.0f);

            mObjectList.push_back(std::move(pHill));
        }
    }

    // Ground and Asphalte
    auto pGround = std::make_unique<Grid>(device, deviceContext, 300.0f, 300.0f, 300, 300);
    pGround->SetTexture(device, L"Texture/gravel_diffuse.dds");
    pGround->SetMaterial(XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f), XMFLOAT3(0.05f, 0.05f, 0.05f), 0.1f);
    pGround->SetPosition(0.0f, 0.0f, 0.0f);
    mObjectList.push_back(std::move(pGround));

    auto pAsphalte = std::make_unique<Grid>(device, deviceContext, 50.0f, 300.0f, 50, 300);
    pAsphalte->SetTexture(device, L"Texture/asphalt_diffuse.dds");
    pAsphalte->SetMaterial(XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f), XMFLOAT3(0.05f, 0.05f, 0.05f), 0.1f);
    pAsphalte->SetPosition(0.0f, 0.1f, 0.0f);
    mObjectList.push_back(std::move(pAsphalte));

    auto pAsphalteGuard1 = std::make_unique<Grid>(device, deviceContext, 5.0f, 300.0f, 5, 300);
    pAsphalteGuard1->SetTexture(device, L"Texture/divingBoardFloor_diffuse.dds");
    pAsphalteGuard1->SetMaterial(XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f), XMFLOAT3(0.05f, 0.05f, 0.05f), 0.1f);
    pAsphalteGuard1->SetPosition(25.0f, 0.2f, 0.0f);
    mObjectList.push_back(std::move(pAsphalteGuard1));

    auto pAsphalteGuard2 = std::make_unique<Grid>(device, deviceContext, 5.0f, 300.0f, 5, 300);
    pAsphalteGuard2->SetTexture(device, L"Texture/divingBoardFloor_diffuse.dds");
    pAsphalteGuard2->SetMaterial(XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f), XMFLOAT3(0.05f, 0.05f, 0.05f), 0.1f);
    pAsphalteGuard2->SetPosition(-25.0f, 0.2f, 0.0f);
    mObjectList.push_back(std::move(pAsphalteGuard2));

    // Fence
    auto pFence = std::make_unique<Grid>(device, deviceContext, 10.0f, 10.0f, 10, 10);
    pFence->SetPosition(0.0f, 1.0f, 0.0f);
    pFence->SetRotation(0.0f, 0.0f, XMConvertToRadians(90.0f));
    pFence->SetTexture(device, L"Texture/WireFence.dds");
    pFence->SetMaterial(XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f), XMFLOAT3(0.05f, 0.05f, 0.05f), 0.1f);
    mObjectList.push_back(std::move(pFence));

    // Poles
    for (int i = -1; i <= 1; ++i)
    {
        for (int j = -2; j <= 2; ++j)
        {
            if (i == 0)
                continue;

            auto pPole = std::make_unique<Cylinder>(device, deviceContext, 1.0f, 1.0f, 30.0f, 10, 20);
            pPole->SetPosition(i * 30.0f, 15.0f, j * 80.0f);
            pPole->SetTexture(device, L"Texture/bricks2.dds");
            pPole->SetMaterial(XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f), XMFLOAT3(0.05f, 0.05f, 0.05f), 0.1f);
            mObjectList.push_back(std::move(pPole));
            

            auto pPoleProp = std::make_unique<Sphere>(device, deviceContext, 2.0f, 10, 10);
            pPoleProp->SetPosition(i * 30.0f, 0.0f, j * 80.0f);
            pPoleProp->SetTexture(device, L"Texture/stone.dds");
            pPoleProp->SetMaterial(XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f), XMFLOAT3(0.05f, 0.05f, 0.05f), 0.1f);
            mObjectList.push_back(std::move(pPoleProp));
        }
    }

    auto pWater = std::make_unique<Plane>(device, deviceContext);
    pWater->SetPosition(0.0f, -10.0f, 0.0f);
    pWater->SetScale(200.0f, 1.0f, 200.0f);
    pWater->SetTexture(device, L"Texture/water2.dds");
    pWater->SetMaterial(XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f), XMFLOAT3(0.05f, 0.05f, 0.05f), 0.1f);
    mObjectList.push_back(std::move(pWater));

    // Model
    auto pModel = std::make_unique<Object>(device, deviceContext, "Model/Rwby.fbx");
    pModel->SetPosition(0.0f, 10.0f, 0.0f);
    pModel->SetScale(10.0f, 10.0f, 10.0f);
    mObjectList.push_back(std::move(pModel));

    // Sky
    std::unique_ptr<Sky> pSky = std::make_unique<Sky>(device, deviceContext, 5000.0f);
    pSky->SetShaderClass(SkyShader::StaticClass());
    pSky->SetTexture(device, L"Texture/sunsetcube1024.dds");
    mSky = std::move(pSky);
}

void Scene::Update(ID3D11DeviceContext& deviceContext, float dt)
{
    for (auto& object : mObjectList)
    {
        object->Update(deviceContext, dt);
    }
}

void Scene::Render(ID3D11DeviceContext& deviceContext)
{
    // Render sky.
    deviceContext.RSSetState(RenderState::NoCullRS.Get());
    deviceContext.OMSetDepthStencilState(RenderState::LessEqualDSS.Get(), 0);
    mSky->GetShaderClass()->SetShader(deviceContext);
    mSky->Render(deviceContext);

    // Render object.
    for (auto& object : mObjectList)
    {
        UpdateConstantBuffer(deviceContext);

        object->GetShaderClass()->SetShader(deviceContext);

        // Set TransparentBS if object is transparent.
        //if (object->IsTransparent())
        //{
        //    deviceContext.OMSetBlendState(RenderState::TransparentBS.Get(), RenderState::DefaultBlendFactor.get(), 0xffffffff);
        //}

        deviceContext.OMSetDepthStencilState(0, 0);
        object->Render(deviceContext);

        // return to defaultBS.
        deviceContext.OMSetBlendState(0, RenderState::DefaultBlendFactor.get(), 0xffffffff);
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

void Scene::UpdateConstantBuffer(ID3D11DeviceContext& deviceContext)
{
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    deviceContext.Map(mPassBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

    Pass* passBufferPtr = reinterpret_cast<Pass*>(mappedResource.pData);
    passBufferPtr->EyePosition = XMFLOAT3(Camera::StaticClass()->GetPosition().x, Camera::StaticClass()->GetPosition().y, Camera::StaticClass()->GetPosition().z);
    passBufferPtr->Lights[0].Ambient = { 0.2f, 0.2f, 0.2f, 1.0f };
    passBufferPtr->Lights[0].Diffuse = { 0.8f, 0.8f, 0.8f, 1.0f };
    passBufferPtr->Lights[0].Direction = { 0.57735f, -0.57735f, 0.57735f };
    passBufferPtr->Lights[0].Strength = { 0.6f, 0.6f, 0.6f };
    passBufferPtr->FogStart = 15.0f;
    passBufferPtr->FogRange = 500.0f;
    passBufferPtr->FogColor = { 0.22f, 0.2f, 0.2f, 0.5f };

    deviceContext.Unmap(mPassBuffer.Get(), 0);

    unsigned int bufferNumber = 1;
    deviceContext.PSSetConstantBuffers(bufferNumber, 1, mPassBuffer.GetAddressOf());
}
