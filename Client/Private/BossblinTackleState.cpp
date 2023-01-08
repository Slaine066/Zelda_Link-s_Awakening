#include "stdafx.h"

#include "BossblinTackleState.h"
#include "BossblinIdleState.h"
#include "BossblinBreakState.h"
#include "BossblinDownState.h"
#include "PlayerState.h"
#include "PlayerGuardState.h"
#include "Effect.h"

using namespace Bossblin;

CTackleState::CTackleState(STATETYPE eStateType)
{
	m_eStateType = eStateType;
}

CBossblinState * CTackleState::AI_Behavior(CBossblin * pBossblin)
{
	return nullptr;
}

CBossblinState * CTackleState::Tick(CBossblin * pBossblin, _float fTimeDelta)
{
	switch (m_eStateType)
	{
		case STATETYPE::STATETYPE_START:
		{
			Spawn_ShockwaveEffect(pBossblin);
		}
		break;
		case STATETYPE::STATETYPE_MAIN:
		{	
			Play_Footsteps(pBossblin);

			if (!Tackle(pBossblin, fTimeDelta)) /* Return FALSE when NavigationMesh::CanMove() function returns FALSE. */
			{
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				pGameInstance->PlaySounds(TEXT("Bossblin_Wall_Hit.wav"), SOUND_MONSTER_EFFECT, 1.f);
				RELEASE_INSTANCE(CGameInstance);

				return new CDownState(STATETYPE::STATETYPE_START);
			}
		}
		break;
	}

	pBossblin->Get_Model()->Play_Animation(fTimeDelta, m_bIsAnimationFinished, pBossblin->Is_AnimationLoop(pBossblin->Get_Model()->Get_CurrentAnimIndex()));
	pBossblin->Sync_WithNavigationHeight();

	return nullptr;
}

CBossblinState * CTackleState::LateTick(CBossblin * pBossblin, _float fTimeDelta)
{
	if (m_bIsAnimationFinished)
	{
		switch (m_eStateType)
		{
		case STATETYPE_START:
			return new CTackleState(STATETYPE_MAIN);
			break;
		case STATETYPE_END:
			return new CIdleState();
			break;
		}
	}

	if (m_eStateType == STATETYPE::STATETYPE_MAIN)
		if (Should_Break(pBossblin, fTimeDelta))
			return new CBreakState();

	return nullptr;
}

void CTackleState::Enter(CBossblin * pBossblin)
{
	m_eStateId = STATE_ID::STATE_ATTACK_TACKLE;

	switch (m_eStateType)
	{
		case STATETYPE::STATETYPE_START:
			pBossblin->Get_Model()->Set_CurrentAnimIndex(CBossblin::ANIMID::ANIM_TACKLE_START);
			break;
		case STATETYPE::STATETYPE_MAIN:
			Compute_TargetDirection(pBossblin);
			pBossblin->Get_Model()->Set_CurrentAnimIndex(CBossblin::ANIMID::ANIM_TACKLE);
			break;
		case STATETYPE::STATETYPE_END:
			pBossblin->Get_Model()->Set_CurrentAnimIndex(CBossblin::ANIMID::ANIM_TACKLE_END);
			break;
	}
}

void CTackleState::Exit(CBossblin * pBossblin)
{
	m_bDidDamage = false;
}

void CTackleState::Compute_TargetDirection(CBossblin * pBossblin)
{
	Find_Target(pBossblin, true);

	if (!m_pTarget)
		return;

	_vector vTargetPosition = XMVectorSet(m_pTarget->Get_Position().x, pBossblin->Get_Position().y, m_pTarget->Get_Position().z, 1.f);
	pBossblin->Get_Transform()->LookAt(vTargetPosition);

	XMStoreFloat3(&m_vTargetDirection, XMVector4Normalize(vTargetPosition - pBossblin->Get_Transform()->Get_State(CTransform::STATE::STATE_TRANSLATION)));
}

_bool CTackleState::Tackle(CBossblin * pBossblin, _float fTimeDelta)
{
	_vector vTargetDirection = XMLoadFloat3(&m_vTargetDirection);
	vTargetDirection = XMVectorSetW(vTargetDirection, 0.f);

	return pBossblin->Get_Transform()->Move_Direction(vTargetDirection, fTimeDelta, pBossblin->Get_Navigation(), pBossblin->Get_Radius());
}

_bool CTackleState::Should_Break(CBossblin * pBossblin, _float fTimeDelta)
{
	_vector vTargetPosition = m_pTarget->Get_Transform()->Get_State(CTransform::STATE::STATE_TRANSLATION);
	_vector vPosition = pBossblin->Get_Transform()->Get_State(CTransform::STATE::STATE_TRANSLATION);

	_vector vDistanceDirection = XMVector4Normalize(vTargetPosition - vPosition);
	_vector vRight = XMVector4Normalize(pBossblin->Get_Transform()->Get_State(CTransform::STATE::STATE_RIGHT));

	_vector vCross = XMVector3Cross(vRight, vDistanceDirection);
	_float3 vStoredCross;
	XMStoreFloat3(&vStoredCross, vCross);

	if (vStoredCross.y > 0.f)
	{
		if (m_fBreakTimer > 1.f)
			return true;
		else
			m_fBreakTimer += fTimeDelta;
	}

	return false;
}

void CTackleState::BounceBack(CBossblin * pBossblin, _float fTimeDelta)
{
	_vector vTargetPosition = m_pTarget->Get_Transform()->Get_State(CTransform::STATE::STATE_TRANSLATION);
	_vector vPosition = pBossblin->Get_Transform()->Get_State(CTransform::STATE::STATE_TRANSLATION);

	_vector BounceDir = vPosition - vTargetPosition;
	BounceDir = XMVector4Normalize(BounceDir);

	pBossblin->Get_Transform()->Move_Direction(BounceDir, fTimeDelta, pBossblin->Get_Navigation(), pBossblin->Get_Radius());
}

void CTackleState::Spawn_ShockwaveEffect(CBossblin * pBossblin)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CEffect::EFFECTDESC tEffectDesc;
	ZeroMemory(&tEffectDesc, sizeof(CEffect::EFFECTDESC));
	tEffectDesc.m_eEffectType = CEffect::EFFECT_TYPE::EFFECT_SHOCKWAVE;
	tEffectDesc.m_fEffectLifespan = .2f;
	tEffectDesc.m_pOwner = pBossblin;
	tEffectDesc.m_bIsPositionDynamic = true;

	CHierarchyNode* m_pSocketL = pBossblin->Get_Model()->Get_BonePtr("attach_L");
	CHierarchyNode* m_pSocketR = pBossblin->Get_Model()->Get_BonePtr("attach_R");
	_matrix SocketMatrixL = m_pSocketL->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&pBossblin->Get_Model()->Get_PivotFloat4x4()) * XMLoadFloat4x4(&pBossblin->Get_Transform()->Get_World4x4());
	_matrix SocketMatrixR = m_pSocketR->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&pBossblin->Get_Model()->Get_PivotFloat4x4()) * XMLoadFloat4x4(&pBossblin->Get_Transform()->Get_World4x4());

	if (pBossblin->Get_Model()->Is_Keyframe("spine_a", 20))
	{
		if (!m_bIsShockwaveSpawned)
		{
			XMStoreFloat4x4(&tEffectDesc.m_WorldMatrix, SocketMatrixL);

			/* Spawn Shockwave Effect (Model) on Bone Position. */
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Shockwave_Effect"), TEXT("Prototype_GameObject_Effect"), pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Effect"), &tEffectDesc)))
				return;

			tEffectDesc.m_eEffectType = CEffect::EFFECT_TYPE::EFFECT_SHOCKWAVE_RING;
			tEffectDesc.m_fEffectLifespan = .3f;

			/* Spawn ShockwaveRing Effect (Model) on Bone Position. */
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Shockwave_Effect"), TEXT("Prototype_GameObject_Effect"), pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Effect"), &tEffectDesc)))
				return;

			_tchar pSoundName[MAX_PATH] = TEXT("");
			_uint iRand = rand() % 2;
			wsprintf(pSoundName, TEXT("Drum_%d.wav"), iRand);
			pGameInstance->PlaySounds(pSoundName, SOUND_MONSTER_EFFECT, 1.f);

			m_bIsShockwaveSpawned = true;
		}
	}
	else if (pBossblin->Get_Model()->Is_Keyframe("spine_a", 29))
	{
		if (!m_bIsShockwaveSpawned)
		{
			XMStoreFloat4x4(&tEffectDesc.m_WorldMatrix, SocketMatrixR);

			/* Spawn Shockwave Effect (Model) on Bone Position. */
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Shockwave_Effect"), TEXT("Prototype_GameObject_Effect"), pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Effect"), &tEffectDesc)))
				return;

			tEffectDesc.m_eEffectType = CEffect::EFFECT_TYPE::EFFECT_SHOCKWAVE_RING;
			tEffectDesc.m_fEffectLifespan = .3f;

			/* Spawn ShockwaveRing Effect (Model) on Bone Position. */
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Shockwave_Effect"), TEXT("Prototype_GameObject_Effect"), pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Effect"), &tEffectDesc)))
				return;

			_tchar pSoundName[MAX_PATH] = TEXT("");
			_uint iRand = rand() % 2;
			wsprintf(pSoundName, TEXT("Drum_%d.wav"), iRand);
			pGameInstance->PlaySounds(pSoundName, SOUND_MONSTER_EFFECT, 1.f);

			m_bIsShockwaveSpawned = true;
		}
	}
	else if (pBossblin->Get_Model()->Is_Keyframe("spine_a", 38))
	{
		if (!m_bIsShockwaveSpawned)
		{
			XMStoreFloat4x4(&tEffectDesc.m_WorldMatrix, SocketMatrixL);

			/* Spawn Shockwave Effect (Model) on Bone Position. */
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Shockwave_Effect"), TEXT("Prototype_GameObject_Effect"), pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Effect"), &tEffectDesc)))
				return;

			tEffectDesc.m_eEffectType = CEffect::EFFECT_TYPE::EFFECT_SHOCKWAVE_RING;
			tEffectDesc.m_fEffectLifespan = .3f;

			/* Spawn ShockwaveRing Effect (Model) on Bone Position. */
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Shockwave_Effect"), TEXT("Prototype_GameObject_Effect"), pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Effect"), &tEffectDesc)))
				return;

			_tchar pSoundName[MAX_PATH] = TEXT("");
			_uint iRand = rand() % 2;
			wsprintf(pSoundName, TEXT("Drum_%d.wav"), iRand);
			pGameInstance->PlaySounds(pSoundName, SOUND_MONSTER_EFFECT, 1.f);

			m_bIsShockwaveSpawned = true;
		}
	}
	else if (pBossblin->Get_Model()->Is_Keyframe("spine_a", 47))
	{
		if (!m_bIsShockwaveSpawned)
		{
			XMStoreFloat4x4(&tEffectDesc.m_WorldMatrix, SocketMatrixR);

			/* Spawn Shockwave Effect (Model) on Bone Position. */
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Shockwave_Effect"), TEXT("Prototype_GameObject_Effect"), pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Effect"), &tEffectDesc)))
				return;

			tEffectDesc.m_eEffectType = CEffect::EFFECT_TYPE::EFFECT_SHOCKWAVE_RING;
			tEffectDesc.m_fEffectLifespan = .3f;

			/* Spawn ShockwaveRing Effect (Model) on Bone Position. */
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Shockwave_Effect"), TEXT("Prototype_GameObject_Effect"), pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Effect"), &tEffectDesc)))
				return;

			_tchar pSoundName[MAX_PATH] = TEXT("");
			_uint iRand = rand() % 2;
			wsprintf(pSoundName, TEXT("Drum_%d.wav"), iRand);
			pGameInstance->PlaySounds(pSoundName, SOUND_MONSTER_EFFECT, 1.f);

			m_bIsShockwaveSpawned = true;
		}
	}
	else if (pBossblin->Get_Model()->Is_Keyframe("spine_a", 57))
	{
		if (!m_bIsShockwaveSpawned)
		{
			XMStoreFloat4x4(&tEffectDesc.m_WorldMatrix, SocketMatrixL);

			/* Spawn Shockwave Effect (Model) on Bone Position. */
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Shockwave_Effect"), TEXT("Prototype_GameObject_Effect"), pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Effect"), &tEffectDesc)))
				return;

			tEffectDesc.m_eEffectType = CEffect::EFFECT_TYPE::EFFECT_SHOCKWAVE_RING;
			tEffectDesc.m_fEffectLifespan = .3f;

			/* Spawn ShockwaveRing Effect (Model) on Bone Position. */
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Shockwave_Effect"), TEXT("Prototype_GameObject_Effect"), pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Effect"), &tEffectDesc)))
				return;

			_tchar pSoundName[MAX_PATH] = TEXT("");
			_uint iRand = rand() % 2;
			wsprintf(pSoundName, TEXT("Drum_%d.wav"), iRand);
			pGameInstance->PlaySounds(pSoundName, SOUND_MONSTER_EFFECT, 1.f);

			m_bIsShockwaveSpawned = true;
		}
	}
	else
		m_bIsShockwaveSpawned = false;

	RELEASE_INSTANCE(CGameInstance);
}

void CTackleState::Play_Footsteps(CBossblin * pBossblin)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pBossblin->Get_Model()->Is_Keyframe("spine_a", 12))
	{
		if (!m_bIsSoundPlayed)
		{
			_tchar pSoundName[MAX_PATH] = TEXT("");
			_uint iRand = rand() % 4;
			wsprintf(pSoundName, TEXT("Bossblin_Step_%d.wav"), iRand);
			pGameInstance->PlaySounds(pSoundName, SOUND_MONSTER_EFFECT, 1.f);

			m_bIsSoundPlayed = true;
		}
	}
	else if (pBossblin->Get_Model()->Is_Keyframe("spine_a", 18))
	{
		if (!m_bIsSoundPlayed)
		{
			_tchar pSoundName[MAX_PATH] = TEXT("");
			_uint iRand = rand() % 4;
			wsprintf(pSoundName, TEXT("Bossblin_Step_%d.wav"), iRand);
			pGameInstance->PlaySounds(pSoundName, SOUND_MONSTER_EFFECT, 1.f);

			m_bIsSoundPlayed = true;
		}
	}
	else if (pBossblin->Get_Model()->Is_Keyframe("spine_a", 28))
	{
		if (!m_bIsSoundPlayed)
		{
			_tchar pSoundName[MAX_PATH] = TEXT("");
			_uint iRand = rand() % 4;
			wsprintf(pSoundName, TEXT("Bossblin_Step_%d.wav"), iRand);
			pGameInstance->PlaySounds(pSoundName, SOUND_MONSTER_EFFECT, 1.f);

			m_bIsSoundPlayed = true;
		}
	}
	else if (pBossblin->Get_Model()->Is_Keyframe("spine_a", 33))
	{
		if (!m_bIsSoundPlayed)
		{
			_tchar pSoundName[MAX_PATH] = TEXT("");
			_uint iRand = rand() % 4;
			wsprintf(pSoundName, TEXT("Bossblin_Step_%d.wav"), iRand);
			pGameInstance->PlaySounds(pSoundName, SOUND_MONSTER_EFFECT, 1.f);

			m_bIsSoundPlayed = true;
		}
	}
	else
		m_bIsSoundPlayed = false;

	RELEASE_INSTANCE(CGameInstance);
}