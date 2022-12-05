#include "stdafx.h"

#include "PlayerFallState.h"
#include "GameInstance.h"
#include "PlayerIdleState.h"

using namespace Player;

CFallState::CFallState()
{

}

CPlayerState * CFallState::HandleInput(CPlayer * pPlayer)
{
	return nullptr;
}

CPlayerState * CFallState::Tick(CPlayer * pPlayer, _float fTimeDelta)
{
	pPlayer->Get_Model()->Play_Animation(fTimeDelta, m_bIsAnimationFinished, pPlayer->Is_AnimationLoop(pPlayer->Get_Model()->Get_CurrentAnimIndex()));
	Move(pPlayer, fTimeDelta);

	return nullptr;
}

CPlayerState * CFallState::LateTick(CPlayer * pPlayer, _float fTimeDelta)
{
	if (pPlayer->Get_Model()->Is_Keyframe("head", 40))
	{
		pPlayer->Get_Model()->Reset_CurrentAnimation();

		_vector vRespawnPosition = XMVectorSetW(XMLoadFloat3(&m_vRespawnPosition), 1.f);
		if (pPlayer->Get_Navigation()->CanMove(vRespawnPosition))
			pPlayer->Get_Transform()->Set_State(CTransform::STATE::STATE_TRANSLATION, vRespawnPosition);

		return new CIdleState();
	}

	return nullptr;
}

void CFallState::Enter(CPlayer * pPlayer)
{
	Compute_RespawnPosition(pPlayer);

	m_eStateId = STATE_ID::STATE_FALL;
}

void CFallState::Exit(CPlayer * pPlayer)
{
	pPlayer->DecrementHp(.5f); /* .5f is the Fall Damage. */
}

void CFallState::Compute_RespawnPosition(CPlayer * pPlayer)
{
	_vector vLook = pPlayer->Get_Transform()->Get_State(CTransform::STATE::STATE_LOOK);
	vLook = XMVector4Normalize(vLook) / 2;

	_vector vCurrentPosition = pPlayer->Get_Transform()->Get_State(CTransform::STATE::STATE_TRANSLATION);
	XMStoreFloat3(&m_vRespawnPosition, vCurrentPosition -= vLook);

	_vector vDown = -pPlayer->Get_Transform()->Get_State(CTransform::STATE::STATE_UP);
	vDown = XMVector4Normalize(vDown);

	XMStoreFloat3(&m_vFallDirection, vDown);
}

void CFallState::Move(CPlayer * pPlayer, _float fTimeDelta)
{
	if (!m_bIsFalling)
	{
		_vector vLook = XMVector4Normalize(pPlayer->Get_Transform()->Get_State(CTransform::STATE::STATE_LOOK));

		if (m_fShiftingTimer > .2f)
		{
			m_bIsFalling = true;
			pPlayer->Get_Model()->Set_CurrentAnimIndex(CPlayer::ANIMID::ANIM_FALL_HOLE);
		}
		else
		{
			pPlayer->Get_Transform()->Move_Direction(vLook, fTimeDelta, pPlayer->Get_Navigation());
			m_fShiftingTimer += fTimeDelta;
		}
	}
	else
	{
		if (m_fFallingTimer > .3f)
			pPlayer->Get_Transform()->Move_Direction(XMVectorSetW(XMLoadFloat3(&m_vFallDirection), 0.f), fTimeDelta * 2.5f, pPlayer->Get_Navigation());
		else
			m_fFallingTimer += fTimeDelta;
	}
}