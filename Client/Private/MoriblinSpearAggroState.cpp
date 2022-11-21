#include "stdafx.h"

#include "MoriblinSpearAggroState.h"
#include "MoriblinSpearAttackState.h"

using namespace MoriblinSpear;

CAggroState::CAggroState()
{
}

CMoriblinSpearState * CAggroState::AI_Behavior(CMoriblinSpear * pMoriblinSpear)
{
	return nullptr;
}

CMoriblinSpearState * CAggroState::Tick(CMoriblinSpear * pMoriblinSpear, _float fTimeDelta)
{
	pMoriblinSpear->Get_Model()->Play_Animation(fTimeDelta, m_bIsAnimationFinished, pMoriblinSpear->Is_AnimationLoop(pMoriblinSpear->Get_Model()->Get_CurrentAnimIndex()));
	pMoriblinSpear->Sync_WithNavigationHeight();

	return nullptr;
}

CMoriblinSpearState * CAggroState::LateTick(CMoriblinSpear * pMoriblinSpear, _float fTimeDelta)
{
	if (m_bIsAnimationFinished)
		return new CAttackState();

	return nullptr;
}

void CAggroState::Enter(CMoriblinSpear * pMoriblinSpear)
{
	m_eStateId = STATE_ID::STATE_AGGRO;

	pMoriblinSpear->Get_Model()->Set_CurrentAnimIndex(CMoriblinSpear::ANIMID::ANIM_FIND);
}

void CAggroState::Exit(CMoriblinSpear * pMoriblinSpear)
{
}