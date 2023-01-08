#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "Texture.h"

BEGIN(Client)
class CUI_Chat final : public CUI
{
private:
	CUI_Chat(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Chat(const CUI_Chat& rhs);
	virtual ~CUI_Chat() = default;

public:
	void Increase_ChatIndex() { m_iChatIndex++; }
	_uint Get_ChatIndex() { return m_iChatIndex; }
	_uint Get_ChatCount() { return m_pTextureCom->Get_Count(); }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual _uint Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT SetUp_ShaderResources() override;

private:
	_uint m_iChatIndex = 0;
	_float m_fFadeTimer = 0.f;
	_float m_fFadeLifespan = 0.5f;

public:
	static CUI_Chat* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};
END