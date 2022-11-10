#pragma once

#include <process.h>

#include "Engine_Defines.h"

namespace Tool
{
	const unsigned int g_iWinSizeX = 1280;
	const unsigned int g_iWinSizeY = 720;

	enum LEVEL { LEVEL_TOOL, LEVEL_END };
	enum TOOL_OBJID { OBJ_MESH, OBJ_END };
}

extern HWND			g_hWnd;
extern HINSTANCE	g_hInst;

using namespace Tool;