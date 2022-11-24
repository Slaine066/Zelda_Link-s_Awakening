#include "stdafx.h"

#include "BossblinDownState.h"
#include "BossblinIdleState.h"

using namespace Bossblin;

CDownState::CDownState(STATETYPE eStateType, _float fDownTimer) : m_fDownTimer(fDownTimer)
{
	m_eStateType = eStateType;
}

CBossblinState * CDownState::AI_Behavior(CBossblin * pBossblin)
{
	return nullptr;
}

CBossblinState * CDownState::Tick(CBossblin * pBossblin, _float fTimeDelta)
{
	switch (m_eStateType)
	{
	case STATETYPE::STATETYPE_MAIN:
		if (m_fDownTimer > 3.f)
			return new CDownState(STATETYPE::STATETYPE_END);
		else
			m_fDownTimer += fTimeDelta;
		break;
	}
	
	pBossblin->Get_Model()->Play_Animation(fTimeDelta, m_bIsAnimationFinished, pBossblin->Is_AnimationLoop(pBossblin->Get_Model()->Get_CurrentAnimIndex()));
	pBossblin->Sync_WithNavigationHeight();

	return nullptr;
}

CBossblinState * CDownState::LateTick(CBossblin * pBossblin, _float fTimeDelta)
{
	if (m_bIsAnimationFinished)
	{
		switch (m_eStateType)
		{
		case STATETYPE_START:
			return new CDownState(STATETYPE_MAIN);
			break;
		case STATETYPE_END:
			Find_Target(pBossblin, true);
			return new CIdleState(m_pTarget);
			break;
		}
	}

	return nullptr;
}

void CDownState::Enter(CBossblin * pBossblin)
{
	m_eStateId = STATE_ID::STATE_DOWN;

	switch (m_eStateType)
	{
	case STATETYPE_START:
		pBossblin->Get_Model()->Set_CurrentAnimIndex(CBossblin::ANIMID::ANIM_DOWN_START);
		break;
	case STATETYPE_MAIN:
		pBossblin->Get_Model()->Set_CurrentAnimIndex(CBossblin::ANIMID::ANIM_DOWN);
		break;
	case STATETYPE_END:
		pBossblin->Get_Model()->Set_CurrentAnimIndex(CBossblin::ANIMID::ANIM_DOWN_END);
		break;
	}
}

void CDownState::Exit(CBossblin * pBossblin)
{
}