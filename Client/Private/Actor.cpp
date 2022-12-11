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

	/* -180¡Æ to revert the PivotMatrix and set same Rotation which was set in MapTool. */
	if (m_tModelDesc.eModelType == CModel::TYPE::TYPE_ANIM)
		m_pTransformCom->Set_Rotation(_float3(m_tModelDesc.vRotation.x, m_tModelDesc.vRotation.y - 180.f, m_tModelDesc.vRotation.z)); 

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

_uint CActor::Late_Tick(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	m_bIsInFrustum = pGameInstance->IsIn_Frustum(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), m_fRadius);
	RELEASE_INSTANCE(CGameInstance);

	Update_Colliders();

	return OBJ_NOEVENT;
}

HRESULT CActor::Render()
{
	if (!m_pShaderCom || !m_pModelCom)
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

CCollider * CActor::Get_Collider(CCollider::AIM eAim)
{
	auto iter = find_if(m_vCollidersCom.begin(), m_vCollidersCom.end(), [&](CCollider* pCollider) {
		return pCollider->Get_ColliderDesc().eAim == eAim;
	});

	if (iter == m_vCollidersCom.end())
		return nullptr;

	return *iter;
}

void CActor::Update_Colliders()
{
	for (auto& pCollider : m_vCollidersCom)
		if (pCollider)
			pCollider->Update(m_pTransformCom->Get_WorldMatrix());
}

void CActor::Render_Colliders()
{
	// Render Colliders only in Debug
#ifdef _DEBUG
	for (auto& pCollider : m_vCollidersCom)
		if (pCollider)
			m_pRendererCom->Add_Debug(pCollider);
#endif
}

void CActor::Render_NavigationMesh()
{
	// Render NavigationMesh only in Debug
#ifdef _DEBUG
	if (m_pNavigationCom)
		m_pRendererCom->Add_Debug(m_pNavigationCom);
			//m_pNavigationCom->Render_Navigation();
#endif
}

void CActor::Sync_WithNavigationHeight()
{
	_float3 vPosition;
	XMStoreFloat3(&vPosition, m_pTransformCom->Get_State(CTransform::STATE::STATE_TRANSLATION));
	
	_float fHeight = m_pNavigationCom->Get_NavigationHeight(vPosition);

	vPosition.y = fHeight;
	_vector vNewPosition = XMLoadFloat3(&vPosition);
	vNewPosition = XMVectorSetW(vNewPosition, 1.f);

	m_pTransformCom->Set_State(CTransform::STATE::STATE_TRANSLATION, vNewPosition);
}

void CActor::Free()
{
	__super::Free();

	for (auto& pCollider : m_vCollidersCom)
		if (pCollider)
			Safe_Release(pCollider);

	m_vCollidersCom.clear();

	if (m_pNavigationCom)
		Safe_Release(m_pNavigationCom);

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
}