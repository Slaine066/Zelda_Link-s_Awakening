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
	HRESULT Render();

private:
	CGameInstance*			m_pGameInstance = nullptr;

private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;
	CRenderer*				m_pRenderer = nullptr;

#ifdef _DEBUG
private:
	_uint					m_iNumRender = 0;
	_tchar					m_szFPS[MAX_PATH] = TEXT("");
	_float					m_fTimeAcc = 0.f;
#endif // _DEBUG

private:
	HRESULT Ready_Prototype_Component();

public:
	static CMainTool* Create();
	virtual void Free();
};
END