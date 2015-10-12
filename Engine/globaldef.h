#pragma once
#include <string>
#define UNICODE

#define say(X) MessageBox(0, X, X, 0)
#define err_say(X) MessageBox(0, X, L"Error", 0)

struct sp_float2
{
	float a, b;
};

struct sp_float3
{
	float a, b, c;
};

struct sp_float4
{
	float a, b, c, d;
};

struct modifier
{
	std::string name;
	bool b1,b2;
	float f1, f2, f3, f4,f5,f6;
	modifier()
		:f1(NULL), f2(NULL), f3(NULL), f4(NULL), f5(NULL), f6(NULL)
	{
		
	}
};