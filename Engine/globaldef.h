#pragma once
#include <string>
#include <map>
#define UNICODE

#define say(X) MessageBox(0, X, X, 0)
#define err_say(X) MessageBox(0, X, L"Error", 0)

struct sp_float2
{
	void refresh()
	{
		a = 0.0f; b = 0.0f;
	}
	float a, b;
};

struct sp_float3
{
	void refresh()
	{
		a = 0.0f; b = 0.0f; c = 0.0f;
	}
	float a, b, c;
};

struct sp_float4
{
	void refresh()
	{
		a = 0.0f; b = 0.0f; c = 0.0f; d = 0.0f;
	}
	float a, b, c, d;
};

struct sp_float5
{
	void refresh()
	{
		a = 0.0f; b = 0.0f; c = 0.0f; d = 0.0f; e = 0.0f;
	}
	float a, b, c, d, e;
};

struct modifier
{
	std::string name;
	bool b1,b2;
	float f1, f2, f3, f4,f5,f6;
	modifier()
		:f1(0.0f), f2(0.0f), f3(0.0f), f4(0.0f), f5(0.0f), f6(0.0f)
	{
		
	}

	void refresh()
	{
		b1 = false; b2 = false; f1 = 0.0f; f2 = 0.0f; f3= 0.0f; f4 = 0.0f; f5 = 0.0f; f6 = 0.0f;
	}
};