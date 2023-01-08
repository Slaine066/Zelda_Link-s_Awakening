#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CUI_RupeesFont final : public CUI
{
private:
	CUI_RupeesFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_RupeesFont(const CUI_RupeesFont& rhs);
	virtual ~CUI_RupeesFont() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual _uint Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT SetUp_ShaderResources() override;

private:
	_uint m_iRupeesCount = 0;

public:
	static CUI_RupeesFont* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};
END