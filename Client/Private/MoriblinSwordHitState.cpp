#include "stdafx.h"

#include "MoriblinSwordHitState.h"
#include "MoriblinSwordIdleState.h"
#include "Transform.h"

using namespace MoriblinSword;

CHitState::CHitState(_float3 vDamageCauserPosition) : m_vDamageCauserPosition(vDamageCauserPosition)
{
}

CMoriblinSwordState * CHitState::AI_Behavior(CMoriblinSword * pMoriblinSword)
{
	return nullptr;
}

CMoriblinSwordState * CHitState::Tick(CMoriblinSword * pMoriblinSword, _float fTimeDelta)
{
	pMoriblinSword->Get_Model()->Play_Animation(fTimeDelta, m_bIsAnimationFinished, pMoriblinSword->Is_AnimationLoop(pMoriblinSword->Get_Model()->Get_CurrentAnimIndex()));
	pMoriblinSword->Sync_WithNavigationHeight();

	return nullptr;
}

CMoriblinSwordState * CHitState::LateTick(CMoriblinSword * pMoriblinSword, _float fTimeDelta)
{
	if (m_bIsAnimationFinished)
		return new CIdleState();

	return nullptr;
}

void CHitState::Enter(CMoriblinSword * pMoriblinSword)
{
	_bool bIsFront = Compute_HitPosition(pMoriblinSword);
	pMoriblinSword->Get_Model()->Set_CurrentAnimIndex(bIsFront ? CMoriblinSword::ANIMID::ANIM_DAMAGE_FRONT : CMoriblinSword::ANIMID::ANIM_DAMAGE_BACK);
}

void CHitState::Exit(CMoriblinSword * pMoriblinSword)
{
	m_bHasAggro = true;
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