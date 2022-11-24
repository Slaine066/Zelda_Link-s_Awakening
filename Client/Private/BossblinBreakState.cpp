#include "stdafx.h"

#include "BossblinBreakState.h"
#include "BossblinAttackState.h"
#include "BossblinIdleState.h"
#include "Transform.h"

using namespace Bossblin;

CBreakState::CBreakState()
{
}

CBossblinState * CBreakState::AI_Behavior(CBossblin* pBossblin)
{
	Find_Target(pBossblin, true);

	return nullptr;
}

CBossblinState * CBreakState::Tick(CBossblin* pBossblin, _float fTimeDelta)
{
	pBossblin->Get_Model()->Play_Animation(fTimeDelta, m_bIsAnimationFinished, pBossblin->Is_AnimationLoop(pBossblin->Get_Model()->Get_CurrentAnimIndex()));
	pBossblin->Sync_WithNavigationHeight();

	return nullptr;
}

CBossblinState * CBreakState::LateTick(CBossblin* pBossblin, _float fTimeDelta)
{
	if (m_bIsAnimationFinished)
		return new CIdleState(m_pTarget);

	return nullptr;
}

void CBreakState::Enter(CBossblin* pBossblin)
{
	m_eStateId = STATE_ID::STATE_BREAK;

	pBossblin->Get_Model()->Set_CurrentAnimIndex(CBossblin::ANIMID::ANIM_BRAKE);
}

void CBreakState::Exit(CBossblin* pBossblin)
{

}

void CBreakState::Break(CBossblin * pBossblin, _float fTimeDelta)
{
}
