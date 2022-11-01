#pragma once

#include <process.h>

#include "Engine_Defines.h"

namespace Tool
{
	const unsigned int g_iWinSizeX = 1280;
	const unsigned int g_iWinSizeY = 720;

	enum LEVEL { LEVEL_TOOL, LEVEL_END };
	enum TOOL_OBJID { OBJ_MESH, OBJ_END };

	/*
	*	Client Info
	*	Change Structure below in order to be able to use this Tool with your Client Data.
	*/

	// Struct used to Load and Save Data to Files
	//typedef struct tagActorDescription
	//{
	//	_tchar wcObjName[MAX_PATH];				// "Fiona"
	//	_tchar wcObjPrototypeId[MAX_PATH];		// "Prototype_GameObject_Player"
	//	_tchar wcModelPrototypeId[MAX_PATH];	// "Prototype_Component_Model_Fiona"
	//	_tchar wcObjLayerTag[MAX_PATH];			// "Layer_Player"
	//	_float4x4 mObjPivotMatrix;				// Represents the World Matrix
	//}ACTORDESC;
}

extern HWND			g_hWnd;
extern HINSTANCE	g_hInst;

using namespace Tool;