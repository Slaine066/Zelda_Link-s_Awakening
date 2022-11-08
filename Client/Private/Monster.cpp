#include "stdafx.h"

#include "Monster.h"
#include "GameInstance.h"

CMonster::CMonster(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CActor(pDevice, pContext)
{
}

CMonster::CMonster(const CMonster & rhs)
	: CActor(rhs)
{
}

HRESULT CMonster::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_Scale(CTransform::STATE::STATE_RIGHT, .4f);
	m_pTransformCom->Set_Scale(CTransform::STATE::STATE_UP, .4f);
	m_pTransformCom->Set_Scale(CTransform::STATE::STATE_LOOK, .4f);

	return S_OK;
}

_uint CMonster::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	pGameInstance->Add_CollisionGroup(CCollision_Manager::COLLISION_GROUP::COLLISION_MONSTER, this);

	// () ..? AI Function?
	Execute_State(fTimeDelta);	// Execute Action based on STATE.

	m_bIsAnimationFinished = false;
	m_pModelCom->Play_Animation(fTimeDelta, m_bIsAnimationFinished, Is_AnimationLoop(m_pModelCom->Get_CurrentAnimIndex()));

	Reset_State();				// Change STATE when Animation ends.

	return OBJ_NOEVENT;
}

void CMonster::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CMonster::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CMonster::Free()
{
	__super::Free();
}