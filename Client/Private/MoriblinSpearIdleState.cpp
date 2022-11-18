#include "stdafx.h"

#include "MoriblinSpearIdleState.h"
#include "MoriblinSpearAttackState.h"
#include "MoriblinSpearMoveState.h"
#include "MoriblinSpearAggroState.h"

using namespace MoriblinSpear;

CIdleState::CIdleState(CPlayer* pTarget)
{
	m_pTarget = pTarget;
}

CMoriblinSpearState * CIdleState::AI_Behavior(CMoriblinSpear * pMoriblinSpear)
{
	Find_Target(pMoriblinSpear);

	return nullptr;
}

CMoriblinSpearState * CIdleState::Tick(CMoriblinSpear * pMoriblinSpear, _float fTimeDelta)
{
	pMoriblinSpear->Get_Model()->Play_Animation(fTimeDelta, m_bIsAnimationFinished, pMoriblinSpear->Is_AnimationLoop(pMoriblinSpear->Get_Model()->Get_CurrentAnimIndex()));
	pMoriblinSpear->Sync_WithNavigationHeight();

	if (m_pTarget)
	{
		_vector vTargetPosition = XMVectorSet(m_pTarget->Get_Position().x, pMoriblinSpear->Get_Position().y, m_pTarget->Get_Position().z, 1.f);
		pMoriblinSpear->Get_Transform()->LookAt(vTargetPosition);

		if (Is_InAttackRadius(pMoriblinSpear))
		{
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
			return new CMoveState(m_pTarget);
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

CMoriblinSpearState * CIdleState::LateTick(CMoriblinSpear * pMoriblinSpear, _float fTimeDelta)
{
	return nullptr;
}

void CIdleState::Enter(CMoriblinSpear * pMoriblinSpear)
{
	if (m_pTarget)
	{
		pMoriblinSpear->Get_Model()->Set_CurrentAnimIndex(CMoriblinSpear::ANIMID::ANIM_STANCE_WAIT);
		m_fIdleAttackTimer = 0.f;
		m_bHasSpottedTarget = true;
	}
	else
		pMoriblinSpear->Get_Model()->Set_CurrentAnimIndex(CMoriblinSpear::ANIMID::ANIM_WAIT);
}

void CIdleState::Exit(CMoriblinSpear * pMoriblinSpear)
{
	m_fIdleMoveTimer = 0.f;
	m_fIdleAttackTimer = 0.f;
}