#pragma once
#include "Thing.h"

struct Vertex
{
	Vertex() {}
	Vertex(float x, float y, float z)
		: pos(x, y, z) {}

	XMFLOAT3 pos;
};


class Entity :	public Thing
{
public:
	ID3D11Buffer* triangleVertBuffer;
	ID3D11VertexShader* VS;
	ID3D11PixelShader* PS;
	ID3D10Blob* VS_Buffer;
	ID3D10Blob* PS_Buffer;
	ID3D11InputLayout* vertLayout;

	Entity(Engine *);
	~Entity();

	virtual bool initialize();
	virtual bool modify(modifier);
	virtual void update();
	virtual void manifest();

protected:

};

