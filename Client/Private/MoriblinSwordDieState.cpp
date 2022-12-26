#include "stdafx.h"

#include "MoriblinSwordDieState.h"

using namespace MoriblinSword;

CDieState::CDieState(_float3 vDamageCauserPosition) : m_vDamageCauserPosition(vDamageCauserPosition)
{
}

CMoriblinSwordState * CDieState::AI_Behavior(CMoriblinSword * pMoriblinSword)
{
	Find_Target(pMoriblinSword, true);

	return nullptr;
}

CMoriblinSwordState * CDieState::Tick(CMoriblinSword * pMoriblinSword, _float fTimeDelta)
{
	if (!m_bIsAnimationFinished)
	{
		pMoriblinSword->Get_Model()->Play_Animation(fTimeDelta * 1.25f, m_bIsAnimationFinished, pMoriblinSword->Is_AnimationLoop(pMoriblinSword->Get_Model()->Get_CurrentAnimIndex()));

		BounceBack(pMoriblinSword, fTimeDelta);
	}
	else
	{
		if (m_fDeadTimer > pMoriblinSword->Get_DissolveLifespanTimer())
			pMoriblinSword->Set_ShouldDestroy(true);
		else
		{
			m_fDeadTimer += fTimeDelta;
			pMoriblinSword->Set_ShaderPass(VTXANIMMODELPASS::VTXANIMMODEL_DISSOLVE);
		}
	}

	pMoriblinSword->Sync_WithNavigationHeight();

	return nullptr;
}

CMoriblinSwordState * CDieState::LateTick(CMoriblinSword * pMoriblinSword, _float fTimeDelta)
{
	return nullptr;
}

void CDieState::Enter(CMoriblinSword * pMoriblinSword)
{
	m_eStateId = STATE_ID::STATE_DIE;

	pMoriblinSword->Set_Dead(true);
	pMoriblinSword->Set_ShaderPass(VTXANIMMODELPASS::VTXANIMMODEL_HIT);
	pMoriblinSword->Reset_HitTimer();

	_bool bIsFront = Compute_HitPosition(pMoriblinSword);
	pMoriblinSword->Get_Model()->Set_CurrentAnimIndex(bIsFront ? CMoriblinSword::ANIMID::ANIM_DEAD_FRONT : CMoriblinSword::ANIMID::ANIM_DEAD_BACK);
}

void CDieState::Exit(CMoriblinSword * pMoriblinSword)
{
}

_bool CDieState::Compute_HitPosition(CMoriblinSword * pMoriblinSword)
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

void CDieState::BounceBack(CMoriblinSword * pMoriblinSword, _float fTimeDelta)
{
	_vector vTargetPosition = m_pTarget->Get_Transform()->Get_State(CTransform::STATE::STATE_TRANSLATION);
	_vector vPosition = pMoriblinSword->Get_Transform()->Get_State(CTransform::STATE::STATE_TRANSLATION);

	_vector BounceDir = vPosition - vTargetPosition;
	BounceDir = XMVector4Normalize(BounceDir);

	pMoriblinSword->Get_Transform()->Move_Direction(BounceDir, fTimeDelta, pMoriblinSword->Get_Navigation(), pMoriblinSword->Get_Radius());
}