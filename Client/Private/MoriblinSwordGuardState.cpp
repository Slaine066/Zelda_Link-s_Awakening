#include "stdafx.h"

#include "MoriblinSwordGuardState.h"
#include "MoriblinSwordAttackState.h"
#include "Transform.h"

using namespace MoriblinSword;

CGuardState::CGuardState()
{
}

CMoriblinSwordState * CGuardState::AI_Behavior(CMoriblinSword * pMoriblinSword)
{
	Find_Target(pMoriblinSword, true);

	return nullptr;
}

CMoriblinSwordState * CGuardState::Tick(CMoriblinSword * pMoriblinSword, _float fTimeDelta)
{
	pMoriblinSword->Get_Model()->Play_Animation(fTimeDelta, m_bIsAnimationFinished, pMoriblinSword->Is_AnimationLoop(pMoriblinSword->Get_Model()->Get_CurrentAnimIndex()));
	pMoriblinSword->Sync_WithNavigationHeight();

	return nullptr;
}

CMoriblinSwordState * CGuardState::LateTick(CMoriblinSword * pMoriblinSword, _float fTimeDelta)
{
	if (m_bIsAnimationFinished)
		return new CAttackState();

	return nullptr;
}

void CGuardState::Enter(CMoriblinSword * pMoriblinSword)
{
	m_eStateId = STATE_ID::STATE_GUARD;

	pMoriblinSword->Get_Model()->Set_CurrentAnimIndex(CMoriblinSword::ANIMID::ANIM_GUARD);
}

void CGuardState::Exit(CMoriblinSword * pMoriblinSword)
{

}