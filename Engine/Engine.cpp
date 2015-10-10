#include "Engine.h"
#include "Entity.h"

Engine::Engine(HINSTANCE in_hInstance)
	:wind(in_hInstance)
{

}

Engine::~Engine()
{
	if(SwapChain)
		SwapChain->Release();
	if(d3d11Device)
		d3d11Device->Release();
	if(d3d11DevCon)
		d3d11DevCon->Release();
	if(renderTargetView)
		renderTargetView->Release();
	if(depthStencilView)
		depthStencilView->Release();
	if(depthStencilBuffer)
		depthStencilBuffer->Release();
	if(VS)
		VS->Release();
	if(PS)
		PS->Release();
	if(VS_Buffer)
		VS_Buffer->Release();
	if(PS_Buffer)
		PS_Buffer->Release();
}

void Engine::tick()
{
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
	bufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
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
	swapChainDesc.Windowed = true;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	IDXGIFactory1 *DXGIFactory;

	if (hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&DXGIFactory) != S_OK)
	{
		err_say(L"Unable to create DXGI factory");
		return false;
	}

	IDXGIAdapter1 *Adapter;

	if (hr = DXGIFactory->EnumAdapters1(0, &Adapter) != S_OK)
	{
		err_say(L"Something else...");
		return false;
	}

	DXGIFactory->Release();

	if (hr = D3D11CreateDeviceAndSwapChain(Adapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, D3D11_CREATE_DEVICE_BGRA_SUPPORT,
		NULL, NULL, D3D11_SDK_VERSION, &swapChainDesc, &SwapChain, &d3d11Device, NULL, &d3d11DevCon) != S_OK)
	{
		err_say(L"Failed creating swap chain");
		return false;
	}
	Adapter->Release();

	//Create our BackBuffer and Render Target
	if (hr = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&BackBuffer11) != S_OK)
	{
		err_say(L"Failed creating backbuffer");
		return false;
	}

	//Describe our Depth/Stencil Buffer
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
	if (hr = d3d11Device->CreateTexture2D(&depthStencilDesc, NULL, &depthStencilBuffer) != S_OK)
	{
		err_say(L"Failed to create 2d texture");
		return false;
	}

	if (hr = d3d11Device->CreateDepthStencilView(depthStencilBuffer, NULL, &depthStencilView) != S_OK)
	{
		err_say(L"Failed to create depth stencil");
		return false;
	}

	initScene();
	messageloop();
	return true;
}



bool Engine::initScene()
{
	if (hr = D3DX11CompileFromFile(L"Effects.hlsl", 0, 0, "VS", "vs_5_0", 0, 0, 0, &VS_Buffer, 0, 0) != S_OK)
	{
		err_say(L"Failed to compile vertex buffer");
		return false;
	}
	if (hr = D3DX11CompileFromFile(L"Effects.hlsl", 0, 0, "PS", "ps_5_0", 0, 0, 0, &PS_Buffer, 0, 0) != S_OK)
	{
		err_say(L"Failed to compile pixel buffer");
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

	d3d11DevCon->VSSetShader(VS, 0, 0);
	d3d11DevCon->PSSetShader(PS, 0, 0);

	Entity * MyEntity = new Entity(this);
	if (MyEntity->initialize())
		Things.push_back(MyEntity);
	else
		delete MyEntity;

	return true;
}

void Engine::updateScene()
{
	for (auto iT = Things.begin(); iT != Things.end(); iT++)
	{
		(*iT)->update();
	}
}

void Engine::drawScene()
{
	//Clear our backbuffer
	float bgColor[4] = { (0.0f, 0.0f, 0.0f, 0.0f) };
	d3d11DevCon->ClearRenderTargetView(renderTargetView, bgColor);
	for (auto iT = Things.begin(); iT != Things.end(); iT++)
	{
		(*iT)->manifest();
	}

	//Present the backbuffer to the screen
	SwapChain->Present(0, 0);
}