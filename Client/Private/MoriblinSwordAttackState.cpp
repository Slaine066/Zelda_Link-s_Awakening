#include "stdafx.h"

#include "MoriblinSwordAttackState.h"

using namespace MoriblinSword;

CAttackState::CAttackState()
{
}

CMoriblinSwordState * CAttackState::AI_Behavior(CMoriblinSword * pMoriblinSword)
{
	return nullptr;
}

CMoriblinSwordState * CAttackState::Tick(CMoriblinSword * pMoriblinSword, _float fTimeDelta)
{
	pMoriblinSword->Get_Model()->Play_Animation(fTimeDelta, m_bIsAnimationFinished, pMoriblinSword->Is_AnimationLoop(pMoriblinSword->Get_Model()->Get_CurrentAnimIndex()));
	pMoriblinSword->Sync_WithNavigationHeight();

	return nullptr;
}

CMoriblinSwordState * CAttackState::LateTick(CMoriblinSword * pMoriblinSword, _float fTimeDelta)
{
	return nullptr;
}

void CAttackState::Enter(CMoriblinSword * pMoriblinSword)
{
	pMoriblinSword->Get_Model()->Set_CurrentAnimIndex(CMoriblinSword::ANIMID::ANIM_STANCE_WALK);
}

void CAttackState::Exit(CMoriblinSword * pMoriblinSword)
{
}