#include "stdafx.h"

#include "Treasure.h"
#include "GameInstance.h"
#include "Player.h"

CTreasure::CTreasure(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CInteractableObject(pDevice, pContext)
{
}

CTreasure::CTreasure(const CTreasure & rhs)
	: CInteractableObject(rhs)
{
}

HRESULT CTreasure::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTreasure::Initialize(void * pArg)
{
	/*if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;*/

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	m_eInteractableType = INTERACTABLE_TREASURE;
	m_pModelCom->Set_CurrentAnimIndex(ANIMID::ANIM_CLOSE_IDLE);

	return S_OK;
}

_uint CTreasure::Tick(_float fTimeDelta)
{
	_uint iEvent = __super::Tick(fTimeDelta);
	
	if (iEvent == OBJ_STOP)
		return iEvent;

	CGameInstance::Get_Instance()->Add_CollisionGroup(CCollision_Manager::COLLISION_GROUP::COLLISION_OBJECT, this);

	m_pModelCom->Play_Animation(fTimeDelta, m_bIsAnimationFinished, Is_AnimationLoop(m_pModelCom->Get_CurrentAnimIndex()));

	return S_OK;
}

_uint CTreasure::Late_Tick(_float fTimeDelta)
{
	_uint iEvent = __super::Late_Tick(fTimeDelta);

	if (iEvent == OBJ_STOP)
		return iEvent;

	if (CanInteract())
	{
		/* TODO: Show Open Button */
	}

	Reset_Animation();

	return S_OK;
}

HRESULT CTreasure::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshContainers();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pModelCom->SetUp_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS);
		m_pModelCom->SetUp_Material(m_pShaderCom, "g_SpecularTexture", i, aiTextureType_SPECULAR);

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 0)))
			return E_FAIL;
	}

	Render_Colliders();

	return S_OK;
}

HRESULT CTreasure::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg)))
		return E_FAIL;
	
	/* Add Specific Components here (Example: Colliders, etc..) */
	CCollider::COLLIDERDESC	ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.eAim = CCollider::AIM::AIM_OBJECT;
	ColliderDesc.vScale = _float3(2.f, 2.f, 2.f);
	ColliderDesc.vPosition = _float3(0.f, .5f, 0.f);
	ColliderDesc.m_bIsAttachedToBone = false;

	m_vCollidersCom.resize(2); // Numbers of Colliders needed for this Object

	/* For.Com_Collider*/
	if (FAILED(__super::Add_Components(TEXT("Com_ColliderTreasure"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), (CComponent**)&m_vCollidersCom[0], &ColliderDesc)))
		return E_FAIL;

	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.eAim = CCollider::AIM::AIM_BLOCK;
	ColliderDesc.vScale = _float3(1.4f, 1.f, 1.5f);
	ColliderDesc.vPosition = _float3(0.f, 0.5f, 0.f);
	ColliderDesc.m_bIsAttachedToBone = false;

	/* For.Com_Collider*/
	if (FAILED(__super::Add_Components(TEXT("Com_ColliderTreasureBlock"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"), (CComponent**)&m_vCollidersCom[1], &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTreasure::SetUp_ShaderResources()
{
	if (FAILED(__super::SetUp_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

_bool CTreasure::CanInteract()
{
	if (!m_bDidInteract && m_pModelCom->Get_CurrentAnimIndex() == ANIM_CLOSE_IDLE && Is_PlayerInRadius() == true)
			return true;
}

void CTreasure::Interact()
{
	/* Open Treasure */
	if (CanInteract())
	{
		/* If Player has Key */
		if (true /* TODO: Check if Player has Key */)
		{
			m_pModelCom->Set_CurrentAnimIndex(ANIM_OPEN);
			m_bDidInteract = true;
		}
		else
		{
			m_pModelCom->Set_CurrentAnimIndex(ANIM_OPEN_FAIL);
			m_bDidInteract = true;
		}
	}
}

_bool CTreasure::Is_AnimationLoop(_uint eAnimId)
{
	switch ((ANIMID)eAnimId)
	{
	case ANIM_APPEAR:
	case ANIM_CLOSE:
	case ANIM_OPEN_FAIL:
	case ANIM_OPEN:
		return false;
		break;
	case ANIM_CLOSE_IDLE:
	case ANIM_OPEN_IDLE:
		return true;
		break;
	}
}

_bool CTreasure::Is_PlayerInRadius()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pCollided = nullptr;
	_bool bIsInRadius = pGameInstance->Collision_with_Group(CCollision_Manager::COLLISION_PLAYER, Get_Collider(CCollider::AIM_OBJECT), CCollider::AIM_DAMAGE_INPUT, pCollided);
	
	CPlayer* pPlayer = (CPlayer*)pGameInstance->Find_Object(pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Player"));
	if (bIsInRadius && pPlayer)
		pPlayer->Set_InteractableObject(this);
	else
		pPlayer->Set_InteractableObject(nullptr);

	RELEASE_INSTANCE(CGameInstance);

	return bIsInRadius;
}

void CTreasure::Reset_Animation()
{
	if (m_bIsAnimationFinished)
	{
		switch (m_pModelCom->Get_CurrentAnimIndex())
		{
		case ANIM_OPEN:
			m_pModelCom->Set_CurrentAnimIndex(ANIM_OPEN_IDLE);
			break;
		case ANIM_OPEN_FAIL:
			m_pModelCom->Set_CurrentAnimIndex(ANIM_CLOSE_IDLE);
			m_bDidInteract = false;
			break;
		case ANIM_CLOSE:
			m_pModelCom->Set_CurrentAnimIndex(ANIM_CLOSE_IDLE);
			break;
		}
	}
}

CTreasure* CTreasure::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTreasure* pInstance = new CTreasure(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Create: CTreasure"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTreasure::Clone(void * pArg)
{
	CTreasure* pInstance = new CTreasure(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Clone: CTreasure"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTreasure::Free()
{
	__super::Free();
}