// ThePhotorealistic.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <windowsX.h>
#include <sstream>
#include "BasicShader.h"
#include "SkyShader.h"
#include "Scene.h"
#include "Object.h"
#include "Camera.h"
#include "RenderState.h"
#include "ThePhotorealistic.h"

namespace
{
	// This is just used to forward Windows messages from a global window
	// procedure to our member function window procedure because we cannot
	// assign a member function to WNDCLASS::lpfnWndProc.
	ThePhotorealistic* gd3dApp = 0;
}

LRESULT CALLBACK
MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Forward hwnd on because we can get messages (e.g., WM_CREATE)
	// before CreateWindow returns, and thus before mhMainWnd is valid.
	return gd3dApp->MsgProc(hwnd, msg, wParam, lParam);
}

ThePhotorealistic::ThePhotorealistic(HINSTANCE hInstance)
	: mhAppInst(hInstance),
	mMainWndCaption(L"ThePhotorealistic"),
	md3dDriverType(D3D_DRIVER_TYPE_HARDWARE),
	mClientWidth(800),
	mClientHeight(600),
	mEnable4xMsaa(false),
	mhMainWnd(0),
	mAppPaused(false),
	mMinimized(false),
	mMaximized(false),
	mResizing(false),
	m4xMsaaQuality(0),

	md3dDevice(0),
	md3dImmediateContext(0),
	mSwapChain(0),
	mDepthStencilBuffer(0),
	mRenderTargetView(0),
	mDepthStencilView(0)
{
	ZeroMemory(&mScreenViewport, sizeof(D3D11_VIEWPORT));

	// Get a pointer to the application object so we can forward 
	// Windows messages to the object's window procedure through
	// the global window procedure.
	gd3dApp = this;
}

ThePhotorealistic::~ThePhotorealistic()
{
}

HINSTANCE ThePhotorealistic::AppInst() const
{
	return mhAppInst;
}

HWND ThePhotorealistic::MainWnd() const
{
	return mhMainWnd;
}

int ThePhotorealistic::Run()
{
	MSG msg = { 0 };

	GameTimer::StaticClass()->Reset();

	while (msg.message != WM_QUIT)
	{
		// If there are Window messages then process them.
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// Otherwise, do animation/game stuff.
		else
		{
			GameTimer::StaticClass()->Tick();

			if (!mAppPaused)
			{
				CalculateFrameStats();
				ProcessInput(GameTimer::StaticClass()->DeltaTime());
				UpdateScene(GameTimer::StaticClass()->DeltaTime());
				DrawScene();
			}
			else
			{
				Sleep(100);
			}
		}
	}

	return (int)msg.wParam;
}

bool ThePhotorealistic::Init()
{
	if (!InitMainWindow())
		return false;

	if (!InitDirect3D())
		return false;

	GameTimer::StaticClass();
	Camera::GetInstance();

	BasicShader::StaticClass()->Init(*md3dDevice.Get());
	SkyShader::StaticClass()->Init(*md3dDevice.Get());
	RenderState::Init(*md3dDevice.Get());

	mScene = std::make_unique<Scene>(*md3dDevice.Get(), *md3dImmediateContext.Get());

	return true;
}

void ThePhotorealistic::OnResize()
{
	assert(md3dImmediateContext);
	assert(md3dDevice);
	assert(mSwapChain);

	// Release the old views, as they hold references to the buffers we
	// will be destroying.  Also release the old depth/stencil buffer.

	// Resize the swap chain and recreate the render target view.
	mSwapChain->ResizeBuffers(1, mClientWidth, mClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
	ComPtr<ID3D11Texture2D> backBuffer;
	mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
	md3dDevice->CreateRenderTargetView(backBuffer.Get(), 0, mRenderTargetView.GetAddressOf());	

	// Create the depth/stencil buffer and view.
	D3D11_TEXTURE2D_DESC depthStencilDesc = {};
	depthStencilDesc.Width = mClientWidth;
	depthStencilDesc.Height = mClientHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// Use 4X MSAA? --must match swap chain MSAA values.
	if (mEnable4xMsaa)
	{
		depthStencilDesc.SampleDesc.Count = 4;
		depthStencilDesc.SampleDesc.Quality = m4xMsaaQuality - 1;
	}
	// No MSAA
	else
	{
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
	}

	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	md3dDevice->CreateTexture2D(&depthStencilDesc, 0, mDepthStencilBuffer.GetAddressOf());
	md3dDevice->CreateDepthStencilView(mDepthStencilBuffer.Get(), 0, mDepthStencilView.GetAddressOf());

	// Bind the render target view and depth/stencil view to the pipeline.
	md3dImmediateContext->OMSetRenderTargets(1, mRenderTargetView.GetAddressOf(), mDepthStencilView.Get());

	// Set Rasterizer State
	D3D11_RASTERIZER_DESC rasterDesc = {};
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	md3dDevice->CreateRasterizerState(&rasterDesc, mRasterizerState.GetAddressOf());
	md3dImmediateContext->RSSetState(mRasterizerState.Get());

	// Set the viewport transform.
	mScreenViewport.TopLeftX = 0;
	mScreenViewport.TopLeftY = 0;
	mScreenViewport.Width = static_cast<float>(mClientWidth);
	mScreenViewport.Height = static_cast<float>(mClientHeight);
	mScreenViewport.MinDepth = 0.0f;
	mScreenViewport.MaxDepth = 1.0f;

	md3dImmediateContext->RSSetViewports(1, &mScreenViewport);

	RECT clientRect;
	GetClientRect(mhMainWnd, &clientRect);
	Camera::GetInstance()->SetLens(0.25f * XM_PI, (clientRect.right - clientRect.left) / (clientRect.bottom - clientRect.top), 1.0f, 1000.0f);
}

void ThePhotorealistic::ProcessInput(float dt)
{
	// Process mouse input
	// Get current cursor position
	POINT cursorPos;
	GetCursorPos(&cursorPos);

	// Get center position of window
	RECT clientRect;
	GetClientRect(mhMainWnd, &clientRect);
	int horizonCenter = (clientRect.right - clientRect.left / 2);
	int verticalCenter = (clientRect.bottom - clientRect.top / 2);
	POINT centerPos = { horizonCenter, verticalCenter };

	// Update data if mouse position is different from center of window
	if (cursorPos.x != centerPos.x || cursorPos.y != centerPos.y)
	{
		float deltaX = static_cast<float>(cursorPos.x) - centerPos.x;
		float deltaY = static_cast<float>(cursorPos.y) - centerPos.y;

		Camera::GetInstance()->RotateY(deltaX * dt);
		Camera::GetInstance()->Pitch(deltaY * dt);

		// Set mouse coordinates to center of window
		SetCursorPos(horizonCenter, verticalCenter);

		// Update mouse coordinates
		mLastMouseX = static_cast<float>(cursorPos.x);
		mLastMouseY = static_cast<float>(cursorPos.y);
	}

	// Process keyboard input
	if (GetAsyncKeyState('W') & 0x8000)
	{
		float velocity = 50.0f * dt;

		Camera::GetInstance()->Walk(velocity);
	}
	if (GetAsyncKeyState('S') & 0x8000)
	{
		float velocity = -50.0f * dt;

		Camera::GetInstance()->Walk(velocity);
	}
	if (GetAsyncKeyState('A') & 0x8000)
	{
		float velocity = -50.0f * dt;

		Camera::GetInstance()->Strafe(velocity);
	}
	if (GetAsyncKeyState('D') & 0x8000)
	{
		float velocity = 50.0f * dt;

		Camera::GetInstance()->Strafe(velocity);
	}
	if (GetAsyncKeyState('Q') & 0x8000)
	{
		XMFLOAT3 currentPosition = Camera::GetInstance()->GetPosition();
		currentPosition.y += 50.0f * dt;
		Camera::GetInstance()->SetPosition(currentPosition);
	}
	if (GetAsyncKeyState('E') & 0x8000)
	{
		XMFLOAT3 currentPosition = Camera::GetInstance()->GetPosition();
		currentPosition.y -= 50.0f * dt;
		Camera::GetInstance()->SetPosition(currentPosition);
	}
}

void ThePhotorealistic::UpdateScene(float dt)
{
	Camera::GetInstance()->UpdateViewMatrix();

	mScene->Update(*md3dImmediateContext.Get(), dt);
}

void ThePhotorealistic::DrawScene()
{
	assert(md3dImmediateContext);
	assert(mSwapChain);

	float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView.Get(), color);
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	mScene->Render(*md3dImmediateContext.Get());

	mSwapChain->Present(0, 0);
}

LRESULT ThePhotorealistic::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		// WM_ACTIVATE is sent when the window is activated or deactivated.  
		// We pause the game when the window is deactivated and unpause it 
		// when it becomes active.  
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			mAppPaused = true;
			GameTimer::StaticClass()->Stop();
		}
		else
		{
			mAppPaused = false;
			GameTimer::StaticClass()->Start();
		}
		return 0;

		// WM_SIZE is sent when the user resizes the window.  
	case WM_SIZE:
		// Save the new client area dimensions.
		mClientWidth = LOWORD(lParam);
		mClientHeight = HIWORD(lParam);

		if (md3dDevice)
		{
			if (wParam == SIZE_MINIMIZED)
			{
				mAppPaused = true;
				mMinimized = true;
				mMaximized = false;
			}
			else if (wParam == SIZE_MAXIMIZED)
			{
				mAppPaused = false;
				mMinimized = false;
				mMaximized = true;
				OnResize();
			}
			else if (wParam == SIZE_RESTORED)
			{

				// Restoring from minimized state?
				if (mMinimized)
				{
					mAppPaused = false;
					mMinimized = false;
					OnResize();
				}

				// Restoring from maximized state?
				else if (mMaximized)
				{
					mAppPaused = false;
					mMaximized = false;
					OnResize();
				}
				else if (mResizing)
				{
					// If user is dragging the resize bars, we do not resize 
					// the buffers here because as the user continuously 
					// drags the resize bars, a stream of WM_SIZE messages are
					// sent to the window, and it would be pointless (and slow)
					// to resize for each WM_SIZE message received from dragging
					// the resize bars.  So instead, we reset after the user is 
					// done resizing the window and releases the resize bars, which 
					// sends a WM_EXITSIZEMOVE message.
				}
				else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
				{
					OnResize();
				}
			}
		}
		return 0;

		// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
	case WM_ENTERSIZEMOVE:
		mAppPaused = true;
		mResizing = true;
		GameTimer::StaticClass()->Stop();
		return 0;

		// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
		// Here we reset everything based on the new window dimensions.
	case WM_EXITSIZEMOVE:
		mAppPaused = false;
		mResizing = false;
		GameTimer::StaticClass()->Start();
		OnResize();
		return 0;

		// WM_DESTROY is sent when the window is being destroyed.
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

		// The WM_MENUCHAR message is sent when a menu is active and the user presses 
		// a key that does not correspond to any mnemonic or accelerator key. 
	case WM_MENUCHAR:
		// Don't beep when we alt-enter.
		return MAKELRESULT(0, MNC_CLOSE);

		// Catch this message so to prevent the window from becoming too small.
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
		return 0;

	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}


bool ThePhotorealistic::InitMainWindow()
{
	WNDCLASS wc = {};
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = mhAppInst;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"D3DWndClassName";

	if (!RegisterClass(&wc))
	{
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return false;
	}

	// Compute window rectangle dimensions based on requested client area dimensions.
	RECT R = { 0, 0, mClientWidth, mClientHeight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;

	mhMainWnd = CreateWindow(L"D3DWndClassName", mMainWndCaption.c_str(),
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, mhAppInst, 0);
	if (!mhMainWnd)
	{
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return false;
	}

	ShowWindow(mhMainWnd, SW_SHOW);
	UpdateWindow(mhMainWnd);

	return true;
}

bool ThePhotorealistic::InitDirect3D()
{
	// Create the device and device context.
	UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevel;
	HRESULT hr = D3D11CreateDevice(
		0,                 // default adapter
		md3dDriverType,
		0,                 // no software device
		createDeviceFlags,
		0, 0,              // default feature level array
		D3D11_SDK_VERSION,
		md3dDevice.GetAddressOf(),
		&featureLevel,
		md3dImmediateContext.GetAddressOf());

	if (FAILED(hr))
	{
		MessageBox(0, L"D3D11CreateDevice Failed.", 0, 0);
		return false;
	}

	if (featureLevel != D3D_FEATURE_LEVEL_11_0)
	{
		MessageBox(0, L"Direct3D Feature Level 11 unsupported.", 0, 0);
		return false;
	}

	// Check 4X MSAA quality support for our back buffer format.
	// All Direct3D 11 capable devices support 4X MSAA for all render 
	// target formats, so we only need to check quality support.

	md3dDevice->CheckMultisampleQualityLevels(
		DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m4xMsaaQuality);
	assert(m4xMsaaQuality > 0);

	// Fill out a DXGI_SWAP_CHAIN_DESC to describe our swap chain.

	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = mClientWidth;
	sd.BufferDesc.Height = mClientHeight;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// Use 4X MSAA? 
	if (mEnable4xMsaa)
	{
		sd.SampleDesc.Count = 4;
		sd.SampleDesc.Quality = m4xMsaaQuality - 1;
	}
	// No MSAA
	else
	{
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
	}

	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = mhMainWnd;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	// To correctly create the swap chain, we must use the IDXGIFactory that was
	// used to create the device.  If we tried to use a different IDXGIFactory instance
	// (by calling CreateDXGIFactory), we get an error: "IDXGIFactory::CreateSwapChain: 
	// This function is being called with a device from a different IDXGIFactory."
	ComPtr<IDXGIDevice> dxgiDevice = 0;
	md3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)dxgiDevice.GetAddressOf());

	ComPtr<IDXGIAdapter> dxgiAdapter = 0;
	dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)dxgiAdapter.GetAddressOf());

	ComPtr<IDXGIFactory> dxgiFactory = 0;
	dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)dxgiFactory.GetAddressOf());

	dxgiFactory->CreateSwapChain(md3dDevice.Get(), &sd, mSwapChain.GetAddressOf());

	// The remaining steps that need to be carried out for d3d creation
	// also need to be executed every time the window is resized.  So
	// just call the OnResize method here to avoid code duplication.

	OnResize();

	return true;
}

void ThePhotorealistic::CalculateFrameStats()
{
	// Code computes the average frames per second, and also the 
	// average time it takes to render one frame.  These stats 
	// are appended to the window caption bar.

	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;

	// Compute averages over one second period.
	if ((GameTimer::StaticClass()->TotalTime() - timeElapsed) >= 1.0f)
	{
		float fps = (float)frameCnt; // fps = frameCnt / 1
		float mspf = 1000.0f / fps;

		std::wostringstream outs;
		outs.precision(6);
		outs << mMainWndCaption << L"    "
			<< L"FPS: " << fps << L"    "
			<< L"Frame Time: " << mspf << L" (ms)";
		SetWindowText(mhMainWnd, outs.str().c_str());

		// Reset for next average.
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}
