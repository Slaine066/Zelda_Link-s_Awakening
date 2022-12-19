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
	enum MODE { MODE_GAME, MODE_INVENTORY, MODE_END };

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
	_tchar* Get_ItemTextureName(ITEMID eItemId);

	void Add_ItemToInventory(INVENTORYOBJDESC tItem, _uint iIndex);
	void Add_ItemX();
	void Add_ItemY();

	void Render_Rupees();

	/* Functions which get executed every Tick() and update the UI. */
	void Get_PlayerHp();
	void Compute_Hearts();
	void Compute_Rupees();
	void Compute_Inventory();

private:
	_bool m_bIsLoaded = false; /* Switch to TRUE once UI gets loaded the first time. */

	MODE m_eMode = MODE_END;

	/* Hearts Variables */
	vector<class CUI_Heart*> m_Hearts;
	_float m_fMaxHp = 0.f;
	_float m_fCurrentHp = 0.f;

	class CInventory* m_pInventory = nullptr;

	/* Rupees Variables */
	_tchar m_szRupees[MAX_PATH] = TEXT("");

	/* Item Slots Variables */
	class CUI_ItemSlot* m_pGameSlotX = nullptr;
	class CUI_ItemSlot* m_pGameSlotY = nullptr;
	class CUI_InventoryItem* m_pItemIconX = nullptr;
	class CUI_InventoryItem* m_pItemIconY = nullptr;
	vector<class CUI_ItemSlot*> m_ItemSlots;
	_int m_iCurrentSlotIndex = 0;

public:
	virtual void Free() override;
};
END