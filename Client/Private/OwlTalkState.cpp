#include "stdafx.h"

#include "OwlTalkState.h"
#include "OwlIdleState.h"
#include "OwlFlyOutState.h"

using namespace Owl;

CTalkState::CTalkState()
{
}

COwlState * CTalkState::AI_Behavior(COwl * pOwl)
{
	return nullptr;
}

COwlState * CTalkState::Tick(COwl * pOwl, _float fTimeDelta)
{
	pOwl->Get_Model()->Play_Animation(fTimeDelta, m_bIsAnimationFinished, pOwl->Is_AnimationLoop(pOwl->Get_Model()->Get_CurrentAnimIndex()));

	return nullptr;
}

COwlState * CTalkState::LateTick(COwl * pOwl, _float fTimeDelta)
{
	pOwl->CanInteract();

	if (pOwl->Get_DidInteract())
		return new CFlyOutState(STATETYPE::STATETYPE_START);

	if (!pOwl->Get_CurrentChat())
		return new CIdleState();

	return nullptr;
}

void CTalkState::Enter(COwl * pOwl)
{
	m_eStateId = STATE_ID::STATE_TALK;

	pOwl->Get_Model()->Set_CurrentAnimIndex(COwl::ANIMID::ANIM_TALK);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	pGameInstance->StopAll();
	pGameInstance->PlayBGM(TEXT("Owl_Theme.mp3"), 0.5f);
	RELEASE_INSTANCE(CGameInstance);
}

void CTalkState::Exit(COwl * pOwl)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	pGameInstance->StopAll();
	pGameInstance->PlayBGM(TEXT("Mysterious_Forest.mp3"), 0.5f);
	RELEASE_INSTANCE(CGameInstance);
}