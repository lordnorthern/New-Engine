#pragma once
#define UNICODE

#define say(X) MessageBox(0, X, X, 0)
#define err_say(X) MessageBox(0, X, L"Error", 0)

struct sp_float2
{
	float a, b;
};

struct modifier
{
	float f1, f2, f3, f4, f5, f6;
	bool b1, b2, b3;
};