#include "stdafx.h"

#include "PlayerBareMoveState.h"
#include "GameInstance.h"
#include "PlayerBareState.h"
#include "PlayerPushState.h"
#include "Effect.h"

using namespace Player;

CBareMoveState::CBareMoveState(DIRID eDir, _bool bFirstSmokeSpawned, _bool bSecondSmokeSpawned)
	: m_eDirection(eDir),
	m_bFirstSmokeSpawned(bFirstSmokeSpawned),
	m_bSecondSmokeSpawned(bSecondSmokeSpawned)
{
}

CPlayerState * CBareMoveState::HandleInput(CPlayer * pPlayer)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	if (pGameInstance->Key_Pressing(VK_UP) && pGameInstance->Key_Pressing(VK_LEFT))
		return new CBareMoveState(DIR_STRAIGHT_LEFT, m_bFirstSmokeSpawned, m_bSecondSmokeSpawned);
	else if (pGameInstance->Key_Pressing(VK_UP) && pGameInstance->Key_Pressing(VK_RIGHT))
		return new CBareMoveState(DIR_STRAIGHT_RIGHT, m_bFirstSmokeSpawned, m_bSecondSmokeSpawned);
	else if (pGameInstance->Key_Pressing(VK_DOWN) && pGameInstance->Key_Pressing(VK_LEFT))
		return new CBareMoveState(DIR_BACKWARD_LEFT, m_bFirstSmokeSpawned, m_bSecondSmokeSpawned);
	else if (pGameInstance->Key_Pressing(VK_DOWN) && pGameInstance->Key_Pressing(VK_RIGHT))
		return new CBareMoveState(DIR_BACKWARD_RIGHT, m_bFirstSmokeSpawned, m_bSecondSmokeSpawned);
	else if (pGameInstance->Key_Pressing(VK_LEFT))
		return new CBareMoveState(DIR_LEFT, m_bFirstSmokeSpawned, m_bSecondSmokeSpawned);
	else if (pGameInstance->Key_Pressing(VK_RIGHT))
		return new CBareMoveState(DIR_RIGHT, m_bFirstSmokeSpawned, m_bSecondSmokeSpawned);
	else if (pGameInstance->Key_Pressing(VK_DOWN))
		return new CBareMoveState(DIR_BACKWARD, m_bFirstSmokeSpawned, m_bSecondSmokeSpawned);
	else if (pGameInstance->Key_Pressing(VK_UP))
		return new CBareMoveState(DIR_STRAIGHT, m_bFirstSmokeSpawned, m_bSecondSmokeSpawned);
	else
		return new CBareState();

	return nullptr;
}

CPlayerState * CBareMoveState::Tick(CPlayer * pPlayer, _float fTimeDelta)
{
	Move(pPlayer, fTimeDelta);

	pPlayer->Get_Model()->Play_Animation(fTimeDelta, m_bIsAnimationFinished, pPlayer->Is_AnimationLoop(pPlayer->Get_Model()->Get_CurrentAnimIndex()));
	pPlayer->Sync_WithNavigationHeight();

	if (pPlayer->Get_Model()->Is_Keyframe("spine_c", 10) && !m_bFirstSmokeSpawned)
	{
		m_bSecondSmokeSpawned = false;

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		CEffect::EFFECTDESC tEffectDesc;
		ZeroMemory(&tEffectDesc, sizeof(CEffect::EFFECTDESC));
		tEffectDesc.m_eEffectType = CEffect::EFFECT_TYPE::EFFECT_SMOKE;
		tEffectDesc.m_WorldMatrix = pPlayer->Get_Transform()->Get_World4x4();
		tEffectDesc.m_fEffectLifespan = 1.f;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Smoke_Effect"), TEXT("Prototype_GameObject_Effect"), pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Effect"), &tEffectDesc)))
			return nullptr;

		_tchar pSoundName[MAX_PATH] = TEXT("");
		_uint iRand = rand() % 3;
		wsprintf(pSoundName, TEXT("Footstep_%d.wav"), iRand);

		pGameInstance->PlaySounds(pSoundName, SOUND_PLAYER, .9f);

		RELEASE_INSTANCE(CGameInstance);

		m_bFirstSmokeSpawned = true;
	}
	else if (pPlayer->Get_Model()->Is_Keyframe("spine_c", 22) && !m_bSecondSmokeSpawned)
	{
		m_bFirstSmokeSpawned = false;

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		CEffect::EFFECTDESC tEffectDesc;
		ZeroMemory(&tEffectDesc, sizeof(CEffect::EFFECTDESC));
		tEffectDesc.m_eEffectType = CEffect::EFFECT_TYPE::EFFECT_SMOKE;
		tEffectDesc.m_WorldMatrix = pPlayer->Get_Transform()->Get_World4x4();
		tEffectDesc.m_fEffectLifespan = 1.f;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Smoke_Effect"), TEXT("Prototype_GameObject_Effect"), pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Effect"), &tEffectDesc)))
			return nullptr;

		_tchar pSoundName[MAX_PATH] = TEXT("");
		_uint iRand = rand() % 3;
		wsprintf(pSoundName, TEXT("Footstep_%d.wav"), iRand);

		pGameInstance->PlaySounds(pSoundName, SOUND_PLAYER, .9f);

		RELEASE_INSTANCE(CGameInstance);

		m_bSecondSmokeSpawned = true;
	}

	return nullptr;
}

CPlayerState * CBareMoveState::LateTick(CPlayer * pPlayer, _float fTimeDelta)
{
	/* Check if Colliding with COLLISION_BLOCK. */
	if (Check_CollisionBlock(pPlayer, fTimeDelta) == true)
	{
		pPlayer->Get_Model()->Reset_CurrentAnimation();
		return new CPushState(STATETYPE::STATETYPE_START, m_eDirection, m_pBlockingObject);
	}

	return nullptr;
}

void CBareMoveState::Enter(CPlayer * pPlayer)
{
	m_eStateId = STATE_ID::STATE_BARE_MOVE;

	pPlayer->Get_Model()->Set_CurrentAnimIndex(CPlayer::ANIMID::ANIM_RUN_BARE);
}

void CBareMoveState::Exit(CPlayer * pPlayer)
{

}

void CBareMoveState::Move(CPlayer * pPlayer, _float fTimeDelta)
{
	switch (m_eDirection)
	{
	case DIR_STRAIGHT_LEFT:
		pPlayer->Get_Transform()->Set_RotationY(305.f);
		break;
	case DIR_STRAIGHT_RIGHT:
		pPlayer->Get_Transform()->Set_RotationY(45.f);
		break;
	case DIR_BACKWARD_LEFT:
		pPlayer->Get_Transform()->Set_RotationY(225.f);
		break;
	case DIR_BACKWARD_RIGHT:
		pPlayer->Get_Transform()->Set_RotationY(135.f);
		break;
	case DIR_STRAIGHT:
		pPlayer->Get_Transform()->Set_RotationY(0.f);
		break;
	case DIR_BACKWARD:
		pPlayer->Get_Transform()->Set_RotationY(180.f);
		break;
	case DIR_LEFT:
		pPlayer->Get_Transform()->Set_RotationY(270.f);
		break;
	case DIR_RIGHT:
		pPlayer->Get_Transform()->Set_RotationY(90.f);
		break;
	}

	pPlayer->Get_Transform()->Move_Straight(fTimeDelta, pPlayer->Get_Navigation(), pPlayer->Get_Radius());
}

_bool CBareMoveState::Check_CollisionBlock(CPlayer * pPlayer, _float fTimeDelta)
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