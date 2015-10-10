#pragma once
#include "wind.h"
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#include <xnamath.h>
#include <D3D10_1.h>
#include <DXGI.h>
#include <D2D1.h>
#include <sstream>
#include <dwrite.h>
#include <dinput.h>
#include <vector>
#include <fstream>
#include <istream>



class Engine :	public wind
{
public:

	Engine(HINSTANCE);
	~Engine();

	bool initialize_engine();
protected:
	IDXGISwapChain* SwapChain;
	ID3D11Device* d3d11Device;
	ID3D11DeviceContext* d3d11DevCon;
	ID3D11Texture2D *BackBuffer11;
	ID3D11DepthStencilView* depthStencilView;
	ID3D11Texture2D* depthStencilBuffer;
	ID3D11RenderTargetView* renderTargetView;
	HRESULT hr;

	ID3D10Blob* VS_Buffer;
	ID3D10Blob* PS_Buffer;
	ID3D11VertexShader* VS;
	ID3D11PixelShader* PS;

	void tick();
	bool initScene();
	void updateScene();
	void drawScene();
};

