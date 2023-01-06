#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CUI_InventoryItem final : public CUI
{
public:
	enum INVENTORYITEM_TYPE { TYPE_INGAME, TYPE_INVENTORY, TYPE_END };

private:
	CUI_InventoryItem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_InventoryItem(const CUI_InventoryItem& rhs);
	virtual ~CUI_InventoryItem() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual _uint Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT SetUp_ShaderResources() override;

public:
	INVENTORYITEM_TYPE Get_InventoryItemType() { return m_eType; }
	void Set_InventoryItemType(INVENTORYITEM_TYPE eType) { m_eType = eType; }

private:
	INVENTORYITEM_TYPE m_eType = TYPE_END;
	_bool m_bIsHovered = false;

public:
	static CUI_InventoryItem* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};
END