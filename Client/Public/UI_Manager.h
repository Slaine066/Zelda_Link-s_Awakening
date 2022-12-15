#pragma once

#include "Client_Defines.h"
#include "Base.h"
#include "Inventory.h"

/*
Note that this Class contains a pointer of the same Object stored in the Object Manager.
"Then why do you need to store them twice?"
This class just encloses a bunch of utility functions which allows us to work with UI Objects more comfortably.
(By doing so, we don't need to serach in the Object Manager every time).
*/

BEGIN(Client)
class CUI_Manager final : public CBase
{
	DECLARE_SINGLETON(CUI_Manager)

public:
	enum MODE { MODE_GAME, MODE_MAP, MODE_INVENTORY, MODE_END };

private:
	CUI_Manager();
	virtual ~CUI_Manager() = default;

public:
	HRESULT Initialize();
	_uint Tick(_float fTimeDelta);
	void Handle_Input();
	void Clear();

private:
	/* Initialize Functions */
	HRESULT Build_Inventory();
	HRESULT Build_InventoryItemSlots();
	HRESULT Build_GameItemSlots();
	HRESULT Build_Hearts();
	HRESULT Build_Rupees();

public:
	MODE Get_Mode() { return m_eMode; }
	void Set_Mode(MODE eMode) { m_eMode = eMode; }
	_float Get_MaxHp() { return m_fMaxHp; }
	_float Get_CurrentHp() { return m_fCurrentHp; }

	/* Manage Functions */
	void Get_PlayerHp();
	void Compute_Hearts();
	void Compute_Rupees();
	void Render_Rupees();

	_tchar* Get_ItemTextureName(ITEMID eItemId);
	void Add_ItemToInventory(INVENTORYOBJDESC tItem, _uint iIndex);

private:
	_bool m_bIsLoaded = false;
	MODE m_eMode = MODE_END;

	class CInventory* m_pInventory = nullptr;

	/* Hearts Variables */
	vector<class CUI_Heart*> m_Hearts;
	_float m_fMaxHp = 0.f;
	_float m_fCurrentHp = 0.f;

	/* Item Slots Variables */
	class CUI_ItemSlot* m_pItemSlotX = nullptr;
	class CUI_ItemSlot* m_pItemSlotY = nullptr;
	vector<class CUI_ItemSlot*> m_ItemSlots;

	_tchar m_szRupees[MAX_PATH] = TEXT("");

public:
	virtual void Free() override;
};
END