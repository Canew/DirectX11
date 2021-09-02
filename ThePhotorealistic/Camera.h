#pragma once

#include "D3DUtil.h"

class Camera
{
public:
	static Camera* GetInstance();

public:
	void UpdateClientSize(int width, int height);
	void UpdateViewMatrix();
	void UpdateProjectionMatrix();

	XMMATRIX GetView() const;
	XMMATRIX GetProjection() const;
	float AspectRatio() const;
	
private:
	XMFLOAT3 mPosition = XMFLOAT3(20.0f, 10.0f, 5.0f);
	int mClientWidth;
	int mClientHeight;
	XMFLOAT4X4 mView; 
	XMFLOAT4X4 mProjection;
};

