#include "stdafx.h"

#include "InteractableObject.h"
#include "GameInstance.h"

CInteractableObject::CInteractableObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CActor(pDevice, pContext)
{
}

CInteractableObject::CInteractableObject(const CInteractableObject & rhs)
	: CActor(rhs)
{
}

HRESULT CInteractableObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CInteractableObject::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

_uint CInteractableObject::Tick(_float fTimeDelta)
{
	_uint iEvent = __super::Tick(fTimeDelta);

	if (iEvent == OBJ_STOP)
		return iEvent;

	return S_OK;
}

_uint CInteractableObject::Late_Tick(_float fTimeDelta)
{
	_uint iEvent = __super::Late_Tick(fTimeDelta);

	if (iEvent == OBJ_STOP)
		return iEvent;

	if (m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	return S_OK;
}

HRESULT CInteractableObject::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CInteractableObject::Ready_Components(void* pArg)
{
	memcpy(&m_tModelDesc, (MODELDESC*)pArg, sizeof(MODELDESC));

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC TransformDesc;
	TransformDesc.vInitialWorldMatrix = m_tModelDesc.mWorldMatrix;
	TransformDesc.fSpeedPerSec = 0.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	if (!wcscmp(m_tModelDesc.wcObjName, TEXT("Sword")))
	{
		/* For.Com_Shader */
		if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"), (CComponent**)&m_pShaderCom)))
			return E_FAIL;
	}
	else
	{
		/* For.Com_Shader */
		if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"), (CComponent**)&m_pShaderCom)))
			return E_FAIL;
	}

	_uint iLevelIndex = Compute_LevelIndex(m_tModelDesc.wcObjName);

	/* For.Com_Model*/
	if (FAILED(__super::Add_Components(TEXT("Com_Model"), iLevelIndex, m_tModelDesc.wcModelPrototypeId, (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CInteractableObject::SetUp_ShaderResources()
{
	if (!m_pShaderCom)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;	
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

_uint CInteractableObject::Compute_LevelIndex(_tchar * pObjName)
{
	if (wcsstr(pObjName, TEXT("Field")) || !wcscmp(pObjName, TEXT("Treasure")) || !wcscmp(pObjName, TEXT("Sword")))
		return 0;

	return CGameInstance::Get_Instance()->Get_NextLevelIndex();
}

void CInteractableObject::Free()
{
	__super::Free();
}