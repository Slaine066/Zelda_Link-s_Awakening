#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CUI_LevelName final : public CUI
{
private:
	CUI_LevelName(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_LevelName(const CUI_LevelName& rhs);
	virtual ~CUI_LevelName() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual _uint Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT SetUp_ShaderResources() override;

public:
	void Set_Level(_uint iLevel);

private:
	_float m_fFadeTimer = 0.f;
	_float m_fScreenFadeTime = 3.f;

	_uint m_iLevel = 0;

public:
	static CUI_LevelName* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};
END