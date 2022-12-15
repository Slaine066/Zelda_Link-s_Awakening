#include "stdafx.h"

#include "MainApp.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Collider.h"
#include "CameraManager.h"
#include "UI_Manager.h"

CMainApp::CMainApp() : 
	m_pGameInstance(CGameInstance::Get_Instance()),
	m_pUIManager(CUI_Manager::Get_Instance())
{
	/*D3D11_SAMPLER_DESC*/
	
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pUIManager);
}

HRESULT CMainApp::Initialize()
{
	srand(unsigned int(time(NULL)));

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

	if (FAILED(Open_Level(LEVEL_LOGO)))
		return E_FAIL; 

	// MakeSpriteFont "폰트이름" /FontSize:32 /FastPack /CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD800 /DefaultCharacter:0xAC00 출력파일이름.spritefont
	if (FAILED(m_pGameInstance->Add_Fonts(m_pDevice, m_pContext, TEXT("Quicksand-24"), TEXT("../../Resources/Fonts/Quicksand-24.spritefont"))))
		return E_FAIL;

	// How to set RenderStates with code (currently setting them in Shaders)
	/*
	m_pDevice->CreateDepthStencilState();	// Takes a D3D11_DEPTH_STENCIL_DESC
	m_pContext->OMSetDepthStencilState();

	m_pDevice->CreateBlendState();			// Takes a D3D11_BLEND_DESC
	m_pContext->OMSetBlendState();
	
	m_pDevice->CreateRasterizerState()		// Takes a D3D11_RASTERIZER_DESC
	m_pContext->RSSetState();
	*/

	return S_OK;
}

void CMainApp::Tick(_float fTimeDelta)
{
	if (!m_pGameInstance)
		return;

	m_pUIManager->Tick(fTimeDelta);
	m_pGameInstance->Tick_Engine(fTimeDelta);

#ifdef _DEBUG
	m_fTimeAcc += fTimeDelta;
#endif // _DEBUG
}

HRESULT CMainApp::Render()
{
	m_pGameInstance->Clear_BackBuffer_View(_float4(0.32f, 0.4f, 0.47f, 1.f));
	m_pGameInstance->Clear_DepthStencil_View();

#ifdef _DEBUG
	m_pRenderer->Add_Debug(m_pGameInstance->Get_NavigationMesh());

	list<CTriggerBox*> TriggerBoxes = m_pGameInstance->Get_TriggerBoxes();
	for (auto& pTriggerBox : TriggerBoxes)
		m_pRenderer->Add_Debug((CGameObject*)pTriggerBox);
#endif // _DEBUG

	m_pRenderer->Render_GameObjects();

#ifdef _DEBUG
	Show_FPS();
#endif // _DEBUG

	m_pGameInstance->Present();

	return S_OK;
}

HRESULT CMainApp::Open_Level(LEVEL eLevel)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	CLevel_Loading* pLevel_Loading = CLevel_Loading::Create(m_pDevice, m_pContext, eLevel);
	if (nullptr == pLevel_Loading)
		return E_FAIL;

	m_pGameInstance->Open_Level(LEVEL_LOADING, pLevel_Loading);

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Component()
{
	if (!m_pGameInstance)
		return E_FAIL;

	/* For.Prototype_Component_Renderer */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), m_pRenderer = CRenderer::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	Safe_AddRef(m_pRenderer);

	return S_OK;
}

void CMainApp::Show_FPS()
{
	++m_iNumRender;

	if (m_fTimeAcc > 1.0f)
	{
		wsprintf(m_szFPS, TEXT("FPS: %d"), m_iNumRender);

		m_fTimeAcc = 0.f;
		m_iNumRender = 0;
	}

	m_pGameInstance->Render_Font(TEXT("Quicksand-24"), m_szFPS, XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
}

CMainApp * CMainApp::Create()
{
	CMainApp* pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		ERR_MSG(TEXT("Failed to Create: CMainApp"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainApp::Free()
{
	Safe_Release(m_pRenderer);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pUIManager);
	
	CGameInstance::Release_Engine();

	CCameraManager::Get_Instance()->Destroy_Instance();
	CUI_Manager::Get_Instance()->Destroy_Instance();
}