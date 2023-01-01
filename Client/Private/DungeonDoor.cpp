#include "stdafx.h"

#include "DungeonDoor.h"
#include "GameInstance.h"

CDungeonDoor::CDungeonDoor(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CDynamicObject(pDevice, pContext)
{
}

CDungeonDoor::CDungeonDoor(const CDungeonDoor & rhs)
	: CDynamicObject(rhs)
{
}

HRESULT CDungeonDoor::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDungeonDoor::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pModelCom->Set_CurrentAnimIndex(ANIMID::ANIM_CLOSE);

	return S_OK;
}

_uint CDungeonDoor::Tick(_float fTimeDelta)
{
	_uint iEvent = __super::Tick(fTimeDelta);

	if (iEvent == OBJ_STOP)
		return iEvent;

	CGameInstance::Get_Instance()->Add_CollisionGroup(CCollision_Manager::COLLISION_GROUP::COLLISION_OBJECT, this);

	if (!m_bIsAnimationFinished)
		m_pModelCom->Play_Animation(fTimeDelta, m_bIsAnimationFinished, Is_AnimationLoop(m_pModelCom->Get_CurrentAnimIndex()));

	return S_OK;
}

_uint CDungeonDoor::Late_Tick(_float fTimeDelta)
{
	_uint iEvent = __super::Late_Tick(fTimeDelta);

	if (iEvent == OBJ_STOP)
		return iEvent;

	Reset_Animation();

	return S_OK;
}

HRESULT CDungeonDoor::Render()
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

HRESULT CDungeonDoor::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg)))
		return E_FAIL;

	m_vCollidersCom.resize(1); // Numbers of Colliders needed for this Object

	CCollider::COLLIDERDESC	ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.eAim = CCollider::AIM::AIM_BLOCK;
	ColliderDesc.vScale = _float3(1.4f, 1.5f, 1.f);
	ColliderDesc.vPosition = _float3(0.f, .75f, 0.f);
	ColliderDesc.m_bIsAttachedToBone = false;

	/* For.Com_Collider*/
	if (FAILED(__super::Add_Components(TEXT("Com_ColliderTreasureBlock"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"), (CComponent**)&m_vCollidersCom[0], &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CDungeonDoor::SetUp_ShaderResources()
{
	if (FAILED(__super::SetUp_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

_bool CDungeonDoor::Is_AnimationLoop(_uint eAnimId)
{
	switch ((ANIMID)eAnimId)
	{
	case ANIM_CLOSE:
	case ANIM_CLOSE_WIDE:
	case ANIM_OPEN:
	case ANIM_OPEN_WIDE:
		return false;
		break;
	case ANIM_OPEN_IDLE:
	case ANIM_OPEN_IDLE_WIDE:
		return true;
		break;
	}
}

void CDungeonDoor::Reset_Animation()
{
	if (m_bIsAnimationFinished)
	{
		
	}
}

CDungeonDoor* CDungeonDoor::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CDungeonDoor* pInstance = new CDungeonDoor(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Create: CDungeonDoor"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDungeonDoor::Clone(void * pArg)
{
	CDungeonDoor* pInstance = new CDungeonDoor(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Clone: CDungeonDoor"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDungeonDoor::Free()
{
	__super::Free();
}