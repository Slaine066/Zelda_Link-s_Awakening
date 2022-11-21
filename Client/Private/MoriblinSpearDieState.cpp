#include "stdafx.h"

#include "MoriblinSpearDieState.h"

using namespace MoriblinSpear;

CDieState::CDieState(_float3 vDamageCauserPosition) : m_vDamageCauserPosition(vDamageCauserPosition)
{
}

CMoriblinSpearState * CDieState::AI_Behavior(CMoriblinSpear * pMoriblinSpear)
{
	Find_Target(pMoriblinSpear, true);

	return nullptr;
}

CMoriblinSpearState * CDieState::Tick(CMoriblinSpear * pMoriblinSpear, _float fTimeDelta)
{
	if (!m_bIsAnimationFinished)
	{
		pMoriblinSpear->Get_Model()->Play_Animation(fTimeDelta, m_bIsAnimationFinished, pMoriblinSpear->Is_AnimationLoop(pMoriblinSpear->Get_Model()->Get_CurrentAnimIndex()));
		
		BounceBack(pMoriblinSpear, fTimeDelta);
	}
	else
	{
		if (m_fDeadTimer > 2.f)
			pMoriblinSpear->Set_ShouldDestroy(true);
		else
			m_fDeadTimer += fTimeDelta;
	}

	pMoriblinSpear->Sync_WithNavigationHeight();

	return nullptr;
}

CMoriblinSpearState * CDieState::LateTick(CMoriblinSpear * pMoriblinSpear, _float fTimeDelta)
{
	return nullptr;
}

void CDieState::Enter(CMoriblinSpear * pMoriblinSpear)
{
	m_eStateId = STATE_ID::STATE_DIE;

	pMoriblinSpear->Set_Dead(true);

	_bool bIsFront = Compute_HitPosition(pMoriblinSpear);
	pMoriblinSpear->Get_Model()->Set_CurrentAnimIndex(bIsFront ? CMoriblinSpear::ANIMID::ANIM_DEAD_FRONT : CMoriblinSpear::ANIMID::ANIM_DEAD_BACK);
}

void CDieState::Exit(CMoriblinSpear * pMoriblinSpear)
{
}

_bool CDieState::Compute_HitPosition(CMoriblinSpear * pMoriblinSpear)
{
	_vector vLook = pMoriblinSpear->Get_Transform()->Get_State(CTransform::STATE::STATE_LOOK);
	_vector vDamageCauserDir = XMLoadFloat3(&pMoriblinSpear->Get_Position()) - XMLoadFloat3(&m_vDamageCauserPosition);

	vLook = XMVector3Normalize(vLook);
	vDamageCauserDir = XMVector3Normalize(vDamageCauserDir);

	_float fDot = XMVectorGetX(XMVector3Dot(vLook, vDamageCauserDir));
	_float fAngleRadian = acos(fDot);
	_float fAngleDegree = XMConvertToDegrees(fAngleRadian);

	if (fAngleDegree > 115.f && fAngleDegree <= 180.f)
		return true;

	return false;
}

void CDieState::BounceBack(CMoriblinSpear * pMoriblinSpear, _float fTimeDelta)
{
	_vector vTargetPosition = m_pTarget->Get_Transform()->Get_State(CTransform::STATE::STATE_TRANSLATION);
	_vector vPosition = pMoriblinSpear->Get_Transform()->Get_State(CTransform::STATE::STATE_TRANSLATION);
	
	_vector BounceDir = vPosition - vTargetPosition;
	BounceDir = XMVector4Normalize(BounceDir);

	pMoriblinSpear->Get_Transform()->Move_Direction(BounceDir, fTimeDelta, pMoriblinSpear->Get_Navigation());
}