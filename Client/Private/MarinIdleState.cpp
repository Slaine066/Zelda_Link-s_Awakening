#include "stdafx.h"

#include "MarinIdleState.h"

using namespace Marin;

CIdleState::CIdleState()
{
}

CMarinState * CIdleState::AI_Behavior(CMarin * pMarin)
{
	return nullptr;
}

CMarinState * CIdleState::Tick(CMarin * pMarin, _float fTimeDelta)
{
	pMarin->Get_Model()->Play_Animation(fTimeDelta, m_bIsAnimationFinished, pMarin->Is_AnimationLoop(pMarin->Get_Model()->Get_CurrentAnimIndex()));
	pMarin->Sync_WithNavigationHeight();

	/* TODO: .. */

	return nullptr;
}

CMarinState * CIdleState::LateTick(CMarin * pMarin, _float fTimeDelta)
{
	return nullptr;
}

void CIdleState::Enter(CMarin * pMarin)
{
	m_eStateId = STATE_ID::STATE_IDLE;

	pMarin->Get_Model()->Set_CurrentAnimIndex(CMarin::ANIMID::ANIM_WAIT);
}

void CIdleState::Exit(CMarin * pMarin)
{
	m_fIdleMoveTimer = 0.f;
}