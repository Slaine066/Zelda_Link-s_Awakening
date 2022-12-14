#include "stdafx.h"
//
//#include "FallingTile.h"
//#include "GameInstance.h"
//
//CFallingTile::CFallingTile(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
//	: CInteractableObject(pDevice, pContext)
//{
//}
//
//CFallingTile::CFallingTile(const CFallingTile & rhs)
//	: CInteractableObject(rhs)
//{
//}
//
//HRESULT CFallingTile::Initialize_Prototype()
//{
//	return S_OK;
//}
//
//HRESULT CFallingTile::Initialize(void * pArg)
//{
//	/*if (FAILED(__super::Initialize(pArg)))
//	return E_FAIL;*/
//
//	if (FAILED(Ready_Components(pArg)))
//		return E_FAIL;
//
//	return S_OK;
//}
//
//_uint CFallingTile::Tick(_float fTimeDelta)
//{
//	_uint iEvent = __super::Tick(fTimeDelta);
//	
//  if (iEvent == OBJ_STOP)
//		return iEvent;
//
//	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
//	pGameInstance->Add_CollisionGroup(CCollision_Manager::COLLISION_GROUP::COLLISION_OBJECT, this);
//
//	if (m_bShouldDestroy)
//		return OBJ_DESTROY;
//
//	return OBJ_NOEVENT;
//}
//
//_uint CFallingTile::Late_Tick(_float fTimeDelta)
//{
//	_uint iEvent = __super::Late_Tick(fTimeDelta);
//		return E_FAIL;
//
//  if (iEvent == OBJ_STOP)
//		return iEvent;
//
//	return OBJ_NOEVENT;
//}
//
//HRESULT CFallingTile::Render()
//{
//	if (FAILED(__super::Render()))
//		return E_FAIL;
//
//	_uint iNumMeshes = m_pModelCom->Get_NumMeshContainers();
//
//	for (_uint i = 0; i < iNumMeshes; ++i)
//	{
//		if (FAILED(m_pModelCom->SetUp_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
//			return E_FAIL;
//
//		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 0)))
//			return E_FAIL;
//	}
//
//	Render_Colliders();
//
//	return S_OK;
//}
//
//_bool CFallingTile::CheckCollision(CCollision_Manager::COLLISION_GROUP eCollisionGroup, _float fTimeDelta)
//{
//	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
//	CGameObject* pCollided = nullptr;
//	_bool bIsCollided = pGameInstance->Collision_with_Group(eCollisionGroup, Get_Collider(CCollider::AIM::AIM_OBJECT), CCollider::AIM::AIM_DAMAGE_INPUT, pCollided);
//	RELEASE_INSTANCE(CGameInstance);
//
//	if (bIsCollided)
//		m_fDestroyTimer += fTimeDelta;
//	else
//		m_fDestroyTimer = 0.f;
//
//	return bIsCollided;
//}
//
//_bool CFallingTile::ShouldDestroy()
//{
//	if (m_fDestroyTimer > 1.f)
//	{
//		m_bShouldDestroy = true;
//		m_fDestroyTimer = 0.f;
//
//		return true;
//	}
//
//	return false;
//}
//
//HRESULT CFallingTile::Ready_Components(void* pArg)
//{
//	if (FAILED(__super::Ready_Components(pArg)))
//		return E_FAIL;
//
//	CCollider::COLLIDERDESC	ColliderDesc;
//	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
//	ColliderDesc.eAim = CCollider::AIM::AIM_OBJECT;
//	ColliderDesc.vScale = _float3(1.5f, .2f, 1.5f);
//	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
//
//	m_vCollidersCom.resize(1); // Numbers of Colliders needed for this Object
//
//	/* For.Com_Collider*/
//	if (FAILED(__super::Add_Components(TEXT("Com_Collider"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"), (CComponent**)&m_vCollidersCom[0], &ColliderDesc)))
//		return E_FAIL;
//
//	return S_OK;
//}
//
//HRESULT CFallingTile::SetUp_ShaderResources()
//{
//	if (FAILED(__super::SetUp_ShaderResources()))
//		return E_FAIL;
//
//	return S_OK;
//}
//
//CFallingTile* CFallingTile::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
//{
//	CFallingTile* pInstance = new CFallingTile(pDevice, pContext);
//
//	if (FAILED(pInstance->Initialize_Prototype()))
//	{
//		ERR_MSG(TEXT("Failed to Create: CFallingTile"));
//		Safe_Release(pInstance);
//	}
//
//	return pInstance;
//}
//
//CGameObject* CFallingTile::Clone(void * pArg)
//{
//	CFallingTile* pInstance = new CFallingTile(*this);
//
//	if (FAILED(pInstance->Initialize(pArg)))
//	{
//		ERR_MSG(TEXT("Failed to Clone: CFallingTile"));
//		Safe_Release(pInstance);
//	}
//
//	return pInstance;
//}
//
//void CFallingTile::Free()
//{
//	__super::Free();
//}