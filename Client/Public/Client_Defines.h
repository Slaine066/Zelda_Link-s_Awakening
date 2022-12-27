#pragma once

#include <process.h>

namespace Client
{
	const unsigned int g_iWinSizeX = 1920;
	const unsigned int g_iWinSizeY = 1080;

	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_FIELD, LEVEL_MORIBLINCAVE, LEVEL_BOTTLEGROTTO, LEVEL_END };

	enum DIRID { DIR_STRAIGHT, DIR_BACKWARD, DIR_LEFT, DIR_RIGHT, DIR_STRAIGHT_LEFT, DIR_STRAIGHT_RIGHT, DIR_BACKWARD_LEFT, DIR_BACKWARD_RIGHT, DIR_END };

	enum INTERACTABLEID { INTERACTABLE_TREASURE, INTERACTABLE_END };

	enum ITEMID { ITEM_RUPEE_GREEN, ITEM_BOMB, ITEM_ROCFEATHER, ITEM_BOW, ITEM_END };

	/* Shader Pass Enums. */
	enum VTXANIMMODELPASS { VTXANIMMODEL_DEFAULT, VTXANIMMODEL_HIT, VTXANIMMODEL_DISSOLVE, VTXANIMMODEL_DISSOLVE_BOSS, VTXANIMMODEL_END };
	enum VTXMODELPASS { VTXMODEL_DEFAULT, VTXMODEL_HITRING, VTXMODEL_SWISH, VTXMODEL_SWORDSLASH, VTXMODEL_HIT, VTXMODEL_GUARDRING, VTXMODEL_GUARD, VTXMODEL_SHOCKWAVERING, VTXMODEL_SHOCKWAVE, VTXMODEL_STAR, VTXMODEL_END };
	enum VTXTEXPASS { VTXTEX_DEFAULT, VTXTEX_UI, VTXTEX_UI_BLEND, VTXTEX_EFFECT_SMOKE, VTXTEX_EFFECT_HIT_FLASH, VTXTEX_END };
}

extern HWND			g_hWnd;
extern HINSTANCE	g_hInst;

using namespace Client;