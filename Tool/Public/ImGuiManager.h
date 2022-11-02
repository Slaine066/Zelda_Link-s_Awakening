#pragma once

#include "Tool_Defines.h"
#include "Base.h"
#include "GameObject.h"
#include "Transform.h"
#include "Mesh.h"

BEGIN(Tool)
class CImGuiManager final : public CBase
{
	DECLARE_SINGLETON(CImGuiManager)

public:
	enum TRANSFORM_TYPE { TRANS_TRANSLATION, TRANS_ROTATION, TRANS_SCALE };

private:
	CImGuiManager();
	virtual ~CImGuiManager() = default;

public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Render(_float fTimeDelta);

private:
	struct ImVec3 { float x, y, z; ImVec3(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) { x = _x; y = _y; z = _z; } };

	void ImGui_SetupCustomStyle();
	void ApplyStyle(ImVec3 color_for_text, ImVec3 color_for_head, ImVec3 color_for_area, ImVec3 color_for_body, ImVec3 color_for_pops);

	void DrawEditor(_float fTimeDelta);
	void DrawOverlayWindow();

	void DrawMapTool(_float fTimeDelta);
	void DrawUITool();
	void DrawCamTool();

	void DrawMapModals();
	//void DrawUIModals();
	//void DrawCamModals();

	void Read_Maps_Name();
	void Read_NonAnimModels_Name(_tchar* cFolderPath);
	void Read_AnimModels_Name(_tchar* cFolderPath);

	void Create_Object();

	_bool SaveData();
	_bool LoadData();

private:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;

public:
	void Set_FramesPerSecond(_uint iFrames) { m_iFramesPerSecond = iFrames; }
	void Set_PickedPosition(_float3 vPickedPosition) { m_vPositionPicked = vPickedPosition; }

private:
	_uint m_iFramesPerSecond = 0;
	
	// EDITOR SETTINGS
	_float m_fEditorAlpha = .9f;
	 
	// MAP TOOL
	//	Maps
	int m_iXVertex = 100;
	int m_iZVertex = 100;

	vector<string> m_vMaps;
	_uint m_iSelectedMap = 0;
	string m_sCurrentMap = "";

	_float3 m_vPositionPicked = _float3(0.f, 0.f, 0.f);

	//	Objects	
	list<CMesh::MODELDESC> m_lObjects; 
	string m_sSelectedObject = "";

	// Layers
	unordered_set<string> m_vLayers;					
	unordered_set<wstring> m_vLayersTemp;				// Storing Layers so that they don't get de-allocated when passing them to Engine
	string m_sCurrentLayer = "";

	// Created Objects
	list<wstring> m_lModelPrototypeIds;					// Storing Model Prototypes so that they don't get de-allocated when passing them to Engine
	CGameObject* m_pSelectedCreatedObject = nullptr;
	
	// Created Objects Transformation
	TRANSFORM_TYPE m_eObjAction = TRANS_TRANSLATION;
	CTransform* m_pTransform = nullptr;
	_float m_fX = 1.f, m_fY = 1.f, m_fZ = 1.f;

	// UI TOOL
	// TODO: ..

	// CAM TOOL
	// TODO: ..

public:
	virtual void Free() override;
};
END