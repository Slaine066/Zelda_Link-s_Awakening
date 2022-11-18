#include "stdafx.h"

#include "MoriblinSwordAggroState.h"
#include "MoriblinSwordAttackState.h"

using namespace MoriblinSword;

CAggroState::CAggroState()
{
}

CMoriblinSwordState * CAggroState::AI_Behavior(CMoriblinSword * pMoriblinSword)
{
	return nullptr;
}

CMoriblinSwordState * CAggroState::Tick(CMoriblinSword * pMoriblinSword, _float fTimeDelta)
{
	pMoriblinSword->Get_Model()->Play_Animation(fTimeDelta, m_bIsAnimationFinished, pMoriblinSword->Is_AnimationLoop(pMoriblinSword->Get_Model()->Get_CurrentAnimIndex()));
	pMoriblinSword->Sync_WithNavigationHeight();

	return nullptr;
}

CMoriblinSwordState * CAggroState::LateTick(CMoriblinSword * pMoriblinSword, _float fTimeDelta)
{
	if (m_bIsAnimationFinished)
		return new CAttackState();

	return nullptr;
}

void CAggroState::Enter(CMoriblinSword * pMoriblinSword)
{
	pMoriblinSword->Get_Model()->Set_CurrentAnimIndex(CMoriblinSword::ANIMID::ANIM_FIND);
}

void CAggroState::Exit(CMoriblinSword * pMoriblinSword)
{
}