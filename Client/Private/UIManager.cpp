#include "stdafx.h"

#include "UIManager.h"
#include "GameInstance.h"
#include "Player.h"
#include "UI_Heart.h"

IMPLEMENT_SINGLETON(CUIManager)

CUIManager::CUIManager()
{
}

HRESULT CUIManager::Initialize()
{
	m_eMode = MODE::MODE_GAME;

	Build_Hearts(); 
	Build_ItemButtonX(); 
	Build_ItemButtonY(); 

	return S_OK;
}

void CUIManager::Handle_Input()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	switch (m_eMode)
	{
		case MODE::MODE_GAME:
		{
			if (pGameInstance->Key_Down('M'))
			{
				/* Open Map */
				m_eMode = MODE::MODE_MAP;
			}
			else if (pGameInstance->Key_Down('N'))
			{
				/* Open Inventory */
				m_eMode = MODE::MODE_INVENTORY;
			}
		}
		break;
		case MODE::MODE_MAP:
		{
			if (pGameInstance->Key_Down('M'))
			{
				/* Nothing happens. Already in Map Mode. */
			}
			else if (pGameInstance->Key_Down('N'))
			{
				/* Switch to Inventory Mode. */
				m_eMode = MODE::MODE_INVENTORY;
			}
			else if (pGameInstance->Key_Down('S'))
			{
				/* Close Map. */
				m_eMode = MODE::MODE_GAME;
			}
		}
		break;
		case MODE::MODE_INVENTORY:
		{
			if (pGameInstance->Key_Down('M'))
			{
				/* Switch to Map Mode. */
				m_eMode = MODE::MODE_MAP;
			}
			else if (pGameInstance->Key_Down('N'))
			{
				/* Nothing happens. Already in Inventory Mode. */
			}
			else if (pGameInstance->Key_Down('S'))
			{
				/* Close Inventory. */
				m_eMode = MODE::MODE_INVENTORY;
			}
		}
		break;
	}

	RELEASE_INSTANCE(CGameInstance);
}

_uint CUIManager::Tick(_float fTimeDelta)
{
	if (m_eMode == MODE::MODE_END)
		return OBJ_NOEVENT;

	Handle_Input();
	Compute_Hearts();

	return OBJ_NOEVENT;
}

void CUIManager::Clear()
{
	m_Hearts.clear();
	m_fMaxHp = 0.f;
	m_fCurrentHp = 0.f;
	m_eMode = MODE::MODE_END;
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

		CUI_Heart* pHeart = nullptr;
		pGameInstance->Add_GameObject_Out(TEXT("UI_Heart"), TEXT("Prototype_GameObject_UI_Heart"), pGameInstance->Get_NextLevelIndex(), TEXT("Layer_UI"), (CGameObject*&)pHeart, &tUIDesc);

		m_Hearts.push_back(pHeart);
		pHeart->Set_HeartType(CUI_Heart::HEART_TYPE::HEART_FULL);
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
		if (m_fCurrentHp > (i + .5f))
			m_Hearts[i]->Set_HeartType(CUI_Heart::HEART_TYPE::HEART_FULL);
		else if (m_fCurrentHp == (i + .5f))
			m_Hearts[i]->Set_HeartType(CUI_Heart::HEART_TYPE::HEART_HALF);
		else
			m_Hearts[i]->Set_HeartType(CUI_Heart::HEART_TYPE::HEART_EMPTY);
	}
}

void CUIManager::Free()
{
}