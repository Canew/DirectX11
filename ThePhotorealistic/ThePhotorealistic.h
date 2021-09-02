#pragma once

#include "resource.h"
#include "D3DUtil.h"
#include "GameTimer.h"
#include <string>
#include <memory>

class ThePhotorealistic
{
public:
	ThePhotorealistic(HINSTANCE hInstance);
	virtual ~ThePhotorealistic();

	HINSTANCE AppInst() const;
	HWND MainWnd() const;

	int Run();

	// Framework methods.  Derived client class overrides these methods to 
	// implement specific application requirements.

	virtual bool Init();
	virtual void OnResize();
	virtual void UpdateScene(float dt);
	virtual void DrawScene();
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// Convenience overrides for handling mouse input.
	virtual void OnMouseDown(WPARAM btnState, int x, int y) { }
	virtual void OnMouseUp(WPARAM btnState, int x, int y) { }
	virtual void OnMouseMove(WPARAM btnState, int x, int y) { }

private:
	bool InitMainWindow();
	bool InitDirect3D();

	void CalculateFrameStats();

private:
	HINSTANCE mhAppInst;
	HWND mhMainWnd;
	bool mAppPaused;
	bool mMinimized;
	bool mMaximized;
	bool mResizing;
	UINT m4xMsaaQuality;

	GameTimer mTimer;

	ComPtr<ID3D11Device> md3dDevice;
	ComPtr<ID3D11DeviceContext> md3dImmediateContext;
	ComPtr<IDXGISwapChain> mSwapChain;
	ComPtr<ID3D11Texture2D> mDepthStencilBuffer;
	ComPtr<ID3D11RenderTargetView> mRenderTargetView;
	ComPtr<ID3D11DepthStencilView> mDepthStencilView;
	ComPtr<ID3D11RasterizerState> mRasterizerState;
	D3D11_VIEWPORT mScreenViewport;

	// Derived class should set these in derived constructor to customize starting values.
	std::wstring mMainWndCaption;
	D3D_DRIVER_TYPE md3dDriverType;
	int mClientWidth;
	int mClientHeight;
	bool mEnable4xMsaa;

	std::unique_ptr<class Object> mObject;
	std::unique_ptr<class Object> mPlane;
	std::unique_ptr<class Shader> mShader;

	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;
};