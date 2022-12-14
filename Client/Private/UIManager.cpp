#include "stdafx.h"

#include "UIManager.h"
#include "GameInstance.h"
#include "UI.h"
#include "Player.h"
#include "UI_Heart.h"

IMPLEMENT_SINGLETON(CUIManager)

CUIManager::CUIManager()
{
}

HRESULT CUIManager::Initialize()
{
	Build_Hearts(); 
	Build_ItemButtonX(); 
	Build_ItemButtonY(); 

	return S_OK;
}

_uint CUIManager::Tick(_float fTimeDelta)
{
	Compute_Hearts();

	return OBJ_NOEVENT;
}

_uint CUIManager::Late_Tick(_float fTimeDelta)
{
	return OBJ_NOEVENT;
}

void CUIManager::Clear()
{
	for (auto& pHeart : m_Hearts)
		Safe_Release(pHeart);

	m_Hearts.clear();
}

HRESULT CUIManager::Build_Hearts()
{
	Get_PlayerHp();

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CUI::UIDESC tUIDesc;
	ZeroMemory(&tUIDesc, sizeof(CUI::UIDESC));

	tUIDesc.m_fSizeX = 55;
	tUIDesc.m_fSizeY = 55;

	for (_uint i = 0; i < m_fMaxHp; i++)
	{
		tUIDesc.m_fX = tUIDesc.m_fSizeX * (i == 0 ? 1 : i + 1);
		tUIDesc.m_fY = tUIDesc.m_fSizeY;
		wcscpy_s(tUIDesc.m_pTextureName, MAX_PATH, TEXT("Prototype_Component_Texture_Heart"));

		CUI* pHeart = nullptr;
		pGameInstance->Add_GameObject_Out(TEXT("UI_Heart"), TEXT("Prototype_GameObject_UI_Heart"), pGameInstance->Get_NextLevelIndex(), TEXT("Layer_UI"), (CGameObject*&)pHeart, &tUIDesc);

		m_Hearts.push_back(pHeart);
		((CUI_Heart*)pHeart)->Set_HeartType(CUI_Heart::HEART_TYPE::HEART_FULL);
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CUIManager::Build_ItemButtonX()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CUI::UIDESC tUIDesc;

	/* Item Background */
	ZeroMemory(&tUIDesc, sizeof(CUI::UIDESC));
	tUIDesc.m_fSizeX = 150;
	tUIDesc.m_fSizeY = 150;
	tUIDesc.m_fX = g_iWinSizeX - 150;
	tUIDesc.m_fY = 100;
	wcscpy_s(tUIDesc.m_pTextureName, MAX_PATH, TEXT("Prototype_Component_Texture_Item_Background"));
	tUIDesc.m_ePass = VTXTEXPASS::PASS_UI_BLEND;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("UI_Test"), TEXT("Prototype_GameObject_UI"), pGameInstance->Get_NextLevelIndex(), TEXT("Layer_UI"), &tUIDesc)))
		return E_FAIL;

	/* Item Button */
	ZeroMemory(&tUIDesc, sizeof(CUI::UIDESC));
	tUIDesc.m_fSizeX = 40;
	tUIDesc.m_fSizeY = 40;
	tUIDesc.m_fX = g_iWinSizeX - 190;
	tUIDesc.m_fY = 140;
	wcscpy_s(tUIDesc.m_pTextureName, MAX_PATH, TEXT("Prototype_Component_Texture_Item_ButtonX"));
	tUIDesc.m_ePass = VTXTEXPASS::PASS_UI_BLEND;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("UI_Test"), TEXT("Prototype_GameObject_UI"), pGameInstance->Get_NextLevelIndex(), TEXT("Layer_UI"), &tUIDesc)))
		return E_FAIL;

	/* Item Number */

	/* Item Image */

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CUIManager::Build_ItemButtonY()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CUI::UIDESC tUIDesc;

	/* Item Background */
	ZeroMemory(&tUIDesc, sizeof(CUI::UIDESC));
	tUIDesc.m_fSizeX = 150;
	tUIDesc.m_fSizeY = 150;
	tUIDesc.m_fX = g_iWinSizeX - 300;
	tUIDesc.m_fY = 130;
	wcscpy_s(tUIDesc.m_pTextureName, MAX_PATH, TEXT("Prototype_Component_Texture_Item_Background"));
	tUIDesc.m_ePass = VTXTEXPASS::PASS_UI_BLEND;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("UI_Test"), TEXT("Prototype_GameObject_UI"), pGameInstance->Get_NextLevelIndex(), TEXT("Layer_UI"), &tUIDesc)))
		return E_FAIL;

	/* Item Button */
	ZeroMemory(&tUIDesc, sizeof(CUI::UIDESC));
	tUIDesc.m_fSizeX = 40;
	tUIDesc.m_fSizeY = 40;
	tUIDesc.m_fX = g_iWinSizeX - 340;
	tUIDesc.m_fY = 170;
	wcscpy_s(tUIDesc.m_pTextureName, MAX_PATH, TEXT("Prototype_Component_Texture_Item_ButtonY"));
	tUIDesc.m_ePass = VTXTEXPASS::PASS_UI_BLEND;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("UI_Test"), TEXT("Prototype_GameObject_UI"), pGameInstance->Get_NextLevelIndex(), TEXT("Layer_UI"), &tUIDesc)))
		return E_FAIL;

	/* Item Number */

	/* Item Image */

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CUIManager::Get_PlayerHp()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CPlayer* pPlayer = (CPlayer*)pGameInstance->Find_Object(pGameInstance->Get_NextLevelIndex(), TEXT("Layer_Player"));
	if (pPlayer)
	{
		m_fMaxHp = pPlayer->Get_Stats().m_fMaxHp;
		m_fCurrentHp = pPlayer->Get_Stats().m_fCurrentHp;
	}
	
	RELEASE_INSTANCE(CGameInstance);
}

void CUIManager::Compute_Hearts()
{
	Get_PlayerHp();

	for (_float i = 0; i < m_fMaxHp; i += 1)
	{
		if (m_fCurrentHp >(i + .5f))
			((CUI_Heart*)m_Hearts[i])->Set_HeartType(CUI_Heart::HEART_TYPE::HEART_FULL);
		else if (m_fCurrentHp == (i + .5f))
			((CUI_Heart*)m_Hearts[i])->Set_HeartType(CUI_Heart::HEART_TYPE::HEART_HALF);
		else
			((CUI_Heart*)m_Hearts[i])->Set_HeartType(CUI_Heart::HEART_TYPE::HEART_EMPTY);
	}
}

void CUIManager::Free()
{
}