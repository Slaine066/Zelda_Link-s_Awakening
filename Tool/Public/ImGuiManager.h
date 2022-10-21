#pragma once

#include "Tool_Defines.h"
#include "Base.h"

class CImGuiManager final : public CBase
{
	DECLARE_SINGLETON(CImGuiManager)

	struct ImVec3 { float x, y, z; ImVec3(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) { x = _x; y = _y; z = _z; } };

private:
	CImGuiManager();
	virtual ~CImGuiManager() = default;

public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Render();

private:
	void ImGui_SetupCustomStyle();
	void ApplyStyle(ImVec3 color_for_text, ImVec3 color_for_head, ImVec3 color_for_area, ImVec3 color_for_body, ImVec3 color_for_pops);

	void DrawEditor();
	void DrawOverlayWindow();

	void DrawMapTool();
	void DrawUITool();
	void DrawCamTool();

	void DrawMapModals();
	//void DrawUIModals();
	//void DrawCamModals();

private:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;

public:
	void Set_PickedPosition(_float3 vPickedPosition) { m_vPositionPicked = vPickedPosition; }

private:
	// EDITOR SETTINGS
	_float m_fEditorAlpha = .9f;
	 
	// MAP TOOL
	// Map
	int m_iXVertex = 100;
	int m_iZVertex = 100;

	vector<string> m_vMaps = { "Field.dat", "Dungeon_1.dat", "Dungeon_2.dat", "Dungeon_3.dat" }; // TODO: Should Load from Files
	_uint m_iSelectedMap = 0;

	// Object
	enum OBJ_ACTION { OBJ_TRANSLATION, OBJ_ROTATION, OBJ_SCALE };
	OBJ_ACTION m_eObjAction = OBJ_TRANSLATION;

	_float3 m_vPositionPicked = _float3(0.f, 0.f, 0.f);

	vector<string> m_vObjects = { "SM_Tree", "SM_House", "SM_Pot", "SM_Chest", "SM_Monster_1", "SM_Monster_2" }; // TODO: Should Load from Files
	_uint m_iSelectedObject = 0;

	vector<string> m_vCreatedObjects = { "Tree_1", "Tree_2", "Chest_1" }; // TODO: Should Load from Files
	_uint m_iSelectedCreatedObject = 0;

	// UI TOOL
	// TODO: ..

	// CAM TOOL
	// TODO: ..

public:
	virtual void Free() override;
};