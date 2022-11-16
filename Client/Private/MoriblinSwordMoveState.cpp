#include "stdafx.h"

#include "MoriblinSwordMoveState.h"
#include "MoriblinSwordIdleState.h"
#include "Navigation.h"
#include "Transform.h"

using namespace MoriblinSword;

CMoveState::CMoveState()
{
}

CMoriblinSwordState * CMoveState::AI_Behavior(CMoriblinSword * pMoriblinSword)
{
	return nullptr;
}

CMoriblinSwordState * CMoveState::Tick(CMoriblinSword * pMoriblinSword, _float fTimeDelta)
{
	Move(pMoriblinSword, fTimeDelta);

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

		m_vMoveTarget.x = bSignX ? (vInitialPosition.x + fOffsetX) : (vInitialPosition.x - fOffsetX);
		m_vMoveTarget.y = pMoriblinSword->Get_Position().y;
		m_vMoveTarget.z = bSignZ ? (vInitialPosition.z + fOffsetZ) : (vInitialPosition.z - fOffsetZ);

		bIsPointValid = pMoriblinSword->Get_Navigation()->Get_PointOnNavigation(m_vMoveTarget);
	} while (!bIsPointValid);
}

void CMoveState::Move(CMoriblinSword * pMoriblinSword, _float fTimeDelta)
{
	_vector vMoveTarget = XMVectorSet(m_vMoveTarget.x, pMoriblinSword->Get_Position().y, m_vMoveTarget.z, 1.f);
	pMoriblinSword->Get_Transform()->LookAt(vMoveTarget);
	m_bIsArrived = pMoriblinSword->Get_Transform()->Go_TargetPosition(fTimeDelta, m_vMoveTarget, 0.f, pMoriblinSword->Get_Navigation());
}