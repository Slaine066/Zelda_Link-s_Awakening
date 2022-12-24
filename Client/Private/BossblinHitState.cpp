#include "stdafx.h"

#include "BossblinHitState.h"
#include "BossblinAttackState.h"
#include "BossblinIdleState.h"
#include "BossblinDownState.h"

using namespace Bossblin;

CHitState::CHitState(_float fDownTimer) : m_fDownTimer(fDownTimer)
{
}

CBossblinState * CHitState::AI_Behavior(CBossblin* pBossblin)
{
	Find_Target(pBossblin, true);

	return nullptr;
}

CBossblinState * CHitState::Tick(CBossblin* pBossblin, _float fTimeDelta)
{
	if (m_fDownTimer > 3.f)
		return new CDownState(STATETYPE::STATETYPE_END);
	else
		m_fDownTimer += fTimeDelta;

	pBossblin->Get_Model()->Play_Animation(fTimeDelta, m_bIsAnimationFinished, pBossblin->Is_AnimationLoop(pBossblin->Get_Model()->Get_CurrentAnimIndex()));
	pBossblin->Sync_WithNavigationHeight();

	return nullptr;
}

CBossblinState * CHitState::LateTick(CBossblin* pBossblin, _float fTimeDelta)
{
	if (m_bIsAnimationFinished)
		return new CDownState(STATETYPE::STATETYPE_MAIN, m_fDownTimer);

	return nullptr;
}

void CHitState::Enter(CBossblin* pBossblin)
{
	m_eStateId = STATE_ID::STATE_HIT;

	pBossblin->Set_ShaderPass(VTXANIMMODELPASS::VTXANIMMODEL_HIT);
	pBossblin->Reset_HitTimer();

	pBossblin->Get_Model()->Set_CurrentAnimIndex(CBossblin::ANIMID::ANIM_DAMAGE);
}

void CHitState::Exit(CBossblin* pBossblin)
{

}