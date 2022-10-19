#pragma once

#include "Tool_Defines.h"
#include "Base.h"

class CImGuiManager final : public CBase
{
	DECLARE_SINGLETON(CImGuiManager)

private:
	CImGuiManager();
	virtual ~CImGuiManager() = default;

public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Render();

private:
	void DrawEditor();

	void DrawMapTool();
	void DrawUITool();
	void DrawCamTool();

	void DrawMapModals();
	//void DrawUIModals();
	//void DrawCamModals();

private:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;

private:
	// Editor Alpha
	_float m_fEditorAlpha = .9f;
	 
	// Map Tool
	vector<string> m_vMaps = { "First Map", "Second Map", "Third Map", "Fourth Map", "Fifth Map" };
	_uint m_iSelectedMap;

	// Object Tool
	_float3 m_vPositionPicked = _float3(0.f, 0.f, 0.f);
	vector<string> m_vMeshes = { "SM_Tree", "SM_House", "SM_Pot", "SM_Chest", "SM_Monster_1", "SM_Monster_2" };
	_uint m_iSelectedMesh;

	// Cam Tool

public:
	virtual void Free() override;
};