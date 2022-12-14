#include "stdafx.h"

#include "PlayerAchieveState.h"
#include "GameInstance.h"
#include "PlayerIdleState.h"

using namespace Player;

CAchieveState::CAchieveState(STATETYPE eStateType)
{
	m_eStateType = eStateType;
}

CPlayerState * CAchieveState::HandleInput(CPlayer * pPlayer)
{
	return nullptr;
}

CPlayerState * CAchieveState::Tick(CPlayer * pPlayer, _float fTimeDelta)
{
	pPlayer->Get_Model()->Play_Animation(fTimeDelta, m_bIsAnimationFinished, pPlayer->Is_AnimationLoop(pPlayer->Get_Model()->Get_CurrentAnimIndex()));
	pPlayer->Sync_WithNavigationHeight();

	return nullptr;
}

CPlayerState * CAchieveState::LateTick(CPlayer * pPlayer, _float fTimeDelta)
{
	if (m_bIsAnimationFinished)
	{
		switch (m_eStateType)
		{
		case STATETYPE_START:
			return new CAchieveState(STATETYPE_MAIN);
			break;
		case STATETYPE_MAIN:
			return new CAchieveState(STATETYPE_END);
			break;
		case STATETYPE_END:
			return new CIdleState();
			break;
		}
	}

	return nullptr;
}

void CAchieveState::Enter(CPlayer * pPlayer)
{
	m_eStateId = STATE_ID::STATE_ACHIEVE;

	switch (m_eStateType)
	{
	case STATETYPE_START:
		pPlayer->Get_Model()->Set_CurrentAnimIndex(CPlayer::ANIMID::ANIM_ITEM_GET_START);
		break;
	case STATETYPE_MAIN:
		pPlayer->Get_Model()->Set_CurrentAnimIndex(CPlayer::ANIMID::ANIM_ITEM_GET_LOOP);
		break;
	case STATETYPE_END:
		pPlayer->Get_Model()->Set_CurrentAnimIndex(CPlayer::ANIMID::ANIM_ITEM_GET_END);
		break;
	}
}

void CAchieveState::Exit(CPlayer * pPlayer)
{

}