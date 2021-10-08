#pragma once

#include "D3DUtil.h"

class Camera
{
public:
	static Camera* GetInstance()
	{
		static Camera camera;
		return &camera;
	}

public:
	void UpdateClientSize(int width, int height);
	void UpdateViewMatrix();
	void UpdateProjectionMatrix();

	XMMATRIX GetView() const;
	XMMATRIX GetProjection() const;
	float AspectRatio() const;

	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 position);
	void SetRotation(float x, float y, float z);
	void SetRotation(XMFLOAT3 rotation);
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();
	
private:
	XMFLOAT3 mPosition = XMFLOAT3(5.0f, 10.0f, 5.0f);
	XMFLOAT3 mRotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	int mClientWidth;
	int mClientHeight;
	XMFLOAT4X4 mView; 
	XMFLOAT4X4 mProjection;
};

