#pragma once

#include <process.h>

namespace Tool
{
	const unsigned int g_iWinSizeX = 1280;
	const unsigned int g_iWinSizeY = 720;

	enum LEVEL { LEVEL_TOOL, LEVEL_END }; // This enum is not needed, but I don't like to directly pass "naked" integers to functions.
}

extern HWND			g_hWnd;
extern HINSTANCE	g_hInst;

using namespace Tool;