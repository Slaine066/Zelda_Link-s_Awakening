#include "stdafx.h"

#include "PlayerJumpState.h"
#include "GameInstance.h"
#include "PlayerIdleState.h"

using namespace Player;

CJumpState::CJumpState(STATETYPE eStateType, _bool bMove) : m_bMove(bMove)
{
	m_eStateType = eStateType;
}

CPlayerState * CJumpState::HandleInput(CPlayer * pPlayer)
{
	return nullptr;
}

CPlayerState * CJumpState::Tick(CPlayer * pPlayer, _float fTimeDelta)
{
	pPlayer->Get_Model()->Play_Animation(fTimeDelta * 2, m_bIsAnimationFinished, pPlayer->Is_AnimationLoop(pPlayer->Get_Model()->Get_CurrentAnimIndex()));
	
	switch (m_eStateType)
	{
		case STATETYPE_MAIN:
		{
			/* Go Up. */
			if (pPlayer->Get_Model()->Between_Keyframe("waist", 0, 15))
				pPlayer->Get_Transform()->Move_Up(fTimeDelta * 2, pPlayer->Get_Navigation(), pPlayer->Get_Radius());
			/* Go Down. */
			else if (pPlayer->Get_Model()->Between_Keyframe("waist", 22, 37))
				pPlayer->Get_Transform()->Move_Down(fTimeDelta * 2, pPlayer->Get_Navigation(), pPlayer->Get_Radius());

			if (m_bMove)
				pPlayer->Get_Transform()->Move_Straight(fTimeDelta * 2, pPlayer->Get_Navigation(), pPlayer->Get_Radius());
		}
		break;
		case STATETYPE_END:
			if (m_eStateType == STATETYPE_END) /* CPlayer::ANIMID::ANIM_LAND */
				pPlayer->Sync_WithNavigationHeight();
		break;
	}

	return nullptr;
}

CPlayerState * CJumpState::LateTick(CPlayer * pPlayer, _float fTimeDelta)
{
	if (m_bIsAnimationFinished)
	{
		switch (m_eStateType)
		{
		case STATETYPE_MAIN:
			return new CJumpState(STATETYPE_END);
			break;
		case STATETYPE_END:
			return new CIdleState();
			break;
		}
	}

	/* Check if Colliding with COLLISION_BLOCK. */
	Check_CollisionBlock(pPlayer, fTimeDelta);

	return nullptr;
}

void CJumpState::Enter(CPlayer * pPlayer)
{
	m_eStateId = STATE_ID::STATE_JUMP;

	switch (m_eStateType)
	{
	case STATETYPE_MAIN:
		pPlayer->Get_Model()->Set_CurrentAnimIndex(CPlayer::ANIMID::ANIM_JUMP);
		break;
	case STATETYPE_END:
		pPlayer->Get_Model()->Set_CurrentAnimIndex(CPlayer::ANIMID::ANIM_LAND);
		break;
	}
}

void CJumpState::Exit(CPlayer * pPlayer)
{

}

_bool CJumpState::Check_CollisionBlock(CPlayer * pPlayer, _float fTimeDelta)
{
	/* Check if colliding with a COLLISION_BLOCK. */
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_bool bCollided = pGameInstance->Collision_with_Group(CCollision_Manager::COLLISION_OBJECT, pPlayer->Get_Collider(CCollider::AIM_DAMAGE_INPUT), CCollider::AIM_BLOCK, (CGameObject*&)m_pBlockingObject);

	if (bCollided)
	{
		/* Revert Move */
		pPlayer->Get_Transform()->Move_Backward(fTimeDelta, pPlayer->Get_Navigation(), pPlayer->Get_Radius());

		RELEASE_INSTANCE(CGameInstance);
		return true;
	}

	RELEASE_INSTANCE(CGameInstance);
	return false;
}
