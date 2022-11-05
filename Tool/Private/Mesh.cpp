#include "stdafx.h"

#include "Mesh.h"
#include "GameInstance.h"

CMesh::CMesh(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CMesh::CMesh(const CMesh & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMesh::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMesh::Initialize(void* pArg)
{
	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	return S_OK;
}

_uint CMesh::Tick(_float fTimeDelta)
{
	/*
	* Tool doesn't need Animations (..for now) 
	* Be sure to use the "Shader_VtxModel" (not the "Shader_VtxAnimModel"). 
	* If the "Shader_VtxAnimModel" is used but the Animations are not played the Bones will be rendered wrongly.
	*/
	if (m_tModelDesc.eModelType == CModel::TYPE::TYPE_ANIM)
	{
		m_pModelCom->Set_CurrentAnimIndex(0);
		_bool isFinished = false;
		m_pModelCom->Play_Animation(fTimeDelta, isFinished);
	}

	return OBJ_NOEVENT;
}

void CMesh::Late_Tick(_float fTimeDelta)
{
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CMesh::Render()
{
	if (m_pShaderCom == nullptr || m_pModelCom == nullptr)
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshContainers();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 0, false))) // "false" (means Model should not animate).
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CMesh::Ready_Components(void* pArg)
{
	memcpy(&m_tModelDesc, pArg, sizeof(MODELDESC));

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_TOOL, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC tTransformDesc;
	ZeroMemory(&tTransformDesc, sizeof(CTransform::TRANSFORMDESC));
	tTransformDesc.vInitialWorldMatrix = m_tModelDesc.mWorldMatrix;
	tTransformDesc.fSpeedPerSec = 3.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_TOOL, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &tTransformDesc)))
		return E_FAIL;
	 
	/* For.Com_Shader */
	//															- - - > Read comment inside Tick() function.
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_TOOL, /*m_tModelDesc.eModelType == CModel::TYPE::TYPE_ANIM ? TEXT("Prototype_Component_Shader_VtxAnimModel") :*/ TEXT("Prototype_Component_Shader_VtxModel"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model*/
	if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_TOOL, m_tModelDesc.wcModelPrototypeId, (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMesh::SetUp_ShaderResources()
{
	if (m_pShaderCom == nullptr)
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

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CMesh"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMesh::Clone(void * pArg)
{
	CMesh* pInstance = new CMesh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CMesh"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMesh::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
}