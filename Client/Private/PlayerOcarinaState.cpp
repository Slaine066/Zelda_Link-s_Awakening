#include "stdafx.h"

#include "PlayerOcarinaState.h"
#include "GameInstance.h"
#include "PlayerIdleState.h"
#include "UI_Manager.h"

using namespace Player;

COcarinaState::COcarinaState(STATETYPE eStateType)
{
	m_eStateType = eStateType;
}

CPlayerState * COcarinaState::HandleInput(CPlayer * pPlayer)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	switch (m_eStateType)
	{
	case STATETYPE_MAIN:
		break;
	case STATETYPE_END:
		break;
	}

	return nullptr;
}

CPlayerState * COcarinaState::Tick(CPlayer * pPlayer, _float fTimeDelta)
{
	pPlayer->Get_Model()->Play_Animation(fTimeDelta, m_bIsAnimationFinished, pPlayer->Is_AnimationLoop(pPlayer->Get_Model()->Get_CurrentAnimIndex()));
	pPlayer->Sync_WithNavigationHeight();

	switch (m_eStateType)
	{
		case STATETYPE_MAIN:
		{
			if (m_fTimer > 10.f)
				return new COcarinaState(STATETYPE_END);
			else
				m_fTimer += fTimeDelta;

			break;
		}
	}

	return nullptr;
}

CPlayerState * COcarinaState::LateTick(CPlayer * pPlayer, _float fTimeDelta)
{
	if (m_bIsAnimationFinished)
	{
		switch (m_eStateType)
		{
		case STATETYPE_START:
			return new COcarinaState(STATETYPE_MAIN);
		case STATETYPE_END:
			CUI_Manager* pUIManager = GET_INSTANCE(CUI_Manager);

			pUIManager->ScreenFadeOut(CUI_ScreenFade::TYPE::TYPE_WHITE, 4.f);

			RELEASE_INSTANCE(CUI_Manager);

			return new CIdleState();
		}
	}

	return nullptr;
}

void COcarinaState::Enter(CPlayer * pPlayer)
{
	m_eStateId = STATE_ID::STATE_OCARINA;

	switch (m_eStateType)
	{
	case STATETYPE_START:
		pPlayer->Get_Model()->Set_CurrentAnimIndex(CPlayer::ANIMID::ANIM_OCARINA_START);
		break;
	case STATETYPE_MAIN:
		pPlayer->Get_Model()->Set_CurrentAnimIndex(CPlayer::ANIMID::ANIM_OCARINA_LOOP);
		PlaySound(pPlayer);
		break;
	case STATETYPE_END:
		pPlayer->Get_Model()->Set_CurrentAnimIndex(CPlayer::ANIMID::ANIM_OCARINA_END);
		break;
	}
}

void COcarinaState::Exit(CPlayer * pPlayer)
{

}

void COcarinaState::PlaySound(CPlayer * pPlayer)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	pGameInstance->StopAll();
	pGameInstance->PlaySounds(TEXT("Link_Ocarina.mp3"), SOUND_BGM, 0.5f);
	RELEASE_INSTANCE(CGameInstance);
}