#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CUI_ItemSlot final : public CUI
{
public:
	enum SLOT_TYPE { SLOT_GAME_X, SLOT_GAME_Y, SLOT_INVENTORY, SLOT_INVENTORY_X, SLOT_INVENTORY_Y, SLOT_END };

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
	SLOT_TYPE Get_HeartType() { return m_eType; }
	void Set_HeartType(SLOT_TYPE eType) { m_eType = eType; }

private:
	SLOT_TYPE m_eType = SLOT_END;

public:
	static CUI_ItemSlot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};
END