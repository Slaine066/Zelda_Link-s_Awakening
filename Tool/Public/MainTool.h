#pragma once

#include "Tool_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CRenderer;
class CGameInstance;
END

BEGIN(Tool)
class CMainTool final : public CBase
{
private:
	CMainTool();
	virtual ~CMainTool() = default;

public:
	HRESULT Initialize();
	void Tick(_float fTimeDelta);
	HRESULT Render(_float fTimeDelta);

private:
	CGameInstance*			m_pGameInstance = nullptr;

private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;
	CRenderer*				m_pRenderer = nullptr;

#ifdef _DEBUG
private:
	_uint					m_iNumRender = 0;
	_float					m_fTimeAcc = 0.f;
#endif // _DEBUG

private:
	HRESULT Ready_Prototype_Component();
	HRESULT Ready_Prototype_GameObjects();

	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);

	HRESULT Ready_Lights();

	void Check_Picking();

public:
	static CMainTool* Create();
	virtual void Free();
};
END