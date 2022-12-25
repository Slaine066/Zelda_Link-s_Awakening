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
			m_eShaderPass = VTXTEXPASS::VTXTEX_EFFECT_SMOKE;

			m_fEffectScale = .3f;
			m_pTransformCom->Set_Scale(CTransform::STATE::STATE_RIGHT, m_fEffectScale);
			m_pTransformCom->Set_Scale(CTransform::STATE::STATE_UP, m_fEffectScale);
			m_pTransformCom->Set_Scale(CTransform::STATE::STATE_LOOK, m_fEffectScale);

			_float4 vPosition; 
			XMStoreFloat4(&vPosition, m_pTransformCom->Get_State(CTransform::STATE::STATE_TRANSLATION));

			vPosition.y += m_pTransformCom->Get_Scale(CTransform::STATE::STATE_RIGHT) / 4;
			m_pTransformCom->Set_State(CTransform::STATE::STATE_TRANSLATION, XMLoadFloat4(&vPosition));
		}	
		break;
		case EFFECT_TYPE::EFFECT_SWISH:
		{
			m_eShaderModelPass = VTXMODELPASS::VTXMODEL_SWISH;
		}
		break;
		case EFFECT_TYPE::EFFECT_SWORD_SLASH:
		{
			m_eShaderModelPass = VTXMODELPASS::VTXMODEL_SWORDSLASH;
		}
		break;
		case EFFECT_TYPE::EFFECT_RING:
		{
			m_eShaderModelPass = VTXMODELPASS::VTXMODEL_HITRING;

			m_fEffectScale = .2f;
			m_pTransformCom->Set_Scale(CTransform::STATE::STATE_RIGHT, m_fEffectScale);
			m_pTransformCom->Set_Scale(CTransform::STATE::STATE_UP, m_fEffectScale);
			m_pTransformCom->Set_Scale(CTransform::STATE::STATE_LOOK, m_fEffectScale);

			/* Move it a bit far away to the Camera, cause it has some Z-fighting with VTXTEX_EFFECT_HIT_FLASH. */
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			m_pTransformCom->LookAt(XMLoadFloat4(&pGameInstance->Get_CamPosition()));
			m_pTransformCom->Move_Backward(0.01f); 
			RELEASE_INSTANCE(CGameInstance);
		}
		break;
		case EFFECT_TYPE::EFFECT_HIT:
		{
			m_eShaderModelPass = VTXMODELPASS::VTXMODEL_HIT;

			m_fEffectScale = .3f;
			m_pTransformCom->Set_Scale(CTransform::STATE::STATE_RIGHT, m_fEffectScale);
			m_pTransformCom->Set_Scale(CTransform::STATE::STATE_UP, m_fEffectScale);
			m_pTransformCom->Set_Scale(CTransform::STATE::STATE_LOOK, m_fEffectScale);

			/* Move it a bit closer to the Camera, cause it has some Z-fighting with VTXMODEL_HIT. */
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			m_pTransformCom->LookAt(XMLoadFloat4(&pGameInstance->Get_CamPosition()));
			m_pTransformCom->Move_Straight(0.01f);
			RELEASE_INSTANCE(CGameInstance);
		}
		break;
		case EFFECT_TYPE::EFFECT_HIT_FLASH:
		{
			m_eShaderPass = VTXTEXPASS::VTXTEX_EFFECT_HIT_FLASH;

			m_fEffectScale = .5f;
			m_pTransformCom->Set_Scale(CTransform::STATE::STATE_RIGHT, m_fEffectScale);
			m_pTransformCom->Set_Scale(CTransform::STATE::STATE_UP, m_fEffectScale);
			m_pTransformCom->Set_Scale(CTransform::STATE::STATE_LOOK, m_fEffectScale);
		}
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
			if (m_fEffectTimer >= m_tEffectDesc.m_fEffectLifespan)
				return OBJ_DESTROY;
			else
			{
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

				m_pTransformCom->LookAt(XMLoadFloat4(&pGameInstance->Get_CamPosition()));
				
				_float fScale = m_fEffectScale + (0.f - m_fEffectScale) * m_fEffectTimer / m_tEffectDesc.m_fEffectLifespan;

				m_pTransformCom->Set_Scale(CTransform::STATE::STATE_RIGHT, fScale);
				m_pTransformCom->Set_Scale(CTransform::STATE::STATE_UP, fScale);
				m_pTransformCom->Set_Scale(CTransform::STATE::STATE_LOOK, fScale);

				RELEASE_INSTANCE(CGameInstance);

				m_fEffectTimer += fTimeDelta;
			}
		}
		break;
		case EFFECT_TYPE::EFFECT_RING:
		{
			if (m_fEffectTimer >= m_tEffectDesc.m_fEffectLifespan)
				return OBJ_DESTROY;
			else
			{
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

				m_pTransformCom->LookAt(XMLoadFloat4(&pGameInstance->Get_CamPosition()));
				
				/* Increase Scale based on Time. */
				_float fScale = m_fEffectScale + (m_fEffectScale * 2 - m_fEffectScale) * m_fEffectTimer / m_tEffectDesc.m_fEffectLifespan;
				m_pTransformCom->Set_Scale(CTransform::STATE::STATE_RIGHT, fScale);
				m_pTransformCom->Set_Scale(CTransform::STATE::STATE_UP, fScale);
				m_pTransformCom->Set_Scale(CTransform::STATE::STATE_LOOK, fScale);

				RELEASE_INSTANCE(CGameInstance);

				m_fEffectTimer += fTimeDelta;
			}
		}
		break;
		case EFFECT_TYPE::EFFECT_HIT:
		{
			if (m_fEffectTimer >= m_tEffectDesc.m_fEffectLifespan)
				return OBJ_DESTROY;
			else
			{
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

				m_pTransformCom->LookAt(XMLoadFloat4(&pGameInstance->Get_CamPosition()));

				/* Increase Scale based on Time. */
				if (m_fEffectTimer < m_tEffectDesc.m_fEffectLifespan / 2)
				{
					_float fScale = m_fEffectScale + (m_fEffectScale * 2 - m_fEffectScale) * (m_fEffectTimer - (m_tEffectDesc.m_fEffectLifespan / 2)) / m_tEffectDesc.m_fEffectLifespan;
					m_pTransformCom->Set_Scale(CTransform::STATE::STATE_RIGHT, fScale);
					m_pTransformCom->Set_Scale(CTransform::STATE::STATE_UP, fScale);
					m_pTransformCom->Set_Scale(CTransform::STATE::STATE_LOOK, fScale);
				}
				/* Decrease Scale based on Time. */
				else
				{
					_float fScale = m_fEffectScale + (m_fEffectScale * .4 - m_fEffectScale) * (m_fEffectTimer - (m_tEffectDesc.m_fEffectLifespan / 2)) / m_tEffectDesc.m_fEffectLifespan;
					m_pTransformCom->Set_Scale(CTransform::STATE::STATE_RIGHT, fScale);
					m_pTransformCom->Set_Scale(CTransform::STATE::STATE_UP, fScale);
					m_pTransformCom->Set_Scale(CTransform::STATE::STATE_LOOK, fScale);
				}
				
				RELEASE_INSTANCE(CGameInstance);

				m_fEffectTimer += fTimeDelta;
			}
		}
		break;
		case EFFECT_TYPE::EFFECT_HIT_FLASH:
		{
			if (m_fEffectTimer >= m_tEffectDesc.m_fEffectLifespan)
				return OBJ_DESTROY;
			else
			{
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

				m_pTransformCom->LookAt(XMLoadFloat4(&pGameInstance->Get_CamPosition()));

				/* Increase Scale based on Time. */
				_float fScale = m_fEffectScale + (m_fEffectScale * 2 - m_fEffectScale) * m_fEffectTimer / m_tEffectDesc.m_fEffectLifespan;
				m_pTransformCom->Set_Scale(CTransform::STATE::STATE_RIGHT, fScale);
				m_pTransformCom->Set_Scale(CTransform::STATE::STATE_UP, fScale);
				m_pTransformCom->Set_Scale(CTransform::STATE::STATE_LOOK, fScale);

				RELEASE_INSTANCE(CGameInstance);

				m_fEffectTimer += fTimeDelta;
			}
		}
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
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
		Compute_CamDistance(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	}

	return OBJ_NOEVENT;
}

HRESULT CEffect::Render()
{
	if (m_tEffectDesc.m_eEffectType == EFFECT_TYPE::EFFECT_HIT)
	{
		if (m_fEffectTimer <= (m_tEffectDesc.m_fEffectLifespan / 100 * 15))
			return S_OK;
	}

	if (Is_ModelEffect())
	{
		if (!m_pShaderCom || !m_pModelCom)
			return E_FAIL;
	}
	else if(!m_pShaderCom || !m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;
	
	if (Is_ModelEffect())
	{
		_uint iNumMeshes = m_pModelCom->Get_NumMeshContainers();

		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			if (FAILED(m_pModelCom->SetUp_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				return E_FAIL;

			m_pModelCom->SetUp_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS);
			m_pModelCom->SetUp_Material(m_pShaderCom, "g_SpecularTexture", i, aiTextureType_SPECULAR);

			if (FAILED(m_pModelCom->Render(m_pShaderCom, i, m_eShaderModelPass)))
				return E_FAIL;
		}
	}
	else
	{
		m_pShaderCom->Begin(m_eShaderPass);
		m_pVIBufferCom->Render();
	}

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
	TransformDesc.fSpeedPerSec = 0.1f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	/* For.Com_Transform */
	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	if (Is_ModelEffect())
	{	
		/* For.Com_Model*/
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, Get_ModelPrototypeId(), (CComponent**)&m_pModelCom)))
			return E_FAIL;

		/* For.Com_Shader */
		if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"), (CComponent**)&m_pShaderCom)))
			return E_FAIL;
	}
	else
	{
		/* For.Com_VIBuffer */
		if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
			return E_FAIL;

		/* For.Com_Texture */
		if (!Is_ModelEffect())
		{			
			if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, Get_TextureName(), (CComponent**)&m_pTextureCom)))
				return E_FAIL;
		}

		/* For.Com_Shader */
		if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderCom)))
			return E_FAIL;
	}

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

	if (!Is_ModelEffect())
	{
		if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(0))))
			return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_EffectTimer", &m_fEffectTimer, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_EffectLifespan", &m_tEffectDesc.m_fEffectLifespan, sizeof(_float))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

_bool CEffect::Is_ModelEffect()
{
	_bool bIsModel = false;
	switch (m_tEffectDesc.m_eEffectType)
	{
	case EFFECT_TYPE::EFFECT_SMOKE:
	case EFFECT_TYPE::EFFECT_HIT_FLASH:
		bIsModel = false;
		break;

	case EFFECT_TYPE::EFFECT_SWISH:
	case EFFECT_TYPE::EFFECT_SWORD_SLASH:
	case EFFECT_TYPE::EFFECT_RING:
	case EFFECT_TYPE::EFFECT_HIT:
		bIsModel = true;
		break;
	}

	return bIsModel;
}

_tchar * CEffect::Get_TextureName()
{
	switch (m_tEffectDesc.m_eEffectType)
	{
	case EFFECT_TYPE::EFFECT_SMOKE:
		return TEXT("Prototype_Component_Texture_Smoke");
		break;
	case EFFECT_TYPE::EFFECT_HIT_FLASH:
		return TEXT("Prototype_Component_Texture_Hit_Flash");
		break;
	}
}

_tchar * CEffect::Get_ModelPrototypeId()
{
	switch (m_tEffectDesc.m_eEffectType)
	{
	case EFFECT_TYPE::EFFECT_SWISH:
		return TEXT("Prototype_Component_Model_Swish");
		break;
	case EFFECT_TYPE::EFFECT_SWORD_SLASH:
		return TEXT("Prototype_Component_Model_SwordSlash");
		break;
	case EFFECT_TYPE::EFFECT_RING:
		return TEXT("Prototype_Component_Model_HitRing");
		break;
	case EFFECT_TYPE::EFFECT_HIT:
		return TEXT("Prototype_Component_Model_HitFlash");
		break;
	}
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