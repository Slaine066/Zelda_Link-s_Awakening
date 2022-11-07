#include "stdafx.h"

#include "Actor.h"
#include "GameInstance.h"

CActor::CActor(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CActor::CActor(const CActor & rhs)
	: CGameObject(rhs)
{
}

HRESULT CActor::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CActor::Initialize(void * pArg)
{
	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	return S_OK;
}

_uint CActor::Tick(_float fTimeDelta)
{
	// Update Position Variable in SuperClass
	_float4 vObjPosition;
	XMStoreFloat4(&vObjPosition, m_pTransformCom->Get_State(CTransform::STATE::STATE_TRANSLATION));
	Set_Position(_float3(vObjPosition.x, vObjPosition.y, vObjPosition.z));

	return OBJ_NOEVENT;
}

void CActor::Late_Tick(_float fTimeDelta)
{
	// Update Colliders
	if (m_pColliderAABBCom)
		m_pColliderAABBCom->Update(m_pTransformCom->Get_WorldMatrix());
	if (m_pColliderOBBCom)
		m_pColliderOBBCom->Update(m_pTransformCom->Get_WorldMatrix());
	if (m_pColliderSphereCom)
		m_pColliderSphereCom->Update(m_pTransformCom->Get_WorldMatrix());
}

HRESULT CActor::Render()
{
	if (!m_pShaderCom || !m_pModelCom)
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

void CActor::Render_Colliders()
{
	// Render Colliders only in Debug
#ifdef _DEBUG
	if (m_pColliderAABBCom)
		m_pColliderAABBCom->Render();
	if (m_pColliderOBBCom)
		m_pColliderOBBCom->Render();
	if (m_pColliderSphereCom)
		m_pColliderSphereCom->Render();
#endif
}

void CActor::Free()
{
	__super::Free();

	if (m_pColliderAABBCom)
		Safe_Release(m_pColliderAABBCom);
	if (m_pColliderOBBCom)
		Safe_Release(m_pColliderOBBCom);
	if (m_pColliderSphereCom)
		Safe_Release(m_pColliderSphereCom);

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
}