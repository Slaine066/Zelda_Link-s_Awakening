#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CUI_ScreenFade final : public CUI
{
private:
	CUI_ScreenFade(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_ScreenFade(const CUI_ScreenFade& rhs);
	virtual ~CUI_ScreenFade() = default;

public:
	_float Get_ScreenFadeTime() { return m_fScreenFadeTime; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual _uint Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT SetUp_ShaderResources() override;

public:
	void FadeIn() { m_eShaderPass = VTXTEXPASS::VTXTEX_UI_BLEND_SCREENFADE_IN; }
	void FadeOut() { m_eShaderPass = VTXTEXPASS::VTXTEX_UI_BLEND_SCREENFADE_OUT; }

private:
	VTXTEXPASS m_eShaderPass = VTXTEXPASS::VTXTEX_END;

	_float m_fFadeTimer = 0.f;
	_float m_fScreenFadeTime = 0.5f;

public:
	static CUI_ScreenFade* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};
END