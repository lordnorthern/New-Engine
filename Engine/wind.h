#pragma once
#include "globaldef.h"
#include <windows.h>

class wind
{
public:
	HINSTANCE hInstance;
	LPCWCHAR window_class_name;
	LPCWCHAR window_title;
	int window_width;
	int window_height;

	wind(HINSTANCE);
	~wind();

	bool initialize_window();
	int messageloop();

protected:
	HWND hwnd;
	virtual void tick();
};

