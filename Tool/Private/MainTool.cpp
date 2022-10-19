#include "stdafx.h"

#include "MainTool.h"
#include "GameInstance.h"

CMainTool::CMainTool()
	: m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainTool::Initialize()
{
	GRAPHIC_DESC Graphic_Desc;
	ZeroMemory(&Graphic_Desc, sizeof(GRAPHIC_DESC));

	Graphic_Desc.hWnd = g_hWnd;
	Graphic_Desc.iWinSizeX = g_iWinSizeX;
	Graphic_Desc.iWinSizeY = g_iWinSizeY;
	Graphic_Desc.eWinMode = GRAPHIC_DESC::MODE_WIN;

	if (FAILED(m_pGameInstance->Initialize_Engine(g_hInst, LEVEL_END, Graphic_Desc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Component()))
		return E_FAIL;

	return S_OK;
}

void CMainTool::Tick(_float fTimeDelta)
{
	if (nullptr == m_pGameInstance)
		return;

	m_pGameInstance->Tick_Engine(fTimeDelta);

#ifdef _DEBUG
	m_fTimeAcc += fTimeDelta;
#endif // _DEBUG

}

HRESULT CMainTool::Render()
{
	m_pGameInstance->Clear_BackBuffer_View(_float4(0.f, 0.f, 1.f, 1.f));
	m_pGameInstance->Clear_DepthStencil_View();

	m_pRenderer->Render_GameObjects();

	++m_iNumRender;

	if (m_fTimeAcc > 1.0f)
	{
		wsprintf(m_szFPS, TEXT("FPS: %d"), m_iNumRender);
		OutputDebugString(m_szFPS);

		m_fTimeAcc = 0.f;
		m_iNumRender = 0;
	}

	m_pGameInstance->Present();

	return S_OK;
}

HRESULT CMainTool::Ready_Prototype_Component()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	/* For.Prototype_Component_Renderer */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Renderer"), m_pRenderer = CRenderer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Rect"), CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Transform */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Transform"), CTransform::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxTex*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../ShaderFiles/Shader_VtxTex.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	Safe_AddRef(m_pRenderer);

	return S_OK;
}

CMainTool * CMainTool::Create()
{
	CMainTool* pInstance = new CMainTool();

	if (FAILED(pInstance->Initialize()))
	{
		ERR_MSG(TEXT("Failed to Created : CMainTool"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainTool::Free()
{
	Safe_Release(m_pRenderer);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);

	CGameInstance::Release_Engine();
}