#pragma once

#include "Shader.h"

class DepthShader : public Shader
{
private:
	DepthShader() = default;

public:
	static DepthShader* StaticClass()
	{
		static DepthShader basicShader;
		return &basicShader;
	}

	virtual void Init(ID3D11Device& device) override;

	virtual void SetShader(ID3D11DeviceContext& deviceContext) override;

private:
	virtual void BuildShader(ID3D11Device& device) override;
};
