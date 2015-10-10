#pragma once
#define UNICODE

#define say(X) MessageBox(0, X, X, 0)
#define err_say(X) MessageBox(hwnd, X, L"Error", 0)

struct sp_float2
{
	float a, b;
};