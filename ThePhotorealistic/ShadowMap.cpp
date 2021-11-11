//***************************************************************************************
// ShadowMap.cpp by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************
#include "stdafx.h"
#include "ShadowMap.h"
#include "Camera.h"
#include "Light.h"
#include "Object.h"
#include "Shader.h"
#include "DepthShader.h"

ShadowMap::ShadowMap(ID3D11Device& device, UINT width, UINT height)
    : mWidth(width), mHeight(height)
{
    mViewport.TopLeftX = 0.0f;
    mViewport.TopLeftY = 0.0f;
    mViewport.Width = static_cast<float>(width);
    mViewport.Height = static_cast<float>(height);
    mViewport.MinDepth = 0.0f;
    mViewport.MaxDepth = 1.0f;

    // Use typeless format because the DSV is going to interpret
    // the bits as DXGI_FORMAT_D24_UNORM_S8_UINT, whereas the SRV is going to interpret
    // the bits as DXGI_FORMAT_R24_UNORM_X8_TYPELESS.
    D3D11_TEXTURE2D_DESC texDesc = {};
    texDesc.Width = mWidth;
    texDesc.Height = mHeight;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    texDesc.CPUAccessFlags = 0;
    texDesc.MiscFlags = 0;

    ComPtr<ID3D11Texture2D> depthMap;
    device.CreateTexture2D(&texDesc, 0, depthMap.GetAddressOf());

    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Flags = 0;
    dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture2D.MipSlice = 0;
    device.CreateDepthStencilView(depthMap.Get(), &dsvDesc, mDepthMapDSV.GetAddressOf());

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
    srvDesc.Texture2D.MostDetailedMip = 0;
    device.CreateShaderResourceView(depthMap.Get(), &srvDesc, mDepthMapSRV.GetAddressOf());
}

void ShadowMap::Render(ID3D11DeviceContext& deviceContext, std::vector<std::unique_ptr<Object>>& objects)
{
    BindDsvAndSetNullRenderTarget(deviceContext);

    for (std::unique_ptr<Object>& object : objects)
    {
        Shader* originShader = object->GetShaderClass();
        object->SetShaderClass(DepthShader::StaticClass());

        object->Render(deviceContext);

        object->SetShaderClass(originShader);
    }
}

ComPtr<ID3D11ShaderResourceView> ShadowMap::GetDepthMapSRV()
{
    return mDepthMapSRV;
}

Light* ShadowMap::GetLightReference()
{
    return mLightReference;
}

void ShadowMap::SetLightReference(Light* lightReference)
{
    mLightReference = lightReference;
}

void ShadowMap::BindDsvAndSetNullRenderTarget(ID3D11DeviceContext& deviceContext)
{
    deviceContext.RSSetViewports(1, &mViewport);

    // Set null render target because we are only going to draw to depth buffer.
    // Setting a null render target will disable color writes.
    ID3D11RenderTargetView* renderTargets[1] = { 0 };
    deviceContext.OMSetRenderTargets(1, renderTargets, mDepthMapDSV.Get());

    deviceContext.ClearDepthStencilView(mDepthMapDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}
