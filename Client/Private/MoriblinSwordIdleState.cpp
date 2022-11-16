#include "stdafx.h"

#include "MoriblinSwordIdleState.h"
#include "MoriblinSwordAttackState.h"
#include "MoriblinSwordMoveState.h"

using namespace MoriblinSword;

CIdleState::CIdleState()
{
}

CMoriblinSwordState * CIdleState::AI_Behavior(CMoriblinSword * pMoriblinSword)
{
	return nullptr;
}

CMoriblinSwordState * CIdleState::Tick(CMoriblinSword * pMoriblinSword, _float fTimeDelta)
{
	pMoriblinSword->Get_Model()->Play_Animation(fTimeDelta, m_bIsAnimationFinished, pMoriblinSword->Is_AnimationLoop(pMoriblinSword->Get_Model()->Get_CurrentAnimIndex()));
	pMoriblinSword->Sync_WithNavigationHeight();

	if (m_bHasAggro)
	{
		// TODO: ..
	}
	else
	{
		Find_Target(); // Populate "m_pTarget" 

		if (m_pTarget)
			return new CAttackState();
		else
		{
			if (m_fIdleTimer > 3.f)
				return new CMoveState();
			else
				m_fIdleTimer += fTimeDelta;
		}
	}

	return nullptr;
}

CMoriblinSwordState * CIdleState::LateTick(CMoriblinSword * pMoriblinSword, _float fTimeDelta)
{
	return nullptr;
}

void CIdleState::Enter(CMoriblinSword * pMoriblinSword)
{
	pMoriblinSword->Get_Model()->Set_CurrentAnimIndex(CMoriblinSword::ANIMID::ANIM_WAIT);
}

void CIdleState::Exit(CMoriblinSword * pPlayer)
{

}

void CIdleState::Find_Target()
{
	// TODO: ..
}