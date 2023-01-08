#include "stdafx.h"

#include "UI_LevelName.h"
#include "GameInstance.h"
#include "UI_Manager.h"

CUI_LevelName::CUI_LevelName(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{

}

CUI_LevelName::CUI_LevelName(const CUI_LevelName & rhs)
	: CUI(rhs)
{

}

HRESULT CUI_LevelName::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_LevelName::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

_uint CUI_LevelName::Tick(_float fTimeDelta)
{
	if (FAILED(__super::Tick(fTimeDelta)))
		return E_FAIL;

	if (m_tUIDesc.m_ePass == VTXTEXPASS::VTXTEX_UI_BLEND_SCREENFADE_IN || m_tUIDesc.m_ePass == VTXTEXPASS::VTXTEX_UI_BLEND_SCREENFADE_OUT)
	{
		if (m_fFadeTimer >= m_fScreenFadeTime)
		{
			m_tUIDesc.m_ePass = VTXTEXPASS::VTXTEX_END;
			m_fFadeTimer = 0.f;

			m_bShouldDestroy = true;
		}
		else
			m_fFadeTimer += fTimeDelta;
	}

	return OBJ_NOEVENT;
}

_uint CUI_LevelName::Late_Tick(_float fTimeDelta)
{
	if (FAILED(__super::Late_Tick(fTimeDelta)))
		return E_FAIL;

	return OBJ_NOEVENT;
}

HRESULT CUI_LevelName::Render()
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

HRESULT CUI_LevelName::Ready_Components(void * pArg)
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
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_LevelName"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_LevelName::SetUp_ShaderResources()
{
	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(m_iLevel))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_FadeTimer", &m_fFadeTimer, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_FadeLifespan", &m_fScreenFadeTime, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

void CUI_LevelName::Set_Level(_uint iLevel)
{
	switch ((LEVEL)iLevel)
	{
		case LEVEL::LEVEL_FIELD:
		{
			m_iLevel = 0;
			break;
		}
		case LEVEL::LEVEL_MORIBLINCAVE:
		{
			m_iLevel = 1;
			break;
		}
	}
}

CUI_LevelName * CUI_LevelName::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_LevelName* pInstance = new CUI_LevelName(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Create: CUI_LevelName"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI_LevelName::Clone(void * pArg)
{
	CUI_LevelName* pInstance = new CUI_LevelName(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Clone: CUI_LevelName"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_LevelName::Free()
{
	__super::Free();
}