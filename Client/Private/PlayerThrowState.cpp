#include "stdafx.h"

#include "PlayerThrowState.h"
#include "GameInstance.h"
#include "PlayerIdleState.h"

using namespace Player;

CThrowState::CThrowState()
{
}

CPlayerState * CThrowState::HandleInput(CPlayer * pPlayer)
{
	return nullptr;
}

CPlayerState * CThrowState::Tick(CPlayer * pPlayer, _float fTimeDelta)
{
	pPlayer->Get_Model()->Play_Animation(fTimeDelta, m_bIsAnimationFinished, pPlayer->Is_AnimationLoop(pPlayer->Get_Model()->Get_CurrentAnimIndex()));
	pPlayer->Sync_WithNavigationHeight();

	return nullptr;
}

CPlayerState * CThrowState::LateTick(CPlayer * pPlayer, _float fTimeDelta)
{
	if (m_bIsAnimationFinished)
		return new CIdleState();

	return nullptr;
}

void CThrowState::Enter(CPlayer * pPlayer)
{
	m_eStateId = STATE_ID::STATE_THROW;

	pPlayer->Get_Model()->Set_CurrentAnimIndex(CPlayer::ANIMID::ANIM_THROW);
}

void CThrowState::Exit(CPlayer * pPlayer)
{
}