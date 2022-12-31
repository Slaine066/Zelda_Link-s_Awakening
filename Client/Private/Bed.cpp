#include "stdafx.h"

#include "Bed.h"
#include "GameInstance.h"
#include "Player.h"
#include "PlayerState.h"

CBed::CBed(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CInteractableObject(pDevice, pContext)
{
}

CBed::CBed(const CBed & rhs)
	: CInteractableObject(rhs)
{
}

HRESULT CBed::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBed::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	return E_FAIL;

	m_eInteractableType = INTERACTABLE_BED;
	m_pModelCom->Set_CurrentAnimIndex(ANIMID::ANIM_IDLE);

	return S_OK;
}

_uint CBed::Tick(_float fTimeDelta)
{
	_uint iEvent = __super::Tick(fTimeDelta);

	if (iEvent == OBJ_STOP)
		return iEvent;

	CGameInstance::Get_Instance()->Add_CollisionGroup(CCollision_Manager::COLLISION_GROUP::COLLISION_OBJECT, this);

	m_pModelCom->Play_Animation(fTimeDelta, m_bIsAnimationFinished, Is_AnimationLoop(m_pModelCom->Get_CurrentAnimIndex()));

	return S_OK;
}

_uint CBed::Late_Tick(_float fTimeDelta)
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

HRESULT CBed::Render()
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

HRESULT CBed::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg)))
		return E_FAIL;

	CCollider::COLLIDERDESC	ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.eAim = CCollider::AIM::AIM_OBJECT;
	ColliderDesc.vScale = _float3(3.f, 3.f, 3.f);
	ColliderDesc.vPosition = _float3(0.f, .5f, 0.f);
	ColliderDesc.m_bIsAttachedToBone = false;

	m_vCollidersCom.resize(2); // Numbers of Colliders needed for this Object

	/* For.Com_Collider*/
	if (FAILED(__super::Add_Components(TEXT("Com_ColliderBed"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), (CComponent**)&m_vCollidersCom[0], &ColliderDesc)))
		return E_FAIL;

	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.eAim = CCollider::AIM::AIM_BLOCK;
	ColliderDesc.vScale = _float3(1.6f, .5f, 2.f);
	ColliderDesc.vPosition = _float3(0.f, 0.5f, 0.f);
	ColliderDesc.m_bIsAttachedToBone = false;

	/* For.Com_Collider*/
	if (FAILED(__super::Add_Components(TEXT("Com_ColliderBedBlock"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"), (CComponent**)&m_vCollidersCom[1], &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBed::SetUp_ShaderResources()
{
	if (FAILED(__super::SetUp_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

_bool CBed::CanInteract()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CPlayer* pPlayer = (CPlayer*)pGameInstance->Find_Object(pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Player"));
	if (!pPlayer)
		return false;

	if (m_bDidInteract)
	{
		pPlayer->Set_InteractableObject(nullptr);
		RELEASE_INSTANCE(CGameInstance);
		return false;
	}

	CGameObject* pCollided = nullptr;
	_bool bCollided = pGameInstance->Collision_with_Group(CCollision_Manager::COLLISION_PLAYER, Get_Collider(CCollider::AIM_OBJECT), CCollider::AIM_DAMAGE_INPUT, pCollided);

	RELEASE_INSTANCE(CGameInstance);

	if (bCollided && m_pModelCom->Get_CurrentAnimIndex() == ANIM_IDLE && !m_bDidInteract)
	{
		pPlayer->Set_InteractableObject(this);
		return true;
	}
	else
	{
		pPlayer->Set_InteractableObject(nullptr);
		return false;
	}
}

void CBed::Interact()
{
	/* Go to Sleep */
	m_pModelCom->Set_CurrentAnimIndex(ANIM_SLEEP_START);
	m_bDidInteract = true;
}

_bool CBed::Is_AnimationLoop(_uint eAnimId)
{
	switch ((ANIMID)eAnimId)
	{
		case ANIM_OUTBED:
		case ANIM_WAKEUP_START:
		case ANIM_SLEEP_END:
		case ANIM_SLEEP_START:
			return false;
			break;
		case ANIM_IDLE_SLEPT:
		case ANIM_SLEEP_LOOP:
		case ANIM_WAKEUP_LOOP:
		case ANIM_IDLE:
		case ANIM_SLEEP_LOOP_:
		case ANIM_TARIN_SLEEP:
			return true;
			break;
	}
}

void CBed::Reset_Animation()
{
	if (m_bIsAnimationFinished)
	{
		switch (m_pModelCom->Get_CurrentAnimIndex())
		{
			case ANIM_OUTBED:
				m_pModelCom->Set_CurrentAnimIndex(ANIM_IDLE_SLEPT);
				break;
		}
	}
}

CBed* CBed::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBed* pInstance = new CBed(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Create: CBed"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBed::Clone(void * pArg)
{
	CBed* pInstance = new CBed(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Clone: CBed"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBed::Free()
{
	__super::Free();
}