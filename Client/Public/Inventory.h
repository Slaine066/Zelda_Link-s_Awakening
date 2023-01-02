#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

typedef struct tagInventoryObjectDesc
{
	ITEMID m_eItemId = ITEMID::ITEM_END;
	_bool m_bIsCountable = false;
	_uint m_iCounter = 0;
} INVENTORYOBJDESC;

class CInventory final : public CBase
{
	DECLARE_SINGLETON(CInventory)

private:
	CInventory();
	virtual ~CInventory() = default;

public:
	/* Getters & Setters*/
	_uint Get_Rupees() { return m_iRupees; }

	INVENTORYOBJDESC* Get_Sword() { return m_pSword; }
	INVENTORYOBJDESC* Get_Shield() { return m_pShield; }
	INVENTORYOBJDESC* Get_Vest() { return m_pVest; }
	INVENTORYOBJDESC* Get_Belt() { return m_pBelt; }
	INVENTORYOBJDESC* Get_Boots() { return m_pBoots; }

	void Set_Sword(INVENTORYOBJDESC* pSword) { m_pSword = pSword; }
	void Set_Shield(INVENTORYOBJDESC* pShield) { m_pShield = pShield; }
	void Set_Vest(INVENTORYOBJDESC* pVest) { m_pVest = pVest; }
	void Set_Belt(INVENTORYOBJDESC* pBelt) { m_pBelt = pBelt; }
	void Set_Boots(INVENTORYOBJDESC* pBoots) { m_pBoots = pBoots; }

	vector<INVENTORYOBJDESC> Get_Items() { return m_Items; }

	_uint Get_IndexItemX() { return m_iIndexItemX; }
	_uint Get_IndexItemY() { return m_iIndexItemY; }
	_uint Get_ItemCounter(_uint iIndex) { return m_Items[iIndex].m_iCounter; }

	void Set_IndexItemX(_uint iIndexItemX) { m_iIndexItemX = iIndexItemX; }
	void Set_IndexItemY(_uint iIndexItemY) { m_iIndexItemY = iIndexItemY; }
	INVENTORYOBJDESC* Get_ItemX() { return m_Items[m_iIndexItemX].m_eItemId == ITEM_END ? nullptr : &m_Items[m_iIndexItemX]; }
	INVENTORYOBJDESC* Get_ItemY() { return m_Items[m_iIndexItemY].m_eItemId == ITEM_END ? nullptr : &m_Items[m_iIndexItemY]; }

	_bool Get_IsGameStarted() { return m_bIsGameStarted; }
	void Set_IsGameStarted(_bool bIsGameStarted) { m_bIsGameStarted = bIsGameStarted; }
	
	/* Inventory Functions */
	void Add_Item(ITEMID eItemId);
	void Decrease_ItemCount(ITEMID eItemId, _uint iItemIndex, _uint iDecreaseAmount = 1);

private:
	void Add_ToEquipment(ITEMID eItemId);
	void Add_ToInventory(ITEMID eItemId);
	void Remove_FromInventory(_uint iIndex);

private:
	_uint m_iRupees = 0.f;

	/* Equipments */
	INVENTORYOBJDESC* m_pSword = nullptr;
	INVENTORYOBJDESC* m_pShield = nullptr;
	INVENTORYOBJDESC* m_pVest = nullptr;
	INVENTORYOBJDESC* m_pBelt = nullptr;
	INVENTORYOBJDESC* m_pBoots = nullptr;

	/* Items in the 12 Inventory Slots. */
	vector<INVENTORYOBJDESC> m_Items;

	_uint m_iIndexItemX = 0;
	_uint m_iIndexItemY = 1;

	_bool m_bIsGameStarted = false;

public:
	virtual void Free() override;
};
END