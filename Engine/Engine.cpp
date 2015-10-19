#include "Engine.h"
#include "Entity.h"
#include "RenderState.h"
#include "Ground.h"
#include "LightSource.h"

Engine::Engine(HINSTANCE in_hInstance)
	:wind(in_hInstance)
{
	mod.refresh("");
}

Engine::~Engine()
{
	if(SwapChain)SwapChain->Release();
	if(d3d11Device)d3d11Device->Release();
	if(d3d11DevCon)d3d11DevCon->Release();
	if(renderTargetView)renderTargetView->Release();
	if(VS)VS->Release();
	if(PS)PS->Release();
	if(VS_Buffer)VS_Buffer->Release();
	if(PS_Buffer)PS_Buffer->Release();
	if(depthStencilView)depthStencilView->Release();
	if(depthStencilBuffer)depthStencilBuffer->Release();
	if(cbPerObjectBuffer)cbPerObjectBuffer->Release();
	if(DIKeyboard)DIKeyboard->Unacquire();
	if(DIMouse)DIMouse->Unacquire();
	if(DirectInput)DirectInput->Release();
	if(cbPerFrameBuffer)cbPerFrameBuffer->Release();

	for (auto iT = Things.begin(); iT != Things.end(); iT++)
	{
		delete (*iT);
	}

	for (auto iT = render_states.begin(); iT != render_states.end(); iT++)
		delete (*iT).second;
}

void Engine::tick()
{
	frameCount++;
	if (GetTime() > 1.0f)
	{
		fps = frameCount;
		frameCount = 0;
		StartTimer();
	}

	frameTime = GetFrameTime();
	DetectInput();
	updateScene();
	drawScene();
}

bool Engine::initialize_engine()
{
	if (!this->initialize_window())
		return false;

	DXGI_MODE_DESC bufferDesc;

	ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));

	bufferDesc.Width = window_width;
	bufferDesc.Height = window_height;
	bufferDesc.RefreshRate.Numerator = 60;
	bufferDesc.RefreshRate.Denominator = 1;
	bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	//Describe our SwapChain
	DXGI_SWAP_CHAIN_DESC swapChainDesc;

	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	swapChainDesc.BufferDesc = bufferDesc;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.OutputWindow = hwnd;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;


	//Create our SwapChain
	if(hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL,D3D11_SDK_VERSION, &swapChainDesc, &SwapChain, &d3d11Device, NULL, &d3d11DevCon) != S_OK)
	{
		err_say(L"Unable to create swap chain");
		return false;
	}

	//Create our BackBuffer
	ID3D11Texture2D* BackBuffer;
	if(hr = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&BackBuffer) != S_OK)
	{
		err_say(L"Failed to create backbuffer");
		return false;
	}

	//Create our Render Target
	if(hr = d3d11Device->CreateRenderTargetView(BackBuffer, NULL, &renderTargetView) != S_OK)
	{
		err_say(L"Failed to create backbuffer");
		return false;
	}
	BackBuffer->Release();

	if (!InitDirectInput(hInstance))
	{
		MessageBox(0, L"Direct Input Initialization - Failed",L"Error", MB_OK);
		return 0;
	}

	d3d11DevCon->OMSetRenderTargets(1, &renderTargetView, NULL);
	initScene();
	messageloop();
	return true;
}



bool Engine::initScene()
{
	if (hr = D3DX11CompileFromFile(L"Effects.hlsl", 0, 0, "VS", "vs_5_0", 0, 0, 0, &VS_Buffer, 0, 0) != S_OK)
	{
		err_say(L"Failed to compile vertex shader");
		return false;
	}
	if (hr = D3DX11CompileFromFile(L"Effects.hlsl", 0, 0, "PS", "ps_5_0", 0, 0, 0, &PS_Buffer, 0, 0) != S_OK)
	{
		err_say(L"Failed to compile pixel shader");
		return false;
	}

	if (hr = D3DX11CompileFromFile(L"Effects.hlsl", 0, 0, "D2D_PS", "ps_5_0", 0, 0, 0, &D2D_PS_Buffer, 0, 0) != S_OK)
	{
		err_say(L"Failed to compile D2D shader");
		return false;
	}

	if (hr = d3d11Device->CreateVertexShader(VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), NULL, &VS) != S_OK)
	{
		err_say(L"Failed to create vertex shader");
		return false;
	}
	if (hr = d3d11Device->CreatePixelShader(PS_Buffer->GetBufferPointer(), PS_Buffer->GetBufferSize(), NULL, &PS) != S_OK)
	{
		err_say(L"Failed to create pixel shader");
		return false;
	}

	if (hr = d3d11Device->CreatePixelShader(D2D_PS_Buffer->GetBufferPointer(), D2D_PS_Buffer->GetBufferSize(), NULL, &D2D_PS) != S_OK)
	{
		err_say(L"Failed to create D2D shader");
		return false;
	}


	d3d11DevCon->VSSetShader(VS, 0, 0);
	d3d11DevCon->PSSetShader(PS, 0, 0);

	initializeRenderState();
	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = window_width;
	depthStencilDesc.Height = window_height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	//Create the Depth/Stencil View
	d3d11Device->CreateTexture2D(&depthStencilDesc, NULL, &depthStencilBuffer);
	d3d11Device->CreateDepthStencilView(depthStencilBuffer, NULL, &depthStencilView);

	//Set our Render Target
	d3d11DevCon->OMSetRenderTargets(1, &renderTargetView, depthStencilView);


	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = window_width;
	viewport.Height = window_height;
	d3d11DevCon->RSSetViewports(1, &viewport);

	//Create the buffer to send to the cbuffer in effect file
	D3D11_BUFFER_DESC cbbd;
	ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

	cbbd.Usage = D3D11_USAGE_DEFAULT;
	cbbd.ByteWidth = sizeof(cbPerObject);
	cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbbd.CPUAccessFlags = 0;
	cbbd.MiscFlags = 0;

	if(hr = d3d11Device->CreateBuffer(&cbbd, NULL, &cbPerObjectBuffer) != S_OK)
	{
		err_say(L"Failed to create pixel shader");
		return false;
	}


	ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

	cbbd.Usage = D3D11_USAGE_DEFAULT;
	cbbd.ByteWidth = sizeof(cbPerFrame);
	cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbbd.CPUAccessFlags = 0;
	cbbd.MiscFlags = 0;

	hr = d3d11Device->CreateBuffer(&cbbd, NULL, &cbPerFrameBuffer);
	camPosition = XMVectorSet(0.0f, 0.0f, -8.0f, 0.0f);
	camTarget = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	camUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	camView = XMMatrixLookAtLH(camPosition, camTarget, camUp);
	camProjection = XMMatrixPerspectiveFovLH(0.4f*3.14f, (float)window_width / window_height, 1.0f, 1000.0f);

	Entity * MyEntity = new Entity(this, 2, L"gate1.png");
	MyEntity->render_state = "no_cull";
	mod.refresh("POSITION");
	mod.f3 = 3.0f;
	MyEntity->modify(mod);
	mod.refresh("ROTATION");
	mod.f1 = 0.000001f;
	mod.f2 = 1.0f;
	mod.f3 = 0.0f;
	mod.f4 = 0.0f;
	mod.f5 = 0.0f;

	MyEntity->modify(mod);
	if (MyEntity->initialize())
		Things.push_back(MyEntity);
	else
		delete MyEntity;
		
	MyEntity = new Entity(this, 1, L"braynzar.jpg");
	MyEntity->modify(mod);
	if (MyEntity->initialize())
		Things.push_back(MyEntity);
	else
		delete MyEntity;

	MyEntity = new Ground(this, 3, L"grass.jpg");
	MyEntity->render_state = "no_cull";
	mod.refresh("POSITION");
	mod.f2 = -1.0f;
	MyEntity->modify(mod);
	mod.refresh("SCALE");
	mod.f1 = 500.0f;
	mod.f2 = 0.0f;
	mod.f3 = 500.0f;

	MyEntity->modify(mod);

	if (MyEntity->initialize())
		Things.push_back(MyEntity);
	else
		delete MyEntity;

	LightSource * MyLight = new LightSource(this, 5);
	MyLight->direction = { 0.25f, 0.6f, 1.0f };
	MyLight->ambient = { 0.2f, 0.2f, 0.2f, 1.0f };
	MyLight->diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	if (MyLight->initialize())
		Things.push_back(MyLight);
	else
		delete MyLight;

	return true;
}

void Engine::updateScene()
{
	rot += 2.5f * frameTime;
	if (rot > 6.28f)
		rot = 0.0f;

	SetWindowText(hwnd, std::to_wstring(fps).c_str());
	for (auto iT = Things.begin(); iT != Things.end(); iT++)
	{

		if ((*iT)->id == 2)
		{
			mod.refresh("ROTATION");
			mod.f5 = rot;
			(*iT)->modify(mod);
		}
		(*iT)->update();
	}
}

void Engine::drawScene()
{
	float bgColor[4] = {(1.0f, 0.0f, 0.0f, 0.0f)};
	d3d11DevCon->ClearRenderTargetView(renderTargetView, bgColor);
	d3d11DevCon->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	d3d11DevCon->VSSetShader(VS, 0, 0);
	d3d11DevCon->PSSetShader(PS, 0, 0);

	for (auto iT = Things.begin(); iT != Things.end(); iT++)
	{
		(*iT)->manifest();
	}
	SwapChain->Present(0, 0);
}

void Engine::UpdateCamera()
{
	camRotationMatrix = XMMatrixRotationRollPitchYaw(camPitch, camYaw, 0);
	camTarget = XMVector3TransformCoord(DefaultForward, camRotationMatrix);
	camTarget = XMVector3Normalize(camTarget);

	XMMATRIX RotateYTempMatrix;
	RotateYTempMatrix = XMMatrixRotationY(camYaw);
	camRight = XMVector3TransformCoord(DefaultRight, RotateYTempMatrix);
	camForward = XMVector3TransformCoord(DefaultForward, RotateYTempMatrix);

	camUp2 = XMVector3TransformCoord(DefaultUp, RotateYTempMatrix);

	camPosition += moveLeftRight*camRight;
	camPosition += moveBackForward*camForward;
	camPosition += (moveBackForward)*camUp2 * (camPitch*-1);
	moveLeftRight = 0.0f;
	moveBackForward = 0.0f;
	moveUpDown = 0.0f;
	camTarget = camPosition + camTarget;
	camView = XMMatrixLookAtLH(camPosition, camTarget, camUp2);
}

void Engine::StartTimer()
{
	LARGE_INTEGER frequencyCount;
	QueryPerformanceFrequency(&frequencyCount);

	countsPerSecond = double(frequencyCount.QuadPart);

	QueryPerformanceCounter(&frequencyCount);
	CounterStart = frequencyCount.QuadPart;
}

double Engine::GetTime()
{
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);
	return double(currentTime.QuadPart - CounterStart) / countsPerSecond;
}

double Engine::GetFrameTime()
{
	LARGE_INTEGER currentTime;
	__int64 tickCount;
	QueryPerformanceCounter(&currentTime);

	tickCount = currentTime.QuadPart - frameTimeOld;
	frameTimeOld = currentTime.QuadPart;

	if (tickCount < 0.0f)
		tickCount = 0.0f;

	return float(tickCount) / countsPerSecond;
}

bool Engine::InitDirectInput(HINSTANCE hInstance)
{
	if (hr = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&DirectInput, NULL) != S_OK)
	{
		err_say(L"Failed to create direct input");
		return false;
	}

	if(hr = DirectInput->CreateDevice(GUID_SysKeyboard,&DIKeyboard,NULL) != S_OK)
	{
		err_say(L"Failed to create keyboard device");
		return false;
	}
	if(hr = DirectInput->CreateDevice(GUID_SysMouse,&DIMouse,NULL) != S_OK)
	{
		err_say(L"Failed to create mouse device");
		return false;
	}
	if (hr = DIKeyboard->SetDataFormat(&c_dfDIKeyboard) != S_OK)
	{
		err_say(L"Failed to set data format for the keyboard");
		return false;
	}
	if (hr = DIKeyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE) != S_OK)
	{
		err_say(L"Failed to set cooperative level for the keyboard");
		return false;
	}
	if (hr = DIMouse->SetDataFormat(&c_dfDIMouse) != S_OK)
	{
		err_say(L"Failed to set data format for the mouse");
		return false;
	}
	if (hr = DIMouse->SetCooperativeLevel(hwnd, DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND) != S_OK)
	{
		err_say(L"Failed to set cooperative level for the mouse");
		return false;
	}

	return true;
}

void Engine::DetectInput()
{
	DIMOUSESTATE mouseCurrState;

	BYTE keyboardState[256];

	DIKeyboard->Acquire();
	DIMouse->Acquire();

	DIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseCurrState);

	DIKeyboard->GetDeviceState(sizeof(keyboardState), (LPVOID)&keyboardState);
	
	float speed = 15.0f * frameTime;

	if (keyboardState[DIK_A] & 0x80)
	{
		moveLeftRight -= speed;
	}
	if (keyboardState[DIK_D] & 0x80)
	{
		moveLeftRight += speed;
	}
	if (keyboardState[DIK_W] & 0x80)
	{
		moveBackForward += speed;
	}
	if (keyboardState[DIK_S] & 0x80)
	{
		moveBackForward -= speed;
	}
	if ((mouseCurrState.lX != mouseLastState.lX) || (mouseCurrState.lY != mouseLastState.lY))
	{
		camYaw += mouseLastState.lX * 0.001f;

		camPitch += mouseCurrState.lY * 0.001f;

		mouseLastState = mouseCurrState;
	}

	UpdateCamera();
}


void Engine::initializeRenderState()
{
	RenderState * tmp = new RenderState(this);
	tmp->rastDesc.FillMode = D3D11_FILL_SOLID;
	tmp->rastDesc.CullMode = D3D11_CULL_NONE;
	tmp->set();
	render_states["no_cull"] = tmp;
}
