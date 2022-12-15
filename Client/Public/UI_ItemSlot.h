#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CUI_ItemSlot final : public CUI
{
public:
	enum SLOT_TYPE 
	{ 
		SLOT_GAME_X,			/* In Game Slot X */
		SLOT_GAME_Y,			/* In Game Slot Y */
		SLOT_GAME_X_INVENTORY,	/* In Game Slot X when Inventory is open. */
		SLOT_GAME_Y_INVENTORY,	/* In Game Slot Y when Inventory is open. */
		SLOT_INVENTORY,			/* Inventory Slot Unselected */
		SLOT_INVENTORY_X,		/* Inventory Slot Set on X */
		SLOT_INVENTORY_Y,		/* Inventory Slot Set on Y */
		SLOT_END 
	};

private:
	CUI_ItemSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_ItemSlot(const CUI_ItemSlot& rhs);
	virtual ~CUI_ItemSlot() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual _uint Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT SetUp_ShaderResources() override;

public:
	SLOT_TYPE Get_SlotType() { return m_eType; }
	void Set_SlotType(SLOT_TYPE eType) { m_eType = eType; }
	void Set_ScreenX(_float fScreenX) { m_fScreenX = fScreenX; }
	void Set_ScreenY(_float fScreenY) { m_fScreenY = fScreenY; }
	_float2 Get_SlotPosition();

private:
	SLOT_TYPE m_eType = SLOT_END;

	_float m_fScreenX = 0.f;
	_float m_fScreenY = 0.f;

public:
	static CUI_ItemSlot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};
END