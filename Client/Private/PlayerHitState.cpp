#include "stdafx.h"

#include "PlayerHitState.h"
#include "GameInstance.h"
#include "PlayerAttackState.h"
#include "PlayerMoveState.h"
#include "PlayerIdleState.h"

using namespace Player;

CHitState::CHitState(_float3 vDamageCauserPosition) : m_vDamageCauserPosition(vDamageCauserPosition)
{
}

CPlayerState * CHitState::HandleInput(CPlayer * pPlayer)
{
	return nullptr;
}

CPlayerState * CHitState::Tick(CPlayer * pPlayer, _float fTimeDelta)
{
	pPlayer->Get_Model()->Play_Animation(fTimeDelta, m_bIsAnimationFinished, pPlayer->Is_AnimationLoop(pPlayer->Get_Model()->Get_CurrentAnimIndex()));
	pPlayer->Sync_WithNavigationHeight();

	return nullptr;
}

CPlayerState * CHitState::LateTick(CPlayer * pPlayer, _float fTimeDelta)
{
	if (m_bIsAnimationFinished)
		return new CIdleState();

	return nullptr;
}

void CHitState::Enter(CPlayer * pPlayer)
{
	m_eStateId = STATE_ID::STATE_HIT;

	pPlayer->Set_ShaderPass(VTXANIMMODELPASS::VTXANIMMODEL_HIT);
	pPlayer->Reset_HitTimer();

	_bool bIsFront = Compute_HitPosition(pPlayer);
	pPlayer->Get_Model()->Set_CurrentAnimIndex(bIsFront ? CPlayer::ANIMID::ANIM_DAMAGE_FRONT : CPlayer::ANIMID::ANIM_DAMAGE_BACK);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_tchar pSoundName[MAX_PATH] = TEXT("");
	_uint iRand = rand() % 5;
	wsprintf(pSoundName, TEXT("Link_Damage_%d.wav"), iRand);

	pGameInstance->PlaySounds(pSoundName, SOUND_PLAYER, 1.f);

	RELEASE_INSTANCE(CGameInstance);
}

void CHitState::Exit(CPlayer * pPlayer)
{
}

_bool CHitState::Compute_HitPosition(CPlayer * pPlayer)
{
	_vector vLook = pPlayer->Get_Transform()->Get_State(CTransform::STATE::STATE_LOOK);
	_vector vDamageCauserDir = XMLoadFloat3(&pPlayer->Get_Position()) - XMLoadFloat3(&m_vDamageCauserPosition);

	vLook = XMVector3Normalize(vLook);
	vDamageCauserDir = XMVector3Normalize(vDamageCauserDir);

	_float fDot = XMVectorGetX(XMVector3Dot(vLook, vDamageCauserDir));
	_float fAngleRadian = acos(fDot);
	_float fAngleDegree = XMConvertToDegrees(fAngleRadian);

	if (fAngleDegree > 115.f && fAngleDegree <= 180.f)
		return true;

	return false;
}