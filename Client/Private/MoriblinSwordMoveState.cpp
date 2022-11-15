#include "stdafx.h"

#include "MoriblinSwordMoveState.h"

using namespace MoriblinSword;

CMoveState::CMoveState(DIRID eDir) : m_eDirection(eDir)
{
}

CMoriblinSwordState * CMoveState::AI_Behavior(CMoriblinSword * pMoriblinSword)
{
	return nullptr;
}

CMoriblinSwordState * CMoveState::Tick(CMoriblinSword * pMoriblinSword, _float fTimeDelta)
{
	pMoriblinSword->Get_Model()->Play_Animation(fTimeDelta, m_bIsAnimationFinished, pMoriblinSword->Is_AnimationLoop(pMoriblinSword->Get_Model()->Get_CurrentAnimIndex()));
	pMoriblinSword->Sync_WithNavigationHeight();

	return nullptr;
}

CMoriblinSwordState * CMoveState::LateTick(CMoriblinSword * pMoriblinSword, _float fTimeDelta)
{
	return nullptr;
}

void CMoveState::Enter(CMoriblinSword * pMoriblinSword)
{
	pMoriblinSword->Get_Model()->Set_CurrentAnimIndex(CMoriblinSword::ANIMID::ANIM_WALK);
}

void CMoveState::Exit(CMoriblinSword * pMoriblinSword)
{

}