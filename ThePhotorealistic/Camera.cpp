#include "stdafx.h"
#include "Camera.h"

void Camera::UpdateClientSize(int width, int height)
{
	mClientWidth = width;
	mClientHeight = height;
}

void Camera::UpdateViewMatrix()
{
	XMVECTOR pos = XMLoadFloat3(&mPosition);
	XMVECTOR target = XMVectorSet(mPosition.x + 1.0f, mPosition.y, mPosition.z, 0.0f);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	float roll = XMConvertToRadians(mRotation.x);
	float pitch = XMConvertToRadians(mRotation.y);
	float yaw = XMConvertToRadians(mRotation.z);
	XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(roll, pitch, yaw);
	target = XMVector3TransformCoord(target, rotationMatrix);
	up = XMVector3TransformCoord(up, rotationMatrix);

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

void Camera::SetPosition(float x, float y, float z)
{
	mPosition.x = x;
	mPosition.y = y;
	mPosition.z = z;
}

void Camera::SetPosition(XMFLOAT3 position)
{
	mPosition = position;
}

void Camera::SetRotation(float x, float y, float z)
{
	mRotation.x = x;
	if (mRotation.x > 360.0f) mRotation.x = 0.0f;
	if (mRotation.x < 0.0f) mRotation.x = 0.0f;

	mRotation.y = y;
	if (mRotation.y > 360.0f) mRotation.y = 0.0f;
	if (mRotation.y < 0.0f) mRotation.y = 0.0f;

	mRotation.z = z;
	if (mRotation.z > 90.0f) mRotation.z = 90.0f;
	if (mRotation.z < -90.0f) mRotation.z = -90.0f;
}

void Camera::SetRotation(XMFLOAT3 rotation)
{
	mRotation = rotation;

	if (mRotation.x > 360.0f) mRotation.x = 0.0f;
	if (mRotation.x < 0.0f) mRotation.x = 0.0f;

	if (mRotation.y > 360.0f) mRotation.y = 0.0f;
	if (mRotation.y < 0.0f) mRotation.y = 0.0f;

	if (mRotation.z > 90.0f) mRotation.z = 90.0f;
	if (mRotation.z < -90.0f) mRotation.z = -90.0f;
}

XMFLOAT3 Camera::GetPosition()
{
	return mPosition;
}

XMFLOAT3 Camera::GetRotation()
{
	return mRotation;
}
