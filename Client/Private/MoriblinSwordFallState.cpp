#include "stdafx.h"

#include "MoriblinSwordFallState.h"

using namespace MoriblinSword;

CFallState::CFallState()
{
}

CMoriblinSwordState * CFallState::AI_Behavior(CMoriblinSword * pMoriblinSword)
{
	return nullptr;
}

CMoriblinSwordState * CFallState::Tick(CMoriblinSword * pMoriblinSword, _float fTimeDelta)
{
	pMoriblinSword->Get_Model()->Play_Animation(fTimeDelta, m_bIsAnimationFinished, pMoriblinSword->Is_AnimationLoop(pMoriblinSword->Get_Model()->Get_CurrentAnimIndex()));
	Move(pMoriblinSword, fTimeDelta);

	return nullptr;
}

CMoriblinSwordState * CFallState::LateTick(CMoriblinSword * pMoriblinSword, _float fTimeDelta)
{
	if (m_bIsAnimationFinished)
		pMoriblinSword->Set_ShouldDestroy(true);

	return nullptr;
}

void CFallState::Enter(CMoriblinSword * pMoriblinSword)
{
	Compute_FallDirection(pMoriblinSword);

	m_eStateId = STATE_ID::STATE_FALL;
}

void CFallState::Exit(CMoriblinSword * pMoriblinSword)
{
}

void CFallState::Compute_FallDirection(CMoriblinSword * pMoriblinSword)
{
	_vector vDown = -pMoriblinSword->Get_Transform()->Get_State(CTransform::STATE::STATE_UP);
	vDown = XMVector4Normalize(vDown);

	XMStoreFloat3(&m_vFallDirection, vDown);
}

void CFallState::Move(CMoriblinSword * pMoriblinSword, _float fTimeDelta)
{
	if (!m_bIsFalling)
	{
		_vector vNegativeLook = -XMVector4Normalize(pMoriblinSword->Get_Transform()->Get_State(CTransform::STATE::STATE_LOOK));

		if (m_fShiftingTimer > .2f)
		{
			m_bIsFalling = true;
			pMoriblinSword->Get_Model()->Set_CurrentAnimIndex(CMoriblinSword::ANIMID::ANIM_FALL);
		}
		else
		{
			pMoriblinSword->Get_Transform()->Move_Direction(vNegativeLook, fTimeDelta, pMoriblinSword->Get_Navigation());
			m_fShiftingTimer += fTimeDelta;
		}
	}
	else
	{
		if (m_fFallingTimer > .3f)
			pMoriblinSword->Get_Transform()->Move_Direction(XMVectorSetW(XMLoadFloat3(&m_vFallDirection), 0.f), fTimeDelta * 2.5f, pMoriblinSword->Get_Navigation());
		else
			m_fFallingTimer += fTimeDelta;
	}
}
