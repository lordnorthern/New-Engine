#pragma once
#include "Entity.h"
class Ground :
	public Entity
{
public:

	Ground(Engine *, int, LPCWSTR);
	virtual ~Ground();
};

