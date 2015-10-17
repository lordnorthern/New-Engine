#pragma once
#include "Engine.h"
#include "globaldef.h"

class RenderState
{
public:
	Engine * engine_obj;
	D3D11_RASTERIZER_DESC rastDesc;
	ID3D11RasterizerState* state;
	RenderState(Engine *);
	~RenderState();
	void set();
};

