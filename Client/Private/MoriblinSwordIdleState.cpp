#include "stdafx.h"

#include "MoriblinSwordIdleState.h"
#include "MoriblinSwordAttackState.h"
#include "MoriblinSwordMoveState.h"
#include "MoriblinSwordAggroState.h"

using namespace MoriblinSword;

CIdleState::CIdleState(CPlayer* pTarget) 
{
	m_pTarget = pTarget;
}

CMoriblinSwordState * CIdleState::AI_Behavior(CMoriblinSword * pMoriblinSword)
{
	Find_Target(pMoriblinSword); 

	return nullptr;
}

CMoriblinSwordState * CIdleState::Tick(CMoriblinSword * pMoriblinSword, _float fTimeDelta)
{
	pMoriblinSword->Get_Model()->Play_Animation(fTimeDelta, m_bIsAnimationFinished, pMoriblinSword->Is_AnimationLoop(pMoriblinSword->Get_Model()->Get_CurrentAnimIndex()));
	pMoriblinSword->Sync_WithNavigationHeight();

	if (m_pTarget)
	{
		_vector vTargetPosition = XMVectorSet(m_pTarget->Get_Position().x, pMoriblinSword->Get_Position().y, m_pTarget->Get_Position().z, 1.f);
		pMoriblinSword->Get_Transform()->LookAt(vTargetPosition);

		if (m_fIdleAttackTimer > 2.f)
		{
			if (!m_bHasSpottedTarget)
				return new CAggroState();
			else
				return new CAttackState();
		}
		else
			m_fIdleAttackTimer += fTimeDelta;
	}
	else
	{
		if (m_fIdleMoveTimer > 3.f)
			return new CMoveState();
		else
			m_fIdleMoveTimer += fTimeDelta;
	}

	return nullptr;
}

CMoriblinSwordState * CIdleState::LateTick(CMoriblinSword * pMoriblinSword, _float fTimeDelta)
{
	return nullptr;
}

void CIdleState::Enter(CMoriblinSword * pMoriblinSword)
{
	if (m_pTarget)
	{
		pMoriblinSword->Get_Model()->Set_CurrentAnimIndex(CMoriblinSword::ANIMID::ANIM_STANCE_WAIT);
		m_fIdleAttackTimer = 0.f;
		m_bHasSpottedTarget = true;
	}
	else
		pMoriblinSword->Get_Model()->Set_CurrentAnimIndex(CMoriblinSword::ANIMID::ANIM_WAIT);
}

void CIdleState::Exit(CMoriblinSword * pPlayer)
{
	m_fIdleMoveTimer = 0.f;
	m_fIdleAttackTimer = 0.f;
}