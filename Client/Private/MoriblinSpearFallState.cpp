#include "stdafx.h"

#include "MoriblinSpearFallState.h"

using namespace MoriblinSpear;

CFallState::CFallState()
{
}

CMoriblinSpearState * CFallState::AI_Behavior(CMoriblinSpear * pMoriblinSpear)
{
	return nullptr;
}

CMoriblinSpearState * CFallState::Tick(CMoriblinSpear * pMoriblinSpear, _float fTimeDelta)
{
	pMoriblinSpear->Get_Model()->Play_Animation(fTimeDelta, m_bIsAnimationFinished, pMoriblinSpear->Is_AnimationLoop(pMoriblinSpear->Get_Model()->Get_CurrentAnimIndex()));
	Move(pMoriblinSpear, fTimeDelta);

	return nullptr;
}

CMoriblinSpearState * CFallState::LateTick(CMoriblinSpear * pMoriblinSpear, _float fTimeDelta)
{
	if (m_bIsAnimationFinished)
		pMoriblinSpear->Set_ShouldDestroy(true);

	return nullptr;
}

void CFallState::Enter(CMoriblinSpear * pMoriblinSpear)
{
	Compute_FallDirection(pMoriblinSpear);

	m_eStateId = STATE_ID::STATE_FALL;
}

void CFallState::Exit(CMoriblinSpear * pMoriblinSpear)
{
}

void CFallState::Compute_FallDirection(CMoriblinSpear * pMoriblinSpear)
{
	_vector vDown = -pMoriblinSpear->Get_Transform()->Get_State(CTransform::STATE::STATE_UP);
	vDown = XMVector4Normalize(vDown);

	XMStoreFloat3(&m_vFallDirection, vDown);
}

void CFallState::Move(CMoriblinSpear * pMoriblinSpear, _float fTimeDelta)
{
	if (!m_bIsFalling)
	{
		_vector vNegativeLook = -XMVector4Normalize(pMoriblinSpear->Get_Transform()->Get_State(CTransform::STATE::STATE_LOOK));

		if (m_fShiftingTimer > .2f)
		{
			m_bIsFalling = true;
			pMoriblinSpear->Get_Model()->Set_CurrentAnimIndex(CMoriblinSpear::ANIMID::ANIM_FALL);
		}
		else
		{
			pMoriblinSpear->Get_Transform()->Move_Direction(vNegativeLook, fTimeDelta, pMoriblinSpear->Get_Navigation());
			m_fShiftingTimer += fTimeDelta;
		}
	}
	else
	{
		if (m_fFallingTimer > .3f)
			pMoriblinSpear->Get_Transform()->Move_Direction(XMVectorSetW(XMLoadFloat3(&m_vFallDirection), 0.f), fTimeDelta * 2.5f, pMoriblinSpear->Get_Navigation());
		else
			m_fFallingTimer += fTimeDelta;
	}
}
