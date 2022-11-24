#include "stdafx.h"

#include "MoriblinSwordMoveState.h"
#include "MoriblinSwordIdleState.h"
#include "Navigation.h"
#include "Transform.h"

using namespace MoriblinSword;

CMoveState::CMoveState(CPlayer* pTarget)
{
	m_pTarget = pTarget;
}

CMoriblinSwordState * CMoveState::AI_Behavior(CMoriblinSword * pMoriblinSword)
{
	return nullptr;
}

CMoriblinSwordState * CMoveState::Tick(CMoriblinSword * pMoriblinSword, _float fTimeDelta)
{
	Patrol(pMoriblinSword, fTimeDelta);

	pMoriblinSword->Get_Model()->Play_Animation(fTimeDelta, m_bIsAnimationFinished, pMoriblinSword->Is_AnimationLoop(pMoriblinSword->Get_Model()->Get_CurrentAnimIndex()));
	pMoriblinSword->Sync_WithNavigationHeight();

	return nullptr;
}

CMoriblinSwordState * CMoveState::LateTick(CMoriblinSword * pMoriblinSword, _float fTimeDelta)
{
	if (m_bIsArrived)
		return new CIdleState();

	return nullptr;
}

void CMoveState::Enter(CMoriblinSword * pMoriblinSword)
{
	m_eStateId = STATE_ID::STATE_MOVE;

	Set_MoveTarget(pMoriblinSword);
	pMoriblinSword->Get_Model()->Set_CurrentAnimIndex(CMoriblinSword::ANIMID::ANIM_WALK);
}

void CMoveState::Exit(CMoriblinSword * pMoriblinSword)
{
	m_bIsArrived = false;
}

void CMoveState::Set_MoveTarget(CMoriblinSword * pMoriblinSword)
{
	_bool bIsPointValid = false;
	do
	{
		_float fOffsetX = ((_float)rand() / (float)(RAND_MAX)) * pMoriblinSword->Get_PatrolRadius();
		_int bSignX = rand() % 2;
		_float fOffsetZ = ((_float)rand() / (float)(RAND_MAX)) * pMoriblinSword->Get_PatrolRadius();
		_int bSignZ = rand() % 2;

		_float4 vInitialPosition = (_float4)pMoriblinSword->Get_Transform()->Get_TransformDesc().vInitialWorldMatrix.m[3];

		m_vPatrolPosition.x = bSignX ? (vInitialPosition.x + fOffsetX) : (vInitialPosition.x - fOffsetX);
		m_vPatrolPosition.y = pMoriblinSword->Get_Position().y;
		m_vPatrolPosition.z = bSignZ ? (vInitialPosition.z + fOffsetZ) : (vInitialPosition.z - fOffsetZ);

		bIsPointValid = pMoriblinSword->Get_Navigation()->Get_PointOnNavigation(m_vPatrolPosition);
	} while (!bIsPointValid);
}

void CMoveState::Patrol(CMoriblinSword * pMoriblinSword, _float fTimeDelta)
{
	_vector vPatrolPosition = XMVectorSet(m_vPatrolPosition.x, pMoriblinSword->Get_Position().y, m_vPatrolPosition.z, 1.f);
	pMoriblinSword->Get_Transform()->LookAt(vPatrolPosition);
	m_bIsArrived = pMoriblinSword->Get_Transform()->Go_TargetPosition(fTimeDelta, m_vPatrolPosition, 0.f, pMoriblinSword->Get_Navigation(), pMoriblinSword->Get_Radius());
}