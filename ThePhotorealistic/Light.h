#pragma once

#include "D3DUtil.h"

class Light
{
    enum class Type
    {
        DirectionalLight,
        PointLight,
        SpotLight
    };
    struct LightBuffer
    {
        XMFLOAT4 Ambient;
        XMFLOAT4 Diffuse;
        XMFLOAT3 Intensity;
        float FalloffStart;           // point/spot light only
        XMFLOAT3 Direction;    // directional/spot light only
        float FalloffEnd;            // point/spot light only
        XMFLOAT3 Position;     // point light only
        float SpotPower;          // spot light only
    };

public:
    Light(ID3D11Device& device);
    virtual ~Light() = default;

    void SetConstantBuffer(ID3D11DeviceContext& deviceContext);
    void SetLightType(Type type) { mLightType = type; }
    Type GetLightType() { return mLightType; }

private:
    ComPtr<ID3D11Buffer> mLightBuffer = nullptr;

    Type mLightType;
    XMFLOAT4 mAmbient;
    XMFLOAT4 mDiffuse;
    XMFLOAT3 mIntensity;
    float mFalloffStart;           // point/spot light only
    XMFLOAT3 mDirection;    // directional/spot light only
    float mFalloffEnd;            // point/spot light only
    XMFLOAT3 mPosition;     // point light only
    float mSpotPower;          // spot light only
};
