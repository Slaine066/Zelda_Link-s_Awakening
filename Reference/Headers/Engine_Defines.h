#pragma once

#pragma warning (disable : 4005)
#pragma warning (disable : 4251)

#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <Effects11/d3dx11effect.h>
#include <DirectXCollision.h>

#include <DirectXTK/DDSTextureLoader.h>
#include <DirectXTK/WICTextureLoader.h>
#include <DirectXTK/ScreenGrab.h>
#include <directXTK/SpriteFont.h>
#include <directXTK/SpriteBatch.h>
#include <DirectXTK/VertexTypes.h>
#include <DirectXTK/PrimitiveBatch.h>
#include <DirectXTK/Effects.h>

#include <assimp/scene.h>
#include <assimp/importer.hpp>
#include <assimp/postprocess.h>
#include <d3dcompiler.h>


using namespace DirectX;

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

enum DIMK { DIMK_LBUTTON, DIMK_RBUTTON, DIMK_WHEEL, DIMK_END };
enum DIMM { DIMM_X, DIMM_Y, DIMM_WHEEL, DIMM_END };

#include <vector>
#include <map>
#include <list>
#include <unordered_set>
#include <algorithm>
#include <typeinfo.h>
using namespace std;


#include "Engine_Macro.h"
#include "Engine_Function.h"
#include "Engine_Struct.h"
#include "Engine_Typedef.h"


#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif

#endif // _DEBUG

using namespace Engine;

#define OBJ_DEAD	1
#define OBJ_NOEVENT 0
#define	VK_MAX		0xff