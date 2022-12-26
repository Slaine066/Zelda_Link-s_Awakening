#include "stdafx.h"

#include "MoriblinSpearHitState.h"
#include "MoriblinSpearAttackState.h"
#include "Transform.h"

using namespace MoriblinSpear;

CHitState::CHitState(_float3 vDamageCauserPosition) : m_vDamageCauserPosition(vDamageCauserPosition)
{
}

CMoriblinSpearState * CHitState::AI_Behavior(CMoriblinSpear * pMoriblinSpear)
{
	Find_Target(pMoriblinSpear, true);

	return nullptr;
}

CMoriblinSpearState * CHitState::Tick(CMoriblinSpear * pMoriblinSpear, _float fTimeDelta)
{
	pMoriblinSpear->Get_Model()->Play_Animation(fTimeDelta * 1.25f, m_bIsAnimationFinished, pMoriblinSpear->Is_AnimationLoop(pMoriblinSpear->Get_Model()->Get_CurrentAnimIndex()));
	pMoriblinSpear->Sync_WithNavigationHeight();

	BounceBack(pMoriblinSpear, fTimeDelta);

	return nullptr;
}

CMoriblinSpearState * CHitState::LateTick(CMoriblinSpear * pMoriblinSpear, _float fTimeDelta)
{
	if (m_bIsAnimationFinished)
		return new CAttackState();

	return nullptr;
}

void CHitState::Enter(CMoriblinSpear * pMoriblinSpear)
{
	m_eStateId = STATE_ID::STATE_HIT;

	pMoriblinSpear->Set_ShaderPass(VTXANIMMODELPASS::VTXANIMMODEL_HIT);
	pMoriblinSpear->Reset_HitTimer();

	_bool bIsFront = Compute_HitPosition(pMoriblinSpear);
	pMoriblinSpear->Get_Model()->Set_CurrentAnimIndex(bIsFront ? CMoriblinSpear::ANIMID::ANIM_DAMAGE_FRONT : CMoriblinSpear::ANIMID::ANIM_DAMAGE_BACK);
}

void CHitState::Exit(CMoriblinSpear * pMoriblinSpear)
{
}

_bool CHitState::Compute_HitPosition(CMoriblinSpear * pMoriblinSpear)
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

void CHitState::BounceBack(CMoriblinSpear * pMoriblinSpear, _float fTimeDelta)
{
	_vector vTargetPosition = m_pTarget->Get_Transform()->Get_State(CTransform::STATE::STATE_TRANSLATION);
	_vector vPosition = pMoriblinSpear->Get_Transform()->Get_State(CTransform::STATE::STATE_TRANSLATION);

	_vector BounceDir = vPosition - vTargetPosition;
	BounceDir = XMVector4Normalize(BounceDir);

	pMoriblinSpear->Get_Transform()->Move_Direction(BounceDir, fTimeDelta, pMoriblinSpear->Get_Navigation(), pMoriblinSpear->Get_Radius());
}