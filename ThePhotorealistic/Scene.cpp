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

    // Ground and Asphalte
    auto pGround = std::make_unique<Grid>(device, deviceContext, 300.0f, 300.0f, 300, 300);
    pGround->SetTexture(device, L"Texture/gravel_diffuse.dds");
    pGround->SetPosition(0.0f, 0.0f, 0.0f);
    mObjectList.push_back(std::move(pGround));

    // Cube
    auto pCube = std::make_unique<Cube>(device, deviceContext, 5.0f, 5.0f, 5.0f);
    pCube->SetPosition(5.0f, 10.0f, 0.0f);
    pCube->SetTexture(device, L"Texture/bricks2.dds");
    mObjectList.push_back(std::move(pCube));

    // Sphere
    for (int i = 0; i <= 10; i++)
    {
        auto pSphere = std::make_unique<Object>(device, deviceContext, "Model/BasicSphere.fbx");
        pSphere->SetPosition(15.0f + i * 10.0f, 15.0f, 0.0f);
        pSphere->SetScale(5.0f, 5.0f, 5.0f);
        pSphere->SetRoughness(i * 0.1f);
        pSphere->SetTexture(device, L"Texture/divingBoardFloor_diffuse.dds");
        mObjectList.push_back(std::move(pSphere));
    }

    // Fence
    auto pFence = std::make_unique<Grid>(device, deviceContext, 10.0f, 10.0f, 10, 10);
    pFence->SetPosition(0.0f, 10.0f, 0.0f);
    pFence->SetRotation(0.0f, 0.0f, XMConvertToRadians(90.0f));
    pFence->SetTexture(device, L"Texture/WireFence.dds");
    mObjectList.push_back(std::move(pFence));

    // Model
    auto pModel = std::make_unique<Object>(device, deviceContext, "Model/Rwby.fbx");
    pModel->SetTexture(device, L"Model/Rwby_Texture.png");
    pModel->SetPosition(40.0f, 10.0f, 20.0f);
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
    passBufferDesc.ByteWidth = sizeof(LightingPass);
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
    LightingPass* lightingPassBufferPtr = reinterpret_cast<LightingPass*>(mappedResource.pData);
    lightingPassBufferPtr->EyePosition = XMFLOAT3(Camera::GetInstance()->GetPosition().x, Camera::GetInstance()->GetPosition().y, Camera::GetInstance()->GetPosition().z);
    lightingPassBufferPtr->Light.Ambient = { 0.1f, 0.1f, 0.1f, 1.0f };
    lightingPassBufferPtr->Light.Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
    lightingPassBufferPtr->Light.Direction = { 0.57735f, -0.57735f, 0.57735f };
    lightingPassBufferPtr->Light.Strength = { 1.0f, 1.0f, 1.0f };
    lightingPassBufferPtr->FogStart = 15.0f;
    lightingPassBufferPtr->FogRange = 500.0f;
    lightingPassBufferPtr->FogColor = { 0.22f, 0.2f, 0.2f, 0.5f };

    deviceContext.Unmap(mPassBuffer.Get(), 0);

    unsigned int bufferNumber = 1;
    deviceContext.PSSetConstantBuffers(bufferNumber, 1, mPassBuffer.GetAddressOf());
}
