#include "stdafx.h"

#include "MarinTalkState.h"
#include "MarinIdleState.h"

using namespace Marin;

CTalkState::CTalkState()
{
}

CMarinState * CTalkState::AI_Behavior(CMarin * pMarin)
{
	return nullptr;
}

CMarinState * CTalkState::Tick(CMarin * pMarin, _float fTimeDelta)
{
	pMarin->Get_Model()->Play_Animation(fTimeDelta, m_bIsAnimationFinished, pMarin->Is_AnimationLoop(pMarin->Get_Model()->Get_CurrentAnimIndex()));
	pMarin->Sync_WithNavigationHeight();

	return nullptr;
}

CMarinState * CTalkState::LateTick(CMarin * pMarin, _float fTimeDelta)
{
	if (!pMarin->Get_CurrentChat())
		return new CIdleState();

	pMarin->CanInteract();

	return nullptr;
}

void CTalkState::Enter(CMarin * pMarin)
{
	m_eStateId = STATE_ID::STATE_TALK;

	pMarin->Get_Model()->Set_CurrentAnimIndex(CMarin::ANIMID::ANIM_TALK);
}

void CTalkState::Exit(CMarin * pMarin)
{
}