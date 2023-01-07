#include "stdafx.h"

#include "UI_ScreenFade.h"
#include "GameInstance.h"
#include "UI_Manager.h"

CUI_ScreenFade::CUI_ScreenFade(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{

}

CUI_ScreenFade::CUI_ScreenFade(const CUI_ScreenFade & rhs)
	: CUI(rhs)
{

}

HRESULT CUI_ScreenFade::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_ScreenFade::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

_uint CUI_ScreenFade::Tick(_float fTimeDelta)
{
	if (FAILED(__super::Tick(fTimeDelta)))
		return E_FAIL;

	if (m_tUIDesc.m_ePass == VTXTEXPASS::VTXTEX_UI_BLEND_SCREENFADE_IN || m_tUIDesc.m_ePass == VTXTEXPASS::VTXTEX_UI_BLEND_SCREENFADE_OUT)
	{
		if (m_fFadeTimer >= m_fScreenFadeTime)
		{
			m_tUIDesc.m_ePass = VTXTEXPASS::VTXTEX_END;
			m_fFadeTimer = 0.f;
		}
		else
			m_fFadeTimer += fTimeDelta;
	}

	return OBJ_NOEVENT;
}

_uint CUI_ScreenFade::Late_Tick(_float fTimeDelta)
{
	if (FAILED(__super::Late_Tick(fTimeDelta)))
		return E_FAIL;

	return OBJ_NOEVENT;
}

HRESULT CUI_ScreenFade::Render()
{
	if (m_tUIDesc.m_ePass == VTXTEXPASS::VTXTEX_END)
		return S_OK;

	if (!m_pShaderCom || !m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(m_tUIDesc.m_ePass);
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_ScreenFade::Ready_Components(void * pArg)
{
	memcpy(&m_tUIDesc, (UIDESC*)pArg, sizeof(UIDESC));

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;
	/* For.Com_Transform */
	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;
	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;
	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Gradient"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_ScreenFade::SetUp_ShaderResources()
{
	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(0))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_FadeTimer", &m_fFadeTimer, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_FadeLifespan", &m_fScreenFadeTime, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CUI_ScreenFade * CUI_ScreenFade::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_ScreenFade* pInstance = new CUI_ScreenFade(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Create: CUI_ScreenFade"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI_ScreenFade::Clone(void * pArg)
{
	CUI_ScreenFade* pInstance = new CUI_ScreenFade(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Clone: CUI_ScreenFade"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_ScreenFade::Free()
{
	__super::Free();
}