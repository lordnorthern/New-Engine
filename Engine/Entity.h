#pragma once
#include "Thing.h"

struct Vertex	//Overloaded Vertex Structure
{
	Vertex() {}
	Vertex(float x, float y, float z,
		float cr, float cg, float cb, float ca)
		: pos(x, y, z), color(cr, cg, cb, ca) {}

	XMFLOAT3 pos;
	XMFLOAT4 color;
};

class Entity :	public Thing
{
public:
	Entity(Engine *,int);
	~Entity();

	bool initialize();
	bool modify(modifier);
	void update();
	void manifest();

protected:
	ID3D11Buffer* vertex_buffer;
	ID3D11Buffer* index_buffer;
	XMMATRIX world_matrix;
	XMMATRIX Rotation;
	XMMATRIX Scale;
	XMMATRIX Translation;
	ID3D11InputLayout* vertLayout;

	sp_float3 position;
	sp_float3 scale;
};

