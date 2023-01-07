#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CUI_ScreenFade final : public CUI
{
public:
	enum TYPE { TYPE_BLACK, TYPE_WHITE, TYPE_END };

private:
	CUI_ScreenFade(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_ScreenFade(const CUI_ScreenFade& rhs);
	virtual ~CUI_ScreenFade() = default;

public:
	void Set_Type(TYPE eType) { m_eType = eType; }
	_float Get_ScreenFadeTime() { return m_fScreenFadeTime; }
	void Set_ScreenFadeTime(_float fTime) { m_fScreenFadeTime = fTime; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual _uint Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT SetUp_ShaderResources() override;

public:
	void FadeIn() { m_tUIDesc.m_ePass = VTXTEXPASS::VTXTEX_UI_BLEND_SCREENFADE_IN; }
	void FadeOut() { m_tUIDesc.m_ePass = VTXTEXPASS::VTXTEX_UI_BLEND_SCREENFADE_OUT; }

private:
	TYPE m_eType = TYPE::TYPE_BLACK;

	_float m_fFadeTimer = 0.f;
	_float m_fScreenFadeTime = 0.5f;

public:
	static CUI_ScreenFade* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};
END