#include "stdafx.h"

#include "UI_Heart.h"
#include "GameInstance.h"
#include "UI_Manager.h"

CUI_Heart::CUI_Heart(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
	
}

CUI_Heart::CUI_Heart(const CUI_Heart & rhs)
	: CUI(rhs)
{

}

HRESULT CUI_Heart::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Heart::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

_uint CUI_Heart::Tick(_float fTimeDelta)
{
	if (FAILED(__super::Tick(fTimeDelta)))
		return E_FAIL;

	return OBJ_NOEVENT;
}

_uint CUI_Heart::Late_Tick(_float fTimeDelta)
{
	if (FAILED(__super::Late_Tick(fTimeDelta)))
		return E_FAIL;

	return OBJ_NOEVENT;
}

HRESULT CUI_Heart::Render()
{
	if (CUI_Manager::Get_Instance()->Get_Mode() == CUI_Manager::MODE::MODE_END)
		return S_OK;

	if (!m_pShaderCom || !m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(2); /* 2nd Pass: AlphaBlend */
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_Heart::Ready_Components(void * pArg)
{
	ZeroMemory(&m_tUIDesc, sizeof(UIDESC));
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
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Heart"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Heart::SetUp_ShaderResources()
{
	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(m_eType))))
		return E_FAIL;

	return S_OK;
}

CUI_Heart * CUI_Heart::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_Heart* pInstance = new CUI_Heart(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Create: CUI_Heart"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI_Heart::Clone(void * pArg)
{
	CUI_Heart* pInstance = new CUI_Heart(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Clone: CUI_Heart"));
		Safe_Release(pInstance);
	}
	
	return pInstance;
}

void CUI_Heart::Free()
{
	__super::Free();
}