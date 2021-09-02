#include "Camera.h"

Camera* Camera::GetInstance()
{
	static Camera camera;
	return &camera;
}

void Camera::UpdateClientSize(int width, int height)
{
	mClientWidth = width;
	mClientHeight = height;
}

void Camera::UpdateViewMatrix()
{
	XMVECTOR pos = XMLoadFloat3(&mPosition);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, V);
}

void Camera::UpdateProjectionMatrix()
{
	// Update Projection Matrix
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * XM_PI, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProjection, P);
}

XMMATRIX Camera::GetView() const
{
	return XMLoadFloat4x4(&mView);
}

XMMATRIX Camera::GetProjection() const
{
	return XMLoadFloat4x4(&mProjection);
}

float Camera::AspectRatio() const
{
	return static_cast<float>(mClientWidth) / mClientHeight;
}
