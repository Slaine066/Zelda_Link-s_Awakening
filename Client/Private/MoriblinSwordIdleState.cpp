#include "stdafx.h"

#include "MoriblinSwordIdleState.h"
#include "MoriblinSwordAttackState.h"
#include "MoriblinSwordMoveState.h"

using namespace MoriblinSword;

CIdleState::CIdleState(_bool bHasAggro) : m_bHasAggro(bHasAggro)
{
}

CMoriblinSwordState * CIdleState::AI_Behavior(CMoriblinSword * pMoriblinSword)
{
	/* Populate "m_pTarget". */
	Find_Target(pMoriblinSword); 

	return nullptr;
}

CMoriblinSwordState * CIdleState::Tick(CMoriblinSword * pMoriblinSword, _float fTimeDelta)
{
	pMoriblinSword->Get_Model()->Play_Animation(fTimeDelta, m_bIsAnimationFinished, pMoriblinSword->Is_AnimationLoop(pMoriblinSword->Get_Model()->Get_CurrentAnimIndex()));
	pMoriblinSword->Sync_WithNavigationHeight();

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

CMoriblinSwordState * CIdleState::LateTick(CMoriblinSword * pMoriblinSword, _float fTimeDelta)
{
	return nullptr;
}

void CIdleState::Enter(CMoriblinSword * pMoriblinSword)
{
	if (m_bHasAggro)
	{
		pMoriblinSword->Get_Model()->Set_CurrentAnimIndex(CMoriblinSword::ANIMID::ANIM_STANCE_WAIT);
		m_fIdleAttackTimer = 0.f;
	}
	else
		pMoriblinSword->Get_Model()->Set_CurrentAnimIndex(CMoriblinSword::ANIMID::ANIM_WAIT);
}

void CIdleState::Exit(CMoriblinSword * pPlayer)
{
	m_fIdleMoveTimer = 0.f;
	m_fIdleAttackTimer = 0.f;
}