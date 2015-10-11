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
	ID3D11Buffer* vertex_buffer;
	ID3D11Buffer* index_buffer;

	ID3D11InputLayout* vertLayout;

	Entity(Engine *);
	~Entity();

	virtual bool initialize();
	virtual bool modify(modifier);
	virtual void update();
	virtual void manifest();

protected:

};

