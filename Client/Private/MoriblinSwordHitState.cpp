#include "stdafx.h"

#include "MoriblinSwordHitState.h"
#include "MoriblinSwordAttackState.h"
#include "Transform.h"

using namespace MoriblinSword;

CHitState::CHitState(_float3 vDamageCauserPosition) : m_vDamageCauserPosition(vDamageCauserPosition)
{
}

CMoriblinSwordState * CHitState::AI_Behavior(CMoriblinSword * pMoriblinSword)
{
	Find_Target(pMoriblinSword, true);

	return nullptr;
}

CMoriblinSwordState * CHitState::Tick(CMoriblinSword * pMoriblinSword, _float fTimeDelta)
{
	pMoriblinSword->Get_Model()->Play_Animation(fTimeDelta, m_bIsAnimationFinished, pMoriblinSword->Is_AnimationLoop(pMoriblinSword->Get_Model()->Get_CurrentAnimIndex()));
	pMoriblinSword->Sync_WithNavigationHeight();

	BounceBack(pMoriblinSword, fTimeDelta);

	return nullptr;
}

CMoriblinSwordState * CHitState::LateTick(CMoriblinSword * pMoriblinSword, _float fTimeDelta)
{
	if (m_bIsAnimationFinished)
		return new CAttackState();

	return nullptr;
}

void CHitState::Enter(CMoriblinSword * pMoriblinSword)
{
	m_eStateId = STATE_ID::STATE_HIT;

	pMoriblinSword->Set_ShaderPass(VTXANIMMODELPASS::VTXANIMMODEL_HIT);

	_bool bIsFront = Compute_HitPosition(pMoriblinSword);
	pMoriblinSword->Get_Model()->Set_CurrentAnimIndex(bIsFront ? CMoriblinSword::ANIMID::ANIM_DAMAGE_FRONT : CMoriblinSword::ANIMID::ANIM_DAMAGE_BACK);
}

void CHitState::Exit(CMoriblinSword * pMoriblinSword)
{
}

_bool CHitState::Compute_HitPosition(CMoriblinSword * pMoriblinSword)
{
	_vector vLook = pMoriblinSword->Get_Transform()->Get_State(CTransform::STATE::STATE_LOOK);
	_vector vDamageCauserDir = XMLoadFloat3(&pMoriblinSword->Get_Position()) - XMLoadFloat3(&m_vDamageCauserPosition);

	vLook = XMVector3Normalize(vLook);
	vDamageCauserDir = XMVector3Normalize(vDamageCauserDir);
	
	_float fDot = XMVectorGetX(XMVector3Dot(vLook, vDamageCauserDir));
	_float fAngleRadian = acos(fDot);
	_float fAngleDegree = XMConvertToDegrees(fAngleRadian);

	if (fAngleDegree > 115.f && fAngleDegree <= 180.f)
		return true;

	return false;
}

void CHitState::BounceBack(CMoriblinSword * pMoriblinSword, _float fTimeDelta)
{
	_vector vTargetPosition = m_pTarget->Get_Transform()->Get_State(CTransform::STATE::STATE_TRANSLATION);
	_vector vPosition = pMoriblinSword->Get_Transform()->Get_State(CTransform::STATE::STATE_TRANSLATION);

	_vector BounceDir = vPosition - vTargetPosition;
	BounceDir = XMVector4Normalize(BounceDir);

	pMoriblinSword->Get_Transform()->Move_Direction(BounceDir, fTimeDelta, pMoriblinSword->Get_Navigation(), pMoriblinSword->Get_Radius());
}