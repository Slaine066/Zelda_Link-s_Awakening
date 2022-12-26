#include "stdafx.h"

#include "BossblinIdleState.h"
#include "BossblinAttackState.h"
#include "BossblinTackleState.h"
#include "BossblinMoveState.h"

using namespace Bossblin;

CIdleState::CIdleState(CPlayer* pTarget)
{
	m_pTarget = pTarget;
}

CBossblinState * CIdleState::AI_Behavior(CBossblin * pBossblin)
{
	Find_Target(pBossblin);

	return nullptr;
}

CBossblinState * CIdleState::Tick(CBossblin * pBossblin, _float fTimeDelta)
{
	pBossblin->Get_Model()->Play_Animation(fTimeDelta, m_bIsAnimationFinished, pBossblin->Is_AnimationLoop(pBossblin->Get_Model()->Get_CurrentAnimIndex()));
	pBossblin->Sync_WithNavigationHeight();

	if (m_pTarget)
	{
		_vector vTargetPosition = XMVectorSet(m_pTarget->Get_Position().x, pBossblin->Get_Position().y, m_pTarget->Get_Position().z, 1.f);
		pBossblin->Get_Transform()->LookAt(vTargetPosition);

		if (Is_InAttackRadius(pBossblin))
		{
			if (m_fIdleAttackTimer > 1.5f)
			{
				_int iAttackType = rand() % 2;
				if (iAttackType == 0)
					return new CTackleState(STATETYPE::STATETYPE_START);
				else
					return new CAttackState();
			}
			else
				m_fIdleAttackTimer += fTimeDelta;
		}
		else
			return new CMoveState(m_pTarget);
	}

	return nullptr;
}

CBossblinState * CIdleState::LateTick(CBossblin * pBossblin, _float fTimeDelta)
{
	return nullptr;
}

void CIdleState::Enter(CBossblin * pBossblin)
{
	m_eStateId = STATE_ID::STATE_IDLE;

	if (m_pTarget)
		m_fIdleAttackTimer = 0.f;

	pBossblin->Get_Model()->Set_CurrentAnimIndex(CBossblin::ANIMID::ANIM_WAIT);
}

void CIdleState::Exit(CBossblin * pPlayer)
{
	m_fIdleAttackTimer = 0.f;
}