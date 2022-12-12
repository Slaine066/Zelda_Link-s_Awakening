#include "stdafx.h"

#include "UIManager.h"
#include "GameInstance.h"
#include "UI.h"

IMPLEMENT_SINGLETON(CUIManager)

CUIManager::CUIManager()
{
}

HRESULT CUIManager::Initialize_UI()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/* UI_HpBar */
	if (FAILED(pGameInstance->Add_GameObject(TEXT("UI_HpBar"), TEXT("Prototype_GameObject_UI_HpBar"), pGameInstance->Get_NextLevelIndex(), TEXT("Layer_UI"))))
		return E_FAIL;

	/* UI_HpBar */
	CUI::UIDESC tUIDesc;
	ZeroMemory(&tUIDesc, sizeof(CUI::UIDESC));

	tUIDesc.m_fSizeX = 150;
	tUIDesc.m_fSizeY = 150;
	tUIDesc.m_fX = g_iWinSizeX - 150;
	tUIDesc.m_fY = 150;
	wcscpy_s(tUIDesc.m_pTextureName, MAX_PATH, TEXT("Prototype_Component_Texture_Item_Background"));
	tUIDesc.m_ePass = VTXTEXPASS::PASS_UI_BLEND;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("UI_Test"), TEXT("Prototype_GameObject_UI"), pGameInstance->Get_NextLevelIndex(), TEXT("Layer_UI"), &tUIDesc)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
}

void CUIManager::Free()
{
}