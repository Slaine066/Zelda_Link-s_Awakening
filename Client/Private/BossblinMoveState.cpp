#include "stdafx.h"

#include "BossblinMoveState.h"
#include "BossblinIdleState.h"
#include "BossblinAttackState.h"
#include "Navigation.h"
#include "Transform.h"

using namespace Bossblin;

CMoveState::CMoveState(CPlayer* pTarget)
{
	m_pTarget = pTarget;
}

CBossblinState * CMoveState::AI_Behavior(CBossblin * pBossblin)
{
	return nullptr;
}

CBossblinState * CMoveState::Tick(CBossblin * pBossblin, _float fTimeDelta)
{
	pBossblin->Get_Model()->Play_Animation(fTimeDelta, m_bIsAnimationFinished, pBossblin->Is_AnimationLoop(pBossblin->Get_Model()->Get_CurrentAnimIndex()));
	pBossblin->Sync_WithNavigationHeight();

	if (m_pTarget)
	{
		if (Is_InAttackRadius(pBossblin))
		{
			if (!pBossblin->Get_IsProjectileAlive())
				return new CAttackState();
			else
				return new CIdleState(m_pTarget);
		}
		else
			Follow_Target(pBossblin, fTimeDelta);
	}

	return nullptr;
}

CBossblinState * CMoveState::LateTick(CBossblin * pBossblin, _float fTimeDelta)
{
	return nullptr;
}

void CMoveState::Enter(CBossblin * pBossblin)
{
	m_eStateId = STATE_ID::STATE_MOVE;

	pBossblin->Get_Model()->Set_CurrentAnimIndex(CBossblin::ANIMID::ANIM_MOVE_JUMP);
}

void CMoveState::Exit(CBossblin * pBossblin)
{

}

void CMoveState::Follow_Target(CBossblin * pBossblin, _float fTimeDelta)
{
	if (!m_pTarget)
		return;

	_vector vTargetPosition = XMVectorSet(m_pTarget->Get_Position().x, pBossblin->Get_Position().y, m_pTarget->Get_Position().z, 1.f);
	pBossblin->Get_Transform()->LookAt(vTargetPosition);

	_float3 vPosition;
	XMStoreFloat3(&vPosition, vTargetPosition);

	pBossblin->Get_Transform()->Move_Straight(fTimeDelta, pBossblin->Get_Navigation(), pBossblin->Get_Radius());
}