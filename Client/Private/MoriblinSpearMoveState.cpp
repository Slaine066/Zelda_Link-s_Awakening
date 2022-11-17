#include "stdafx.h"

#include "MoriblinSpearMoveState.h"
#include "MoriblinSpearIdleState.h"
#include "MoriblinSpearAttackState.h"
#include "Navigation.h"
#include "Transform.h"

using namespace MoriblinSpear;

CMoveState::CMoveState(CPlayer* pTarget)
{
	m_pTarget = pTarget;
}

CMoriblinSpearState * CMoveState::AI_Behavior(CMoriblinSpear * pMoriblinSpear)
{
	return nullptr;
}

CMoriblinSpearState * CMoveState::Tick(CMoriblinSpear * pMoriblinSpear, _float fTimeDelta)
{
	if (m_pTarget)
	{
		if (Is_InAttackRadius(pMoriblinSpear))
			return new CAttackState();
		else
			Follow_Target(pMoriblinSpear, fTimeDelta);
	}
	else
		Patrol(pMoriblinSpear, fTimeDelta);

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
	if (m_pTarget)
		pMoriblinSpear->Get_Model()->Set_CurrentAnimIndex(CMoriblinSpear::ANIMID::ANIM_STANCE_WALK_FRONT);
	else
	{
		Set_MoveTarget(pMoriblinSpear);
		pMoriblinSpear->Get_Model()->Set_CurrentAnimIndex(CMoriblinSpear::ANIMID::ANIM_WALK);
	}
}

void CMoveState::Exit(CMoriblinSpear * pMoriblinSpear)
{
	m_bIsArrived = false;
	m_fIdleAttackTimer = 2.f;
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

		m_vPatrolPosition.x = bSignX ? (vInitialPosition.x + fOffsetX) : (vInitialPosition.x - fOffsetX);
		m_vPatrolPosition.y = pMoriblinSpear->Get_Position().y;
		m_vPatrolPosition.z = bSignZ ? (vInitialPosition.z + fOffsetZ) : (vInitialPosition.z - fOffsetZ);

		bIsPointValid = pMoriblinSpear->Get_Navigation()->Get_PointOnNavigation(m_vPatrolPosition);
	} while (!bIsPointValid);
}

void CMoveState::Patrol(CMoriblinSpear * pMoriblinSpear, _float fTimeDelta)
{
	_vector vPatrolPosition = XMVectorSet(m_vPatrolPosition.x, pMoriblinSpear->Get_Position().y, m_vPatrolPosition.z, 1.f);
	pMoriblinSpear->Get_Transform()->LookAt(vPatrolPosition);
	m_bIsArrived = pMoriblinSpear->Get_Transform()->Go_TargetPosition(fTimeDelta, m_vPatrolPosition, 0.f, pMoriblinSpear->Get_Navigation());
}

void CMoveState::Follow_Target(CMoriblinSpear * pMoriblinSpear, _float fTimeDelta)
{
	if (!m_pTarget)
		return;

	_vector vTargetPosition = XMVectorSet(m_pTarget->Get_Position().x, pMoriblinSpear->Get_Position().y, m_pTarget->Get_Position().z, 1.f);
	pMoriblinSpear->Get_Transform()->LookAt(vTargetPosition);

	_float3 vPosition;
	XMStoreFloat3(&vPosition, vTargetPosition);

	pMoriblinSpear->Get_Transform()->Go_Straight(fTimeDelta, pMoriblinSpear->Get_Navigation());
}