#pragma once

#include "Thing.h"


struct Vertex	//Overloaded Vertex Structure
{
	Vertex() {}
	Vertex(float x, float y, float z,
		float u, float v)
		: pos(x, y, z), texCoord(u, v) {}

	XMFLOAT3 pos;
	XMFLOAT2 texCoord;
};

class Entity :	public Thing
{
public:
	LPCWSTR texture_name;
	std::string render_state;
	Entity(Engine *,int,LPCWSTR);
	virtual ~Entity();

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

	sp_float3 position_pos;
	sp_float3 scale_pos;
	sp_float5 rotation_pos;
	ID3D11ShaderResourceView* texture;
	ID3D11SamplerState* sampler_state;

};

