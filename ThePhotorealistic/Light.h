#pragma once

#include "D3DUtil.h"

class Light
{
    enum class LightType
    {
        DirectionalLight,
        PointLight,
        SpotLight
    };

public:
    Light() = default;
    virtual ~Light() = default;

    LightType GetLightType() const { return mLightType; }
    XMFLOAT4 GetAmbient() const { return mAmbient; }
    XMFLOAT4 GetDiffuse() const { return mDiffuse; }
    XMFLOAT3 GetIntensity() const { return mIntensity; }
    XMFLOAT3 GetDirection() const { return mDirection; }
    XMFLOAT3 GetPosition() const { return mPosition; }

    void SetLightType(LightType type) { mLightType = type; }
    void SetAmbient(XMFLOAT4 ambient) { mAmbient = ambient; }
    void SetDiffuse(XMFLOAT4 diffuse) { mDiffuse = diffuse; }
    void SetIntensity(XMFLOAT3 intensity) { mIntensity = intensity; }
    void SetDirection(XMFLOAT3 direction) { mDirection = direction; }
    void SetPosition(XMFLOAT3 position) { mPosition = position; }

    XMMATRIX GetView();

private:
    LightType mLightType;

    XMFLOAT4 mAmbient;
    XMFLOAT4 mDiffuse;
    XMFLOAT3 mIntensity;
    XMFLOAT3 mDirection;    // directional/spot light only
    XMFLOAT3 mPosition;     // point light only
};
