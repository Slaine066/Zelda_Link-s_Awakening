#include "stdafx.h"

#include "MoriblinSpearIdleState.h"
#include "MoriblinSpearAttackState.h"
#include "MoriblinSpearMoveState.h"

using namespace MoriblinSpear;

CIdleState::CIdleState(_bool bHasAggro) : m_bHasAggro(bHasAggro)
{
}

CMoriblinSpearState * CIdleState::AI_Behavior(CMoriblinSpear * pMoriblinSpear)
{
	/* Populate "m_pTarget". */
	Find_Target(pMoriblinSpear);

	return nullptr;
}

CMoriblinSpearState * CIdleState::Tick(CMoriblinSpear * pMoriblinSpear, _float fTimeDelta)
{
	pMoriblinSpear->Get_Model()->Play_Animation(fTimeDelta, m_bIsAnimationFinished, pMoriblinSpear->Is_AnimationLoop(pMoriblinSpear->Get_Model()->Get_CurrentAnimIndex()));
	pMoriblinSpear->Sync_WithNavigationHeight();

	if (m_pTarget)
	{
		if (m_fIdleAttackTimer > 2.f)
			return new CAttackState();
		else
			m_fIdleAttackTimer += fTimeDelta;
	}
	else
	{
		if (m_fIdleMoveTimer > 3.f)
			return new CMoveState();
		else
			m_fIdleMoveTimer += fTimeDelta;
	}

	return nullptr;
}

CMoriblinSpearState * CIdleState::LateTick(CMoriblinSpear * pMoriblinSpear, _float fTimeDelta)
{
	return nullptr;
}

void CIdleState::Enter(CMoriblinSpear * pMoriblinSpear)
{
	if (m_bHasAggro)
	{
		pMoriblinSpear->Get_Model()->Set_CurrentAnimIndex(CMoriblinSpear::ANIMID::ANIM_STANCE_WAIT);
		m_fIdleAttackTimer = 0.f;
	}
	else
		pMoriblinSpear->Get_Model()->Set_CurrentAnimIndex(CMoriblinSpear::ANIMID::ANIM_WAIT);
}

void CIdleState::Exit(CMoriblinSpear * pPlayer)
{
	m_fIdleMoveTimer = 0.f;
	m_fIdleAttackTimer = 0.f;
}