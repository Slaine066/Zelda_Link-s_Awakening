#include "stdafx.h"

#include "Effect.h"
#include "GameInstance.h"

CEffect::CEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{

}

CEffect::CEffect(const CEffect & rhs)
	: CGameObject(rhs)
{

}

HRESULT CEffect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect::Initialize(void * pArg)
{
	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	switch (m_tEffectDesc.m_eEffectType)
	{
		case EFFECT_TYPE::EFFECT_SMOKE:
		{
			m_pTransformCom->Set_Scale(CTransform::STATE::STATE_RIGHT, .2f);
			m_pTransformCom->Set_Scale(CTransform::STATE::STATE_UP, .2f);
			m_pTransformCom->Set_Scale(CTransform::STATE::STATE_LOOK, .2f);

			_float4 vPosition; 
			XMStoreFloat4(&vPosition, m_pTransformCom->Get_State(CTransform::STATE::STATE_TRANSLATION));

			vPosition.y += m_pTransformCom->Get_Scale(CTransform::STATE::STATE_RIGHT) / 2;
			m_pTransformCom->Set_State(CTransform::STATE::STATE_TRANSLATION, XMLoadFloat4(&vPosition));
		}	
		break;
		case EFFECT_TYPE::EFFECT_HIT:
			break;
		case EFFECT_TYPE::EFFECT_DEATH:
			break;
		case EFFECT_TYPE::EFFECT_GET_ITEM:
			break;
		case EFFECT_TYPE::EFFECT_BOMB_EXPLOSION:
			break;
	}

	return S_OK;
}

_uint CEffect::Tick(_float fTimeDelta)
{
	switch (m_tEffectDesc.m_eEffectType)
	{
		case EFFECT_TYPE::EFFECT_SMOKE:
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			m_pTransformCom->LookAt(XMLoadFloat4(&pGameInstance->Get_CamPosition()));
			RELEASE_INSTANCE(CGameInstance);

			if (m_fEffectTimer > m_fEffectLife)
				return OBJ_DESTROY;
			else
				m_fEffectTimer += fTimeDelta;
		}
		break;
		case EFFECT_TYPE::EFFECT_HIT:
			break;
		case EFFECT_TYPE::EFFECT_DEATH:
			break;
		case EFFECT_TYPE::EFFECT_GET_ITEM:
			break;
		case EFFECT_TYPE::EFFECT_BOMB_EXPLOSION:
			break;
	}

	return OBJ_NOEVENT;
}

_uint CEffect::Late_Tick(_float fTimeDelta)
{
	if (m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);

	return OBJ_NOEVENT;
}

HRESULT CEffect::Render()
{
	if (!m_pShaderCom || !m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(4);
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CEffect::Ready_Components(void * pArg)
{
	ZeroMemory(&m_tEffectDesc, sizeof(EFFECTDESC));
	memcpy(&m_tEffectDesc, (EFFECTDESC*)pArg, sizeof(EFFECTDESC));

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	CTransform::TRANSFORMDESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));
	TransformDesc.vInitialWorldMatrix = m_tEffectDesc.m_WorldMatrix;
	TransformDesc.fSpeedPerSec = 0.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	/* For.Com_Transform */
	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;
	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;
	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, m_tEffectDesc.m_pTextureName, (CComponent**)&m_pTextureCom)))
		return E_FAIL;
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect::SetUp_ShaderResources()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(0))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_EffectTimer", &m_fEffectTimer, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_EffectLife", &m_fEffectLife, sizeof(_float))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CEffect * CEffect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEffect* pInstance = new CEffect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Create: CEffect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEffect::Clone(void * pArg)
{
	CEffect* pInstance = new CEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Clone: CEffect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
}