#include "stdafx.h"

#include "BossblinGuardState.h"
#include "BossblinAttackState.h"
#include "Transform.h"

using namespace Bossblin;

CGuardState::CGuardState()
{
}

CBossblinState * CGuardState::AI_Behavior(CBossblin * pBossblin)
{
	Find_Target(pBossblin, true);

	return nullptr;
}

CBossblinState * CGuardState::Tick(CBossblin * pBossblin, _float fTimeDelta)
{
	pBossblin->Get_Model()->Play_Animation(fTimeDelta, m_bIsAnimationFinished, pBossblin->Is_AnimationLoop(pBossblin->Get_Model()->Get_CurrentAnimIndex()));
	pBossblin->Sync_WithNavigationHeight();

	return nullptr;
}

CBossblinState * CGuardState::LateTick(CBossblin * pBossblin, _float fTimeDelta)
{
	if (m_bIsAnimationFinished)
		return new CAttackState();

	return nullptr;
}

void CGuardState::Enter(CBossblin * pBossblin)
{
	m_eStateId = STATE_ID::STATE_GUARD;

	pBossblin->Get_Model()->Set_CurrentAnimIndex(CBossblin::ANIMID::ANIM_GUARD);
}

void CGuardState::Exit(CBossblin * pBossblin)
{

}