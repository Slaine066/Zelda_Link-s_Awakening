#include "stdafx.h"

#include "Inventory.h"
#include "UI_Manager.h"

IMPLEMENT_SINGLETON(CInventory)

CInventory::CInventory()
{
	m_Items.resize(12);
	m_iIndexItemX = 0;
	m_iIndexItemY = 1;
}

void CInventory::Add_Item(ITEMID eItemId)
{
	switch ((ITEMID)eItemId)
	{
		case ITEMID::ITEM_RUPEE_GREEN:
			m_iRupees++;
			break;
		/*case ITEMID::ITEM_ROCFEATHER:
		case ITEMID::ITEM_BOW:*/
		case ITEMID::ITEM_BOMB:
			Add_ToInventory(eItemId);
			break;
	}
}

void CInventory::Remove_Item(ITEMID eItemId, _uint iCount)
{
	switch ((ITEMID)eItemId)
	{
		case ITEMID::ITEM_RUPEE_GREEN:
			m_iRupees -= iCount;
			break;
		case ITEMID::ITEM_BOMB:
			break;
	}
}

void CInventory::Add_ToInventory(ITEMID eItemId)
{
	/* First, check if we already have the Item we are trying to add in the Inventory. */
	for (_uint i = 0; i < m_Items.size(); i++)
	{
		if (m_Items[i].m_eItemId == eItemId)
		{
			m_Items[i].m_iCounter++; /* If we already have it, just increase the counter. */
			return;
		}
	}

	/* If we don't have it, we need to add it.
	We have to add the Item in the first empty slot in the Inventory. */
	for (_uint i = 0; i < m_Items.size(); i++)
	{
		if (m_Items[i].m_eItemId == ITEMID::ITEM_END)
		{
			INVENTORYOBJDESC tInventoryObjectDesc;
			ZeroMemory(&tInventoryObjectDesc, sizeof(INVENTORYOBJDESC));

			tInventoryObjectDesc.m_eItemId = eItemId;

			if (eItemId == ITEMID::ITEM_BOMB)
			{
				tInventoryObjectDesc.m_bIsCountable = true;
				tInventoryObjectDesc.m_iCounter = 1;
			}
			
			m_Items[i] = tInventoryObjectDesc;

			CUI_Manager::Get_Instance()->Add_ItemToInventory(eItemId, i); /* Add the Item to UI. */

			return;
		}
	}
}

void CInventory::Free()
{
}