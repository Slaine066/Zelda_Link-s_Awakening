#include "stdafx.h"

#include "MoriblinSwordAttackState.h"
#include "MoriblinSwordIdleState.h"

using namespace MoriblinSword;

CAttackState::CAttackState()
{
}

CMoriblinSwordState * CAttackState::AI_Behavior(CMoriblinSword * pMoriblinSword)
{
	Find_Target(pMoriblinSword, true);

	return nullptr;
}

CMoriblinSwordState * CAttackState::Tick(CMoriblinSword * pMoriblinSword, _float fTimeDelta)
{
	Move(pMoriblinSword, fTimeDelta);

	pMoriblinSword->Get_Model()->Play_Animation(fTimeDelta, m_bIsAnimationFinished, pMoriblinSword->Is_AnimationLoop(pMoriblinSword->Get_Model()->Get_CurrentAnimIndex()));
	pMoriblinSword->Sync_WithNavigationHeight();

	return nullptr;
}

CMoriblinSwordState * CAttackState::LateTick(CMoriblinSword * pMoriblinSword, _float fTimeDelta)
{
	if (!m_bDidDamage)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		vector<CGameObject*> pDamagedObjects;
		pGameInstance->Collision_Check_Group_Multi(CCollision_Manager::COLLISION_GROUP::COLLISION_PLAYER, pMoriblinSword->Get_Collider(CCollider::AIM::AIM_DAMAGE_OUTPUT), CCollider::AIM::AIM_DAMAGE_INPUT, pDamagedObjects);
		RELEASE_INSTANCE(CGameInstance);

		if (!pDamagedObjects.empty())
		{
			for (auto& pDamaged : pDamagedObjects)
				pDamaged->Take_Damage(pMoriblinSword->Get_Stats().m_fAttackPower, nullptr, pMoriblinSword);

			m_bDidDamage = true;
		}
	}

	if (m_bIsAnimationFinished && m_bDidDamage)
		return new CIdleState(m_pTarget);

	return nullptr;
}

void CAttackState::Enter(CMoriblinSword * pMoriblinSword)
{
	pMoriblinSword->Get_Model()->Set_CurrentAnimIndex(CMoriblinSword::ANIMID::ANIM_STANCE_WALK);
}

void CAttackState::Exit(CMoriblinSword * pMoriblinSword)
{
	m_bDidDamage = false;
}

void CAttackState::Move(CMoriblinSword * pMoriblinSword, _float fTimeDelta)
{
	if (!m_pTarget)
		return;

	_vector vTargetPosition = XMVectorSet(m_pTarget->Get_Position().x, pMoriblinSword->Get_Position().y, m_pTarget->Get_Position().z, 1.f);
	pMoriblinSword->Get_Transform()->LookAt(vTargetPosition);

	_float3 vPosition;
	XMStoreFloat3(&vPosition, vTargetPosition);

	_float fAttackRadius = pMoriblinSword->Get_AttackRadius();
	pMoriblinSword->Get_Transform()->Go_TargetPosition(fTimeDelta, vPosition, fAttackRadius, pMoriblinSword->Get_Navigation());
}
