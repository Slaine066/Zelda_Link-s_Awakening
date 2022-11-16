#include "stdafx.h"

#include "MoriblinSpearMoveState.h"
#include "MoriblinSpearIdleState.h"
#include "Navigation.h"
#include "Transform.h"

using namespace MoriblinSpear;

CMoveState::CMoveState()
{
}

CMoriblinSpearState * CMoveState::AI_Behavior(CMoriblinSpear * pMoriblinSpear)
{
	return nullptr;
}

CMoriblinSpearState * CMoveState::Tick(CMoriblinSpear * pMoriblinSpear, _float fTimeDelta)
{
	Move(pMoriblinSpear, fTimeDelta);

	pMoriblinSpear->Get_Model()->Play_Animation(fTimeDelta, m_bIsAnimationFinished, pMoriblinSpear->Is_AnimationLoop(pMoriblinSpear->Get_Model()->Get_CurrentAnimIndex()));
	pMoriblinSpear->Sync_WithNavigationHeight();

	return nullptr;
}

CMoriblinSpearState * CMoveState::LateTick(CMoriblinSpear * pMoriblinSpear, _float fTimeDelta)
{
	if (m_bIsArrived)
		return new CIdleState();

	return nullptr;
}

void CMoveState::Enter(CMoriblinSpear * pMoriblinSpear)
{
	Set_MoveTarget(pMoriblinSpear);
	pMoriblinSpear->Get_Model()->Set_CurrentAnimIndex(CMoriblinSpear::ANIMID::ANIM_WALK);
}

void CMoveState::Exit(CMoriblinSpear * pMoriblinSpear)
{
	m_bIsArrived = false;
}

void CMoveState::Set_MoveTarget(CMoriblinSpear * pMoriblinSpear)
{
	_bool bIsPointValid = false;
	do
	{
		_float fOffsetX = ((_float)rand() / (float)(RAND_MAX)) * pMoriblinSpear->Get_PatrolRadius();
		_int bSignX = rand() % 2;
		_float fOffsetZ = ((_float)rand() / (float)(RAND_MAX)) * pMoriblinSpear->Get_PatrolRadius();
		_int bSignZ = rand() % 2;

		_float4 vInitialPosition = (_float4)pMoriblinSpear->Get_Transform()->Get_TransformDesc().vInitialWorldMatrix.m[3];

		m_vMoveTarget.x = bSignX ? (vInitialPosition.x + fOffsetX) : (vInitialPosition.x - fOffsetX);
		m_vMoveTarget.y = pMoriblinSpear->Get_Position().y;
		m_vMoveTarget.z = bSignZ ? (vInitialPosition.z + fOffsetZ) : (vInitialPosition.z - fOffsetZ);

		bIsPointValid = pMoriblinSpear->Get_Navigation()->Get_PointOnNavigation(m_vMoveTarget);
	} while (!bIsPointValid);
}

void CMoveState::Move(CMoriblinSpear * pMoriblinSpear, _float fTimeDelta)
{
	_vector vMoveTarget = XMVectorSet(m_vMoveTarget.x, pMoriblinSpear->Get_Position().y, m_vMoveTarget.z, 1.f);
	pMoriblinSpear->Get_Transform()->LookAt(vMoveTarget);
	m_bIsArrived = pMoriblinSpear->Get_Transform()->Go_TargetPosition(fTimeDelta, m_vMoveTarget, 0.f, pMoriblinSpear->Get_Navigation());
}