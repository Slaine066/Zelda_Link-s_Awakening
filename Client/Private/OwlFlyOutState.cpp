#include "stdafx.h"

#include "OwlFlyOutState.h"
#include "UI.h"

using namespace Owl;

CFlyOutState::CFlyOutState(STATETYPE eStateType)
{
	m_eStateType = eStateType;
}

COwlState * CFlyOutState::AI_Behavior(COwl * pOwl)
{
	return nullptr;
}

COwlState * CFlyOutState::Tick(COwl * pOwl, _float fTimeDelta)
{
	pOwl->Get_Model()->Play_Animation(fTimeDelta * 1.5, m_bIsAnimationFinished, pOwl->Is_AnimationLoop(pOwl->Get_Model()->Get_CurrentAnimIndex()));

	return nullptr;
}

COwlState * CFlyOutState::LateTick(COwl * pOwl, _float fTimeDelta)
{
	if (m_bIsAnimationFinished)
	{
		switch (m_eStateType)
		{
		case STATETYPE::STATETYPE_START:
			return new CFlyOutState(STATETYPE::STATETYPE_END);
		case STATETYPE::STATETYPE_END:
			pOwl->Set_ShouldDestroy(true);
			break;
		}
	}

	return nullptr;
}

void CFlyOutState::Enter(COwl * pOwl)
{
	m_eStateId = STATE_ID::STATE_FLY_OUT;

	switch (m_eStateType)
	{
	case STATETYPE::STATETYPE_START:
	{
		pOwl->Get_Model()->Set_CurrentAnimIndex(COwl::ANIMID::ANIM_FLY_OUT_START);
		break;
	}
	case STATETYPE::STATETYPE_END:
		pOwl->Get_Model()->Set_CurrentAnimIndex(COwl::ANIMID::ANIM_FLY_OUT_END);
		break;
	}
}

void CFlyOutState::Exit(COwl * pOwl)
{
}