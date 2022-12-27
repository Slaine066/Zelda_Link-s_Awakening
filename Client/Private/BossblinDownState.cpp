#include "stdafx.h"

#include "BossblinDownState.h"
#include "BossblinIdleState.h"
#include "Effect.h"

using namespace Bossblin;

CDownState::CDownState(STATETYPE eStateType, _float fDownTimer) : m_fDownTimer(fDownTimer)
{
	m_eStateType = eStateType;
}

CBossblinState * CDownState::AI_Behavior(CBossblin * pBossblin)
{
	return nullptr;
}

CBossblinState * CDownState::Tick(CBossblin * pBossblin, _float fTimeDelta)
{
	if (m_fDownTimer > 4.f)
		return new CDownState(STATETYPE::STATETYPE_END);
	else
		m_fDownTimer += fTimeDelta;

	pBossblin->Get_Model()->Play_Animation(fTimeDelta, m_bIsAnimationFinished, pBossblin->Is_AnimationLoop(pBossblin->Get_Model()->Get_CurrentAnimIndex()));
	pBossblin->Sync_WithNavigationHeight();

	return nullptr;
}

CBossblinState * CDownState::LateTick(CBossblin * pBossblin, _float fTimeDelta)
{
	if (m_bIsAnimationFinished)
	{
		switch (m_eStateType)
		{
		case STATETYPE_START:
			return new CDownState(STATETYPE_MAIN, m_fDownTimer);
			break;
		case STATETYPE_END:
			Find_Target(pBossblin, true);
			return new CIdleState(m_pTarget);
			break;
		}
	}

	return nullptr;
}

void CDownState::Enter(CBossblin * pBossblin)
{
	m_eStateId = STATE_ID::STATE_DOWN;

	switch (m_eStateType)
	{
	case STATETYPE_START:
		pBossblin->Get_Model()->Set_CurrentAnimIndex(CBossblin::ANIMID::ANIM_DOWN_START);
		pBossblin->Reset_StarsEffectSpawned();
		break;
	case STATETYPE_MAIN:
		pBossblin->Get_Model()->Set_CurrentAnimIndex(CBossblin::ANIMID::ANIM_DOWN);
		break;
	case STATETYPE_END:
		pBossblin->Get_Model()->Set_CurrentAnimIndex(CBossblin::ANIMID::ANIM_DOWN_END);
		break;
	}
}

void CDownState::Exit(CBossblin * pBossblin)
{
}

void CDownState::Spawn_StarEffect(CBossblin* pBossblin)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CEffect::EFFECTDESC tEffectDesc;
	ZeroMemory(&tEffectDesc, sizeof(CEffect::EFFECTDESC));
	tEffectDesc.m_eEffectType = CEffect::EFFECT_TYPE::EFFECT_STAR;
	tEffectDesc.m_fEffectLifespan = /*4.f*/20;
	tEffectDesc.m_pOwner = pBossblin;
	tEffectDesc.m_bIsPositionDynamic = true;

	CHierarchyNode* m_pSocket = pBossblin->Get_Model()->Get_BonePtr("jaw");
	_matrix SocketMatrix = m_pSocket->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&pBossblin->Get_Model()->Get_PivotFloat4x4()) * XMLoadFloat4x4(&pBossblin->Get_Transform()->Get_World4x4());
	
	_float fOffset = 0.3f;
	_matrix mOffset = XMMatrixTranslation(fOffset, .3f, 0.f);
	_matrix mWorldMatrix = SocketMatrix * mOffset;
	XMStoreFloat4x4(&tEffectDesc.m_WorldMatrix, mWorldMatrix);
	
	/* Spawn Star Effect on Bone. */
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Star_Effect"), TEXT("Prototype_GameObject_Effect"), pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Effect"), &tEffectDesc)))
		return;

	mWorldMatrix = SocketMatrix;
	mOffset = XMMatrixTranslation(-fOffset, .3f, 0.f);
	mWorldMatrix = SocketMatrix * mOffset;
	XMStoreFloat4x4(&tEffectDesc.m_WorldMatrix, mWorldMatrix);

	/* Spawn Star Effect on Bone. */
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Star_Effect"), TEXT("Prototype_GameObject_Effect"), pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Effect"), &tEffectDesc)))
		return;

	mWorldMatrix = SocketMatrix;
	mOffset = XMMatrixTranslation(0.f, .3f, fOffset);
	mWorldMatrix = SocketMatrix * mOffset;
	XMStoreFloat4x4(&tEffectDesc.m_WorldMatrix, mWorldMatrix);

	/* Spawn Star Effect on Bone. */
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Star_Effect"), TEXT("Prototype_GameObject_Effect"), pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Effect"), &tEffectDesc)))
		return;

	mWorldMatrix = SocketMatrix;
	mOffset = XMMatrixTranslation(0.f, .3f, -fOffset);
	mWorldMatrix = SocketMatrix * mOffset;
	XMStoreFloat4x4(&tEffectDesc.m_WorldMatrix, mWorldMatrix);

	/* Spawn Star Effect on Bone. */
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Star_Effect"), TEXT("Prototype_GameObject_Effect"), pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Effect"), &tEffectDesc)))
		return;

	RELEASE_INSTANCE(CGameInstance);
}