#include "stdafx.h"
#include "Light.h"

XMMATRIX Light::GetView()
{
	XMVECTOR P = XMLoadFloat3(&mPosition);
	XMVECTOR U = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR L = XMVectorAdd(P, XMLoadFloat3(&mDirection));
	XMVECTOR R = XMVector3Cross(U, L);

	// Keep camera's axes orthogonal to each other and of unit length.
	L = XMVector3Normalize(L);
	U = XMVector3Normalize(XMVector3Cross(L, R));

	// U, L already ortho-normal, so no need to normalize cross product.
	R = XMVector3Normalize(R);

	// Fill in the view matrix entries.
	float x = -XMVectorGetX(XMVector3Dot(P, R));
	float y = -XMVectorGetX(XMVector3Dot(P, U));
	float z = -XMVectorGetX(XMVector3Dot(P, L));

	XMFLOAT3 right;
	XMStoreFloat3(&right, R);
	XMFLOAT3 up;
	XMStoreFloat3(&up, U);
	XMFLOAT3 look;
	XMStoreFloat3(&look, L);

	XMFLOAT4X4 view;

	view(0, 0) = right.x;
	view(1, 0) = right.y;
	view(2, 0) = right.z;
	view(3, 0) = x;

	view(0, 1) = up.x;
	view(1, 1) = up.y;
	view(2, 1) = up.z;
	view(3, 1) = y;

	view(0, 2) = look.x;
	view(1, 2) = look.y;
	view(2, 2) = look.z;
	view(3, 2) = z;

	view(0, 3) = 0.0f;
	view(1, 3) = 0.0f;
	view(2, 3) = 0.0f;
	view(3, 3) = 1.0f;

	return XMLoadFloat4x4(&view);
}
