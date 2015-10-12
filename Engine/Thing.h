#pragma once
#include "globaldef.h"
#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <D3DX10.h>
#include <xnamath.h>
#include "Engine.h"

class Thing
{
public:
	int id;
	Engine * engine_obj;

	Thing(Engine *,int);
	~Thing();

	virtual bool initialize() = 0;
	virtual bool modify(modifier) = 0;
	virtual void update() = 0;
	virtual void manifest()=0;

protected:
	HRESULT hr;
};

