#pragma once
#include "Thing.h"
class LightSource :	public Thing
{
public:
	Light light;

	LightSource(Engine *, int);
	virtual ~LightSource();
	bool initialize();
	bool modify(modifier);
	void update();
	void manifest();
};

