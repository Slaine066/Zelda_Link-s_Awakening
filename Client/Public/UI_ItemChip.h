#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CUI_ItemChip final : public CUI
{
public:
	enum CHIP_TYPE
	{
		CHIP_GAME,
		CHIP_INVENTORY,
		CHIP_END
	};

private:
	CUI_ItemChip(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_ItemChip(const CUI_ItemChip& rhs);
	virtual ~CUI_ItemChip() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual _uint Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT SetUp_ShaderResources() override;

public:
	CHIP_TYPE Get_ChipType() { return m_eType; }
	void Set_ChipType(CHIP_TYPE eType) { m_eType = eType; }
	void Set_ScreenX(_float fScreenX) { m_fScreenX = fScreenX; }
	void Set_ScreenY(_float fScreenY) { m_fScreenY = fScreenY; }
	_float2 Get_ChipPosition();
	void Set_Counter(_uint iCounter) { m_iCounter = iCounter; }
	void Set_SlotIndex(_uint iSlotIndex) { m_iSlotIndex = iSlotIndex; }

private:
	CHIP_TYPE m_eType = CHIP_END;

	_float m_fScreenX = 0.f;
	_float m_fScreenY = 0.f;

	_uint m_iCounter = 0;
	_tchar m_szCounter[MAX_PATH] = TEXT("");

	_uint m_iSlotIndex = 0;

public:
	static CUI_ItemChip* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};
END