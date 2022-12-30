#include "stdafx.h"

#include "UI_Manager.h"
#include "GameInstance.h"
#include "Player.h"
#include "UI_Heart.h"
#include "UI_ItemSlot.h"
#include "UI_ItemChip.h"
#include "UI_InventoryItem.h"

IMPLEMENT_SINGLETON(CUI_Manager)

CUI_Manager::CUI_Manager() : m_pInventory(CInventory::Get_Instance())
{
	Safe_AddRef(m_pInventory);
}

HRESULT CUI_Manager::Initialize()
{
	m_eMode = MODE::MODE_GAME;

	if (m_bIsLoaded)
		return S_OK;

	m_ItemIcons.resize(12);

	Build_Inventory(); /* Need to be executed first, so other UI component will be visible on top of the Inventory UI. */
	Build_GameItemSlots();
	Build_Hearts();
	Build_Rupees();

	m_bIsLoaded = true;

	return S_OK;
}

void CUI_Manager::Handle_Input()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	switch (m_eMode)
	{
		case MODE::MODE_GAME:
		{
			if (pGameInstance->Key_Down('N'))
			{
				/* Open Inventory */
				m_eMode = MODE::MODE_INVENTORY;

				m_pGameSlotX->Set_SlotType(CUI_ItemSlot::SLOT_TYPE::SLOT_GAME_X_INVENTORY);
				m_pGameSlotY->Set_SlotType(CUI_ItemSlot::SLOT_TYPE::SLOT_GAME_Y_INVENTORY);
			}
		}
		break;
		case MODE::MODE_INVENTORY:
		{
			if (pGameInstance->Key_Down('S'))
			{
				/* Close Inventory. */
				m_eMode = MODE::MODE_GAME;
				m_pGameSlotX->Set_SlotType(CUI_ItemSlot::SLOT_TYPE::SLOT_GAME_X);
				m_pGameSlotY->Set_SlotType(CUI_ItemSlot::SLOT_TYPE::SLOT_GAME_Y);

				m_iCurrentSlotIndex = 0;
			}
			else if (pGameInstance->Key_Down(VK_RIGHT))
			{
				if (m_iCurrentSlotIndex < m_ItemSlots.size() - 1)
					m_iCurrentSlotIndex += 1;
			}
			else if (pGameInstance->Key_Down(VK_LEFT))
			{
				if (m_iCurrentSlotIndex > 0)
					m_iCurrentSlotIndex -= 1;
			}
			else if (pGameInstance->Key_Down(VK_UP))
			{
				if (m_iCurrentSlotIndex - 4 < 0)
					m_iCurrentSlotIndex = 0;
				else
					m_iCurrentSlotIndex -= 4;
			}
			else if (pGameInstance->Key_Down(VK_DOWN))
			{
				if (m_iCurrentSlotIndex + 4 > m_ItemSlots.size() - 1)
					m_iCurrentSlotIndex = m_ItemSlots.size() - 1;
				else
					m_iCurrentSlotIndex += 4;
			}
			else if (pGameInstance->Key_Down('Z'))
			{
				m_ItemSlots[m_pInventory->Get_IndexItemX()]->Set_SlotType(CUI_ItemSlot::SLOT_TYPE::SLOT_INVENTORY);
				m_ItemSlots[m_iCurrentSlotIndex]->Set_SlotType(CUI_ItemSlot::SLOT_TYPE::SLOT_INVENTORY_X);

				if (m_pInventory->Get_IndexItemY() == m_iCurrentSlotIndex)
				{
					m_ItemSlots[m_pInventory->Get_IndexItemX()]->Set_SlotType(CUI_ItemSlot::SLOT_TYPE::SLOT_INVENTORY_Y);
					m_pInventory->Set_IndexItemY(m_pInventory->Get_IndexItemX());
				}
				m_pInventory->Set_IndexItemX(m_iCurrentSlotIndex);

				Add_ItemX();
				Add_ItemY();
			}
			else if (pGameInstance->Key_Down('X'))
			{
				m_ItemSlots[m_pInventory->Get_IndexItemY()]->Set_SlotType(CUI_ItemSlot::SLOT_TYPE::SLOT_INVENTORY);
				m_ItemSlots[m_iCurrentSlotIndex]->Set_SlotType(CUI_ItemSlot::SLOT_TYPE::SLOT_INVENTORY_Y);

				if (m_pInventory->Get_IndexItemX() == m_iCurrentSlotIndex)
				{
					m_ItemSlots[m_pInventory->Get_IndexItemY()]->Set_SlotType(CUI_ItemSlot::SLOT_TYPE::SLOT_INVENTORY_X);
					m_pInventory->Set_IndexItemX(m_pInventory->Get_IndexItemY());
				}
				m_pInventory->Set_IndexItemY(m_iCurrentSlotIndex);

				Add_ItemX();
				Add_ItemY();
			}
		}
		break;
	}

	RELEASE_INSTANCE(CGameInstance);
}

_uint CUI_Manager::Tick(_float fTimeDelta)
{
	if (m_eMode == MODE::MODE_END)
		return OBJ_NOEVENT;

	Handle_Input();
	Compute_Hearts();
	Compute_Rupees();
	Compute_Inventory();

	return OBJ_NOEVENT;
}

void CUI_Manager::Clear()
{
	m_eMode = MODE::MODE_END;
}

HRESULT CUI_Manager::Build_Hearts()
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
		pGameInstance->Add_GameObject_Out(TEXT("UI_Heart"), TEXT("Prototype_GameObject_UI_Heart"), LEVEL_STATIC, TEXT("Layer_UI"), (CGameObject*&)pHeart, &tUIDesc);

		m_Hearts.push_back(pHeart);
		pHeart->Set_HeartType(CUI_Heart::HEART_TYPE::HEART_FULL);
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CUI_Manager::Build_Rupees()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CUI::UIDESC tUIDesc;
	ZeroMemory(&tUIDesc, sizeof(CUI::UIDESC));

	tUIDesc.m_fSizeX = 70;
	tUIDesc.m_fSizeY = 70;
	tUIDesc.m_fX = g_iWinSizeX - 225;
	tUIDesc.m_fY = 220;
	tUIDesc.m_ePass = VTXTEXPASS::VTXTEX_UI_BLEND;
	wcscpy_s(tUIDesc.m_pTextureName, MAX_PATH, TEXT("Prototype_Component_Texture_Item_Rupee"));

	pGameInstance->Add_GameObject(TEXT("UI_Rupees"), TEXT("Prototype_GameObject_UI"), LEVEL_STATIC, TEXT("Layer_UI"), &tUIDesc);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CUI_Manager::Build_GameItemSlots()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CUI::UIDESC tUIDesc;

	/* Game Item Slot X */
	ZeroMemory(&tUIDesc, sizeof(CUI::UIDESC));
	tUIDesc.m_fSizeX = 115;
	tUIDesc.m_fSizeY = 115;
	tUIDesc.m_fX = g_iWinSizeX - 115;
	tUIDesc.m_fY = 65;
	tUIDesc.m_ePass = VTXTEXPASS::VTXTEX_UI_BLEND;
	wcscpy_s(tUIDesc.m_pTextureName, MAX_PATH, TEXT("Prototype_Component_Texture_ItemSlot"));

	CUI_ItemSlot* pItemSlot = nullptr;
	if (FAILED(pGameInstance->Add_GameObject_Out(TEXT("ItemSlot_X_Game"), TEXT("Prototype_GameObject_UI_ItemSlot"), LEVEL_STATIC, TEXT("Layer_UI"), (CGameObject*&)pItemSlot, &tUIDesc)))
		return E_FAIL;

	m_pGameSlotX = pItemSlot;
	m_pGameSlotX->Set_SlotType(CUI_ItemSlot::SLOT_TYPE::SLOT_GAME_X);
	m_pGameSlotX->Set_ScreenX(tUIDesc.m_fX);
	m_pGameSlotX->Set_ScreenY(tUIDesc.m_fY);

	/* Game Item Slot Y */
	ZeroMemory(&tUIDesc, sizeof(CUI::UIDESC));
	tUIDesc.m_fSizeX = 115;
	tUIDesc.m_fSizeY = 115;
	tUIDesc.m_fX = g_iWinSizeX - 250;
	tUIDesc.m_fY = 80;
	tUIDesc.m_ePass = VTXTEXPASS::VTXTEX_UI_BLEND;
	wcscpy_s(tUIDesc.m_pTextureName, MAX_PATH, TEXT("Prototype_Component_Texture_ItemSlot"));

	pItemSlot = nullptr;
	if (FAILED(pGameInstance->Add_GameObject_Out(TEXT("ItemSlot_Y_Game"), TEXT("Prototype_GameObject_UI_ItemSlot"), LEVEL_STATIC, TEXT("Layer_UI"), (CGameObject*&)pItemSlot, &tUIDesc)))
		return E_FAIL;

	m_pGameSlotY = pItemSlot;
	m_pGameSlotY->Set_SlotType(CUI_ItemSlot::SLOT_TYPE::SLOT_GAME_Y);
	m_pGameSlotY->Set_ScreenX(tUIDesc.m_fX);
	m_pGameSlotY->Set_ScreenY(tUIDesc.m_fY);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CUI_Manager::Build_Inventory()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CUI::UIDESC tUIDesc;

	/* Inventory */
	ZeroMemory(&tUIDesc, sizeof(CUI::UIDESC));
	tUIDesc.m_fSizeX = g_iWinSizeX;
	tUIDesc.m_fSizeY = g_iWinSizeY;
	tUIDesc.m_fX = g_iWinSizeX / 2;
	tUIDesc.m_fY = g_iWinSizeY / 2;
	tUIDesc.m_ePass = VTXTEXPASS::VTXTEX_UI_BLEND;
	wcscpy_s(tUIDesc.m_pTextureName, MAX_PATH, TEXT("Prototype_Component_Texture_Inventory"));

	if (FAILED(pGameInstance->Add_GameObject(TEXT("UI_Test"), TEXT("Prototype_GameObject_UI_Inventory"), LEVEL_STATIC, TEXT("Layer_UI"), &tUIDesc)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	Build_InventoryItemSlots();

	return S_OK;
}

HRESULT CUI_Manager::Build_InventoryItemSlots()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CUI::UIDESC tUIDesc;
	ZeroMemory(&tUIDesc, sizeof(CUI::UIDESC));

	tUIDesc.m_fSizeX = 125;
	tUIDesc.m_fSizeY = 125;

	for (_uint i = 0; i < 3; i++)
	{
		for (_uint j = 0; j < 4; j++)
		{
			tUIDesc.m_fX = (g_iWinSizeX / 1.65) + 25 * j + tUIDesc.m_fSizeX * j;
			tUIDesc.m_fY = (g_iWinSizeY / 2.7) + 25 * i + tUIDesc.m_fSizeY * i;
			wcscpy_s(tUIDesc.m_pTextureName, MAX_PATH, TEXT("Prototype_Component_Texture_SlotItem"));
			
			CUI_ItemSlot* pItemSlot = nullptr;
			pGameInstance->Add_GameObject_Out(TEXT("UI_ItemSlot"), TEXT("Prototype_GameObject_UI_ItemSlot"), LEVEL_STATIC, TEXT("Layer_UI"), (CGameObject*&)pItemSlot, &tUIDesc);

			pItemSlot->Set_ScreenX(tUIDesc.m_fX);
			pItemSlot->Set_ScreenY(tUIDesc.m_fY);

			m_ItemSlots.push_back(pItemSlot);

			if (i == 0)
			{
				switch (j)
				{
				case 0:
					pItemSlot->Set_SlotType(CUI_ItemSlot::SLOT_TYPE::SLOT_INVENTORY_X);
					break;
				case 1:
					pItemSlot->Set_SlotType(CUI_ItemSlot::SLOT_TYPE::SLOT_INVENTORY_Y);
					break;
				default:
					pItemSlot->Set_SlotType(CUI_ItemSlot::SLOT_TYPE::SLOT_INVENTORY);
				}
			}
			else
				pItemSlot->Set_SlotType(CUI_ItemSlot::SLOT_TYPE::SLOT_INVENTORY);
		}
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CUI_Manager::Get_PlayerHp()
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

void CUI_Manager::Compute_Hearts()
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

void CUI_Manager::Compute_Rupees()
{
	_uint iRupees = m_pInventory->Get_Rupees();
	wsprintf(m_szRupees, TEXT("%d"), iRupees);
}

void CUI_Manager::Compute_Inventory()
{
	for (auto& pItemSlot : m_ItemSlots)
		pItemSlot->Set_IsHovered(false);

	m_ItemSlots[m_iCurrentSlotIndex]->Set_IsHovered(true);
}

void CUI_Manager::Render_Rupees()
{
	if (m_eMode != MODE::MODE_GAME && m_eMode != MODE::MODE_INVENTORY)
		return;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	pGameInstance->Render_Font(TEXT("Quicksand-24"), m_szRupees, XMVectorSet(1720.f, 200.f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
	RELEASE_INSTANCE(CGameInstance);
}

_tchar * CUI_Manager::Get_ItemTextureName(ITEMID eItemId)
{
	switch ((ITEMID)eItemId)
	{
	case ITEMID::ITEM_BOMB:
		return TEXT("Prototype_Component_Texture_Item_Bomb");
	case ITEMID::ITEM_ROCFEATHER:
		return TEXT("Prototype_Component_Texture_Item_RocsFeather");
	case ITEMID::ITEM_BOW:
		return TEXT("Prototype_Component_Texture_Item_Bow");
	}
}

void CUI_Manager::Add_ItemToInventory(INVENTORYOBJDESC tItem, _uint iIndex)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_float2 vPosition = m_ItemSlots[iIndex]->Get_SlotPosition();

	/* Add UI_InventoryItem to Inventory. */
	CUI::UIDESC tUIDesc;
	ZeroMemory(&tUIDesc, sizeof(CUI::UIDESC));
	tUIDesc.m_fSizeX = 120;
	tUIDesc.m_fSizeY = 120;
	tUIDesc.m_fX = vPosition.x;
	tUIDesc.m_fY = vPosition.y;
	tUIDesc.m_ePass = VTXTEXPASS::VTXTEX_UI_BLEND;
	wcscpy_s(tUIDesc.m_pTextureName, MAX_PATH, Get_ItemTextureName(tItem.m_eItemId));
	
	CUI_InventoryItem* pInventoryIcon = nullptr;
	pGameInstance->Add_GameObject_Out(TEXT("Item_Icon"), TEXT("Prototype_GameObject_UI_InventoryItem"), LEVEL_STATIC, TEXT("Layer_UI"), (CGameObject*&)pInventoryIcon, &tUIDesc);

	pInventoryIcon->Set_InventoryItemType(CUI_InventoryItem::INVENTORYITEM_TYPE::TYPE_INVENTORY);
	m_ItemIcons[iIndex] = pInventoryIcon;

	/* Add UI_ItemChip to Inventory. */
	if (tItem.m_bIsCountable)
	{
		ZeroMemory(&tUIDesc, sizeof(CUI::UIDESC));
		tUIDesc.m_fSizeX = 55;
		tUIDesc.m_fSizeY = 50;
		tUIDesc.m_fX = vPosition.x + 40;
		tUIDesc.m_fY = vPosition.y + 42;
		tUIDesc.m_ePass = VTXTEXPASS::VTXTEX_UI_BLEND;

		CUI_ItemChip* pItemChip = nullptr;
		pGameInstance->Add_GameObject_Out(TEXT("Item_Chip"), TEXT("Prototype_GameObject_UI_ItemChip"), LEVEL_STATIC, TEXT("Layer_UI"), (CGameObject*&)pItemChip, &tUIDesc);

		pItemChip->Set_ScreenX(tUIDesc.m_fX - 13);
		pItemChip->Set_ScreenY(tUIDesc.m_fY - 20);
		pItemChip->Set_SlotIndex(iIndex);

		pItemChip->Set_ChipType(CUI_ItemChip::CHIP_TYPE::CHIP_INVENTORY);
	}

	/* Check whether the added Item is set on the X Key. */
	if (iIndex == m_pInventory->Get_IndexItemX())
	{
		_float2 vPositionX = m_pGameSlotX->Get_SlotPosition();

		/* Add UI_InventoryItem to Game Slot. */
		CUI::UIDESC tUIDesc;
		ZeroMemory(&tUIDesc, sizeof(CUI::UIDESC));
		tUIDesc.m_fSizeX = 100;
		tUIDesc.m_fSizeY = 100;
		tUIDesc.m_fX = vPositionX.x + 8;
		tUIDesc.m_fY = vPositionX.y - 8;
		tUIDesc.m_ePass = VTXTEXPASS::VTXTEX_UI_BLEND;
		wcscpy_s(tUIDesc.m_pTextureName, MAX_PATH, Get_ItemTextureName(tItem.m_eItemId));

		pGameInstance->Add_GameObject_Out(TEXT("Item_Icon"), TEXT("Prototype_GameObject_UI_InventoryItem"), LEVEL_STATIC, TEXT("Layer_UI"), (CGameObject*&)m_pItemIconX, &tUIDesc);
		m_pItemIconX->Set_InventoryItemType(CUI_InventoryItem::INVENTORYITEM_TYPE::TYPE_INGAME);
	}
	/* Check whether the added Item is set on the Y Key. */
	else if (iIndex == m_pInventory->Get_IndexItemY())
	{
		_float2 vPositionY = m_pGameSlotY->Get_SlotPosition();

		/* Add UI_InventoryItem to Game Slot. */
		CUI::UIDESC tUIDesc;
		ZeroMemory(&tUIDesc, sizeof(CUI::UIDESC));
		tUIDesc.m_fSizeX = 100;
		tUIDesc.m_fSizeY = 100;
		tUIDesc.m_fX = vPositionY.x + 8;
		tUIDesc.m_fY = vPositionY.y - 8;
		tUIDesc.m_ePass = VTXTEXPASS::VTXTEX_UI_BLEND;
		wcscpy_s(tUIDesc.m_pTextureName, MAX_PATH, Get_ItemTextureName(tItem.m_eItemId));

		pGameInstance->Add_GameObject_Out(TEXT("Item_Icon"), TEXT("Prototype_GameObject_UI_InventoryItem"), LEVEL_STATIC, TEXT("Layer_UI"), (CGameObject*&)m_pItemIconY, &tUIDesc);
		m_pItemIconY->Set_InventoryItemType(CUI_InventoryItem::INVENTORYITEM_TYPE::TYPE_INGAME);
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CUI_Manager::Add_ItemX()
{
	if (m_pItemIconX)
	{
		m_pItemIconX->Set_ShouldDestroy(true);
		m_pItemIconX = nullptr;
	}

	if (m_pInventory->Get_ItemX())
	{
		INVENTORYOBJDESC* tItem = m_pInventory->Get_ItemX();
		_float2 vPositionX = m_pGameSlotX->Get_SlotPosition();

		/* Add UI_InventoryItem to Game Slot. */
		CUI::UIDESC tUIDesc;
		ZeroMemory(&tUIDesc, sizeof(CUI::UIDESC));
		tUIDesc.m_fSizeX = 100;
		tUIDesc.m_fSizeY = 100;
		tUIDesc.m_fX = vPositionX.x + 8;
		tUIDesc.m_fY = vPositionX.y - 8;
		tUIDesc.m_ePass = VTXTEXPASS::VTXTEX_UI_BLEND;
		wcscpy_s(tUIDesc.m_pTextureName, MAX_PATH, Get_ItemTextureName(tItem->m_eItemId));

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		pGameInstance->Add_GameObject_Out(TEXT("Item_Icon"), TEXT("Prototype_GameObject_UI_InventoryItem"), LEVEL_STATIC, TEXT("Layer_UI"), (CGameObject*&)m_pItemIconX, &tUIDesc);
		m_pItemIconX->Set_InventoryItemType(CUI_InventoryItem::INVENTORYITEM_TYPE::TYPE_INGAME);
		RELEASE_INSTANCE(CGameInstance);
	}
}

void CUI_Manager::Add_ItemY()
{
	if (m_pItemIconY)
	{
		m_pItemIconY->Set_ShouldDestroy(true);
		m_pItemIconY = nullptr;
	}

	if (m_pInventory->Get_ItemY())
	{
		INVENTORYOBJDESC* tItem = m_pInventory->Get_ItemY();
		_float2 vPositionY = m_pGameSlotY->Get_SlotPosition();

		/* Add UI_InventoryItem to Game Slot. */
		CUI::UIDESC tUIDesc;
		ZeroMemory(&tUIDesc, sizeof(CUI::UIDESC));
		tUIDesc.m_fSizeX = 100;
		tUIDesc.m_fSizeY = 100;
		tUIDesc.m_fX = vPositionY.x + 8;
		tUIDesc.m_fY = vPositionY.y - 8;
		tUIDesc.m_ePass = VTXTEXPASS::VTXTEX_UI_BLEND;
		wcscpy_s(tUIDesc.m_pTextureName, MAX_PATH, Get_ItemTextureName(tItem->m_eItemId));

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		pGameInstance->Add_GameObject_Out(TEXT("Item_Icon"), TEXT("Prototype_GameObject_UI_InventoryItem"), LEVEL_STATIC, TEXT("Layer_UI"), (CGameObject*&)m_pItemIconY, &tUIDesc);
		m_pItemIconY->Set_InventoryItemType(CUI_InventoryItem::INVENTORYITEM_TYPE::TYPE_INGAME);
		RELEASE_INSTANCE(CGameInstance);
	}
}

void CUI_Manager::Remove_ItemFromInventory(_uint iIndex)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/* Delete: */
	/*	- Inventory Item Icon; */
	m_ItemIcons[iIndex]->Set_ShouldDestroy(true);
	m_ItemIcons[iIndex] = nullptr;

	/*	- Inventory Item Chip; */
	/* Gets deleted automatically when counter reaches 0. */

	/*- In Game Item Icon. */
	if (iIndex == m_pInventory->Get_IndexItemX())
	{
		m_pItemIconX->Set_ShouldDestroy(true);
		m_pItemIconX = nullptr;
	}
	else if (iIndex == m_pInventory->Get_IndexItemY())
	{
		m_pItemIconY->Set_ShouldDestroy(true);
		m_pItemIconY = nullptr;
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CUI_Manager::Remove_ItemX()
{
}

void CUI_Manager::Remove_ItemY()
{
}

void CUI_Manager::Free()
{
	Safe_Release(m_pInventory);
	CInventory::Get_Instance()->Destroy_Instance();
}