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
#include <dinput.h>

class RenderState;

struct cbPerObject
{
	XMMATRIX  WVP;
	XMMATRIX  World;
};

struct Light
{
	Light()
	{
		ZeroMemory(this, sizeof(Light));
	}
	XMFLOAT3 dir;
	float pad;
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
};

struct cbPerFrame
{
	Light  light;
};

class Thing;

class Engine :	public wind
{
public:
	float rot;

	ID3D11Device* d3d11Device;
	ID3D11DeviceContext* d3d11DevCon;
	ID3D10Blob* VS_Buffer;
	ID3D10Blob* PS_Buffer;
	ID3D11VertexShader* VS;
	ID3D11PixelShader* PS;
	ID3D11DepthStencilView* depthStencilView;
	ID3D11Texture2D* depthStencilBuffer;
	ID3D11Buffer* cbPerObjectBuffer;
	
	cbPerFrame constbuffPerFrame;

	XMMATRIX WVP;
	XMMATRIX World;
	XMMATRIX camView;
	XMMATRIX camProjection;
	XMVECTOR camPosition;
	XMVECTOR camTarget;
	XMVECTOR camUp;
	modifier mod;
	cbPerObject cbPerObj;
	ID3D11Buffer* cbPerFrameBuffer;
	ID3D11PixelShader* D2D_PS;
	ID3D10Blob* D2D_PS_Buffer;


	std::map<std::string, RenderState*> render_states;
	IDirectInputDevice8* DIKeyboard;
	IDirectInputDevice8* DIMouse;
	Engine(HINSTANCE);
	~Engine();

	bool initialize_engine();
protected:
	IDXGISwapChain* SwapChain;
	ID3D11Texture2D *BackBuffer11;
	ID3D11RenderTargetView* renderTargetView;
	HRESULT hr;
	std::vector<Thing *> Things;


	DIMOUSESTATE mouseLastState;
	LPDIRECTINPUT8 DirectInput;

	XMVECTOR DefaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	XMVECTOR DefaultRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR camForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	XMVECTOR camRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR DefaultUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR camUp2 = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX camRotationMatrix;
	XMMATRIX groundWorld;

	float moveLeftRight = 0.0f;
	float moveBackForward = 0.0f;
	float moveUpDown = 0.0f;

	float camYaw = 0.0f;
	float camPitch = 0.0f;

	float rotx = 0;
	float rotz = 0;
	float scaleX = 1.0f;
	float scaleY = 1.0f;

	XMMATRIX Rotationx;
	XMMATRIX Rotationz;

	void tick();
	bool initScene();
	void updateScene();
	void drawScene();
	void UpdateCamera();

	void StartTimer();
	double GetTime();
	double GetFrameTime();

	bool InitDirectInput(HINSTANCE);
	void DetectInput();
private:
	void initializeRenderState();
};

