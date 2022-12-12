#pragma once

#include <process.h>

namespace Client
{
	const unsigned int g_iWinSizeX = 1920;
	const unsigned int g_iWinSizeY = 1080;

	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_FIELD, LEVEL_MORIBLINCAVE, LEVEL_BOTTLEGROTTO, LEVEL_END };
	enum DIRID { DIR_STRAIGHT, DIR_BACKWARD, DIR_LEFT, DIR_RIGHT, DIR_STRAIGHT_LEFT, DIR_STRAIGHT_RIGHT, DIR_BACKWARD_LEFT, DIR_BACKWARD_RIGHT, DIR_END };

	enum VTXTEXPASS { PASS_DEFAULT, PASS_UI, PASS_UI_BLEND, PASS_END };
}

extern HWND			g_hWnd;
extern HINSTANCE	g_hInst;

using namespace Client;