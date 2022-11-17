#include "stdafx.h"

#include "MoriblinSpearAttackState.h"
#include "MoriblinSpearIdleState.h"
#include "MoriblinSpearMoveState.h"

using namespace MoriblinSpear;

CAttackState::CAttackState()
{
}

CMoriblinSpearState * CAttackState::AI_Behavior(CMoriblinSpear * pMoriblinSpear)
{
	Find_Target(pMoriblinSpear, true);

	if (!Is_InAttackRadius(pMoriblinSpear))
	{
		pMoriblinSpear->Get_Model()->Reset_CurrentAnimation();
		return new CMoveState(m_pTarget);
	}

	return nullptr;
}

CMoriblinSpearState * CAttackState::Tick(CMoriblinSpear * pMoriblinSpear, _float fTimeDelta)
{
	_vector vTargetPosition = XMVectorSet(m_pTarget->Get_Position().x, pMoriblinSpear->Get_Position().y, m_pTarget->Get_Position().z, 1.f);
	pMoriblinSpear->Get_Transform()->LookAt(vTargetPosition);

	pMoriblinSpear->Get_Model()->Play_Animation(fTimeDelta, m_bIsAnimationFinished, pMoriblinSpear->Is_AnimationLoop(pMoriblinSpear->Get_Model()->Get_CurrentAnimIndex()));
	pMoriblinSpear->Sync_WithNavigationHeight();

	return nullptr;
}

CMoriblinSpearState * CAttackState::LateTick(CMoriblinSpear * pMoriblinSpear, _float fTimeDelta)
{
	if (!m_bDidDamage)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		vector<CGameObject*> pDamagedObjects;
		pGameInstance->Collision_Check_Group_Multi(CCollision_Manager::COLLISION_GROUP::COLLISION_PLAYER, pMoriblinSpear->Get_Collider(CCollider::AIM::AIM_DAMAGE_OUTPUT), CCollider::AIM::AIM_DAMAGE_INPUT, pDamagedObjects);
		RELEASE_INSTANCE(CGameInstance);

		if (!pDamagedObjects.empty())
		{
			for (auto& pDamaged : pDamagedObjects)
				pDamaged->Take_Damage(pMoriblinSpear->Get_Stats().m_fAttackPower, nullptr, pMoriblinSpear);

			m_bDidDamage = true;
		}
	}

	if (m_bIsAnimationFinished)
		return new CIdleState(m_pTarget);

	return nullptr;
}

void CAttackState::Enter(CMoriblinSpear * pMoriblinSpear)
{
	pMoriblinSpear->Get_Model()->Set_CurrentAnimIndex(CMoriblinSpear::ANIMID::ANIM_SPEAR_THROW);
}

void CAttackState::Exit(CMoriblinSpear * pMoriblinSpear)
{
	m_bDidDamage = false;
}

void CAttackState::Move(CMoriblinSpear * pMoriblinSpear, _float fTimeDelta)
{
	if (!m_pTarget)
		return;

	_vector vTargetPosition = XMVectorSet(m_pTarget->Get_Position().x, pMoriblinSpear->Get_Position().y, m_pTarget->Get_Position().z, 1.f);
	pMoriblinSpear->Get_Transform()->LookAt(vTargetPosition);

	_float3 vPosition;
	XMStoreFloat3(&vPosition, vTargetPosition);

	_float fAttackRadius = pMoriblinSpear->Get_AttackRadius();
	pMoriblinSpear->Get_Transform()->Go_TargetPosition(fTimeDelta, vPosition, fAttackRadius, pMoriblinSpear->Get_Navigation());
}