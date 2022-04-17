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
#include "Fog.h"
#include "ShadowMap.h"

Scene::Scene(ID3D11Device& device, ID3D11DeviceContext& deviceContext)
    : mDevice(device), mDeviceContext(deviceContext)
{
    CreateConstantBuffer(device);

    // Light
    std::unique_ptr<Light> pMainLight = std::make_unique<Light>();
    pMainLight->SetAmbient(XMFLOAT4(0.03f, 0.03f, 0.03f, 1.0f));
    pMainLight->SetDiffuse(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
    pMainLight->SetDirection(XMFLOAT3(0.17735f, -0.27735f, 0.87735f));
    pMainLight->SetIntensity(XMFLOAT3(0.2f, 0.2f, 0.2f));
    pMainLight->SetPosition(XMFLOAT3(0.0f, 250.0f, 0.0f));
    mLightList.push_back(std::move(pMainLight));

    // ShadowMap
    std::unique_ptr<ShadowMap> pShadowMap = std::make_unique<ShadowMap>(device, 1280, 720);
    pShadowMap->SetLightReference(mLightList[0].get());
    mShadowMap = std::move(pShadowMap);

    // Ground
    auto pGround = std::make_unique<Grid>(device, deviceContext, 300.0f, 300.0f, 300, 300);
    pGround->SetDiffuseMap(device, L"Texture/gravel_diffuse.dds");
    pGround->SetNormalMap(device, L"Texture/gravel_diffuse_normal.png");
    pGround->SetPosition(0.0f, 0.0f, 0.0f);
    pGround->SetRoughness(0.9f);
    mObjectList.push_back(std::move(pGround));

    // Cube
    auto pCube = std::make_unique<Cube>(device, deviceContext, 5.0f, 5.0f, 5.0f);
    pCube->SetPosition(5.0f, 10.0f, 0.0f);
    pCube->SetMetallic(0.02f);
    pCube->SetRoughness(0.98f);
    pCube->SetDiffuseMap(device, L"Texture/bricks2.dds");
    pCube->SetNormalMap(device, L"Texture/bricks2_normal.png");
    mObjectList.push_back(std::move(pCube));

    // Sphere
    std::shared_ptr<Texture> sphereDiffuse = std::make_shared<Texture>(device, L"Texture/divingBoardFloor_diffuse.dds");
    std::shared_ptr<Texture> sphereNormal = std::make_shared<Texture>(device, L"Texture/divingBoardFloor_diffuse_normal.png");
    for (int i = 0; i <= 5; i++)
    {
        for (int j = 0; j <= 5; j++)
        {
            auto pSphere = std::make_unique<Object>(device, deviceContext, "Model/BasicSphere_Subdivide3.fbx");
            pSphere->SetPosition(15.0f + i * 5.0f, 250.0f - j * 5.0f, 0.0f);
            pSphere->SetScale(2.5f, 2.5f, 2.5f);
            pSphere->SetRoughness(i * 0.2f);
            pSphere->SetMetallic(1.0f - (j * 0.2f));
            pSphere->SetDiffuseMap(sphereDiffuse);
            pSphere->SetNormalMap(sphereNormal);
            mObjectList.push_back(std::move(pSphere));
        }
    }

    // Fence
    auto pFence = std::make_unique<Grid>(device, deviceContext, 10.0f, 10.0f, 10, 10);
    pFence->SetPosition(0.0f, 10.0f, 0.0f);
    pFence->SetRotation(0.0f, 0.0f, XMConvertToRadians(90.0f));
    pFence->SetDiffuseMap(device, L"Texture/WireFence.dds");
    pFence->SetNormalMap(device, L"Texture/WireFence_normal.png");
    mObjectList.push_back(std::move(pFence));

    // Model
    auto pModel = std::make_unique<Object>(device, deviceContext, "Model/Rwby.fbx");
    pModel->SetDiffuseMap(device, L"Model/Rwby_Diffuse.png");
    pModel->SetNormalMap(device, L"Model/Rwby_Normal.png");
    pModel->SetPosition(40.0f, 10.0f, 20.0f);
    pModel->SetScale(10.0f, 10.0f, 10.0f);
    mObjectList.push_back(std::move(pModel));

    // Sky
    std::unique_ptr<Sky> pSky = std::make_unique<Sky>(device, deviceContext, 1000.0f);
    pSky->SetShaderClass(SkyShader::StaticClass());
    pSky->SetDiffuseMap(device, L"Texture/grasscube1024.dds");
    mSky = std::move(pSky);

    // Fog
    std::unique_ptr<Fog> pFog = std::make_unique<Fog>();
    pFog->SetFogStart(500.0f);
    pFog->SetFogRange(1000.0f);
    pFog->SetFogColor(XMFLOAT4(0.22f, 0.2f, 0.2f, 0.5f));
    mFog = std::move(pFog);
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
    // Render shadow map.
    UpdateConstantBuffer(deviceContext, mShadowMap->GetLightReference()->GetView());
    mShadowMap->Render(deviceContext, mObjectList);

    // Render object.
    BindRtvAndDsv(deviceContext);

    UpdateConstantBuffer(deviceContext, Camera::GetInstance()->GetView());
    deviceContext.PSSetShaderResources(2, 1, mShadowMap->GetDepthMapSRV().GetAddressOf());
    deviceContext.PSSetShaderResources(3, 1, mSky->GetDiffuseMap()->GetShaderResourceView().GetAddressOf());
    deviceContext.OMSetDepthStencilState(0, 0);
    for (auto& object : mObjectList)
    {
        object->Render(deviceContext);
    }

    // Render sky.
    deviceContext.RSSetState(RenderState::NoCullRS.Get());
    deviceContext.OMSetDepthStencilState(RenderState::LessEqualDSS.Get(), 0);
    mSky->Render(deviceContext);
}

void Scene::CreateConstantBuffer(ID3D11Device& device)
{
    // Build Frame Constant Buffers
    D3D11_BUFFER_DESC frameBufferDesc = {};
    frameBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    frameBufferDesc.ByteWidth = sizeof(FrameMatrix);
    frameBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    frameBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    frameBufferDesc.MiscFlags = 0;
    frameBufferDesc.StructureByteStride = 0;
    device.CreateBuffer(&frameBufferDesc, NULL, mFrameBuffer.GetAddressOf());

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

void Scene::UpdateConstantBuffer(ID3D11DeviceContext& deviceContext, XMMATRIX viewMatrix)
{
    XMMATRIX view = viewMatrix;
    XMMATRIX projection = Camera::GetInstance()->GetProjection();

    view = XMMatrixTranspose(view);
    projection = XMMatrixTranspose(projection);

    // VS constant buffer
    // Per frame
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    deviceContext.Map(mFrameBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

    FrameMatrix* frameBufferPtr = reinterpret_cast<FrameMatrix*>(mappedResource.pData);
    frameBufferPtr->View = view;
    frameBufferPtr->Projection = projection;

    deviceContext.Unmap(mFrameBuffer.Get(), 0);

    unsigned int bufferNumber = 1;
    deviceContext.VSSetConstantBuffers(bufferNumber, 1, mFrameBuffer.GetAddressOf());

    // PS constant buffer.
    deviceContext.Map(mPassBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

    LightingPass* lightingPassBufferPtr = reinterpret_cast<LightingPass*>(mappedResource.pData);
    lightingPassBufferPtr->EyePosition = XMFLOAT3(Camera::GetInstance()->GetPosition().x, Camera::GetInstance()->GetPosition().y, Camera::GetInstance()->GetPosition().z);

    lightingPassBufferPtr->LightAmbient = mLightList[0]->GetAmbient();
    lightingPassBufferPtr->LightDiffuse = mLightList[0]->GetDiffuse();
    lightingPassBufferPtr->LightIntensity = mLightList[0]->GetIntensity();
    lightingPassBufferPtr->LightDirection = mLightList[0]->GetDirection();
    lightingPassBufferPtr->LightPosition = mLightList[0]->GetPosition();

    lightingPassBufferPtr->FogStart = mFog->GetFogStart();
    lightingPassBufferPtr->FogRange = mFog->GetFogRange();
    lightingPassBufferPtr->FogColor = mFog->GetFogColor();

    deviceContext.Unmap(mPassBuffer.Get(), 0);

    bufferNumber = 1;
    deviceContext.PSSetConstantBuffers(bufferNumber, 1, mPassBuffer.GetAddressOf());
}

void Scene::BindRtvAndDsv(ID3D11DeviceContext& deviceContext)
{
    deviceContext.RSSetViewports(1, &mSceneViewport);

    deviceContext.OMSetRenderTargets(1, &mSceneRTV, mSceneDSV);
}
