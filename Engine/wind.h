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

	double countsPerSecond = 0.0;
	__int64 CounterStart = 0;

	int frameCount = 0;
	int fps = 0;

	__int64 frameTimeOld = 0;
	double frameTime;


	wind(HINSTANCE);
	~wind();

	bool initialize_window();
	int messageloop();

protected:
	HWND hwnd;
	virtual void tick();
};