#pragma once

#include "D3DUtil.h"

class Fog
{
public:
	Fog() = default;
	~Fog() = default;

	float GetFogStart() const { return mFogStart; }
	float GetFogRange() const { return mFogRange; }
	XMFLOAT4 GetFogColor() const { return mFogColor; }

	void SetFogStart(float fogStart) { mFogStart = fogStart; }
	void SetFogRange(float fogRange) { mFogRange = fogRange; }
	void SetFogColor(XMFLOAT4 fogColor) { mFogColor = fogColor; }

private:
	float mFogStart;
	float mFogRange;
	XMFLOAT4 mFogColor;
};

