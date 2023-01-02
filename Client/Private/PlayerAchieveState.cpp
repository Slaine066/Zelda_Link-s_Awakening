#include "stdafx.h"

#include "PlayerAchieveState.h"
#include "GameInstance.h"
#include "PlayerIdleState.h"
#include "Item.h"

using namespace Player;

CAchieveState::CAchieveState(STATETYPE eStateType, ITEMID eItemId, CEffect* pGetItemEffect) : m_eItemId(eItemId), m_pGetItemEffect(pGetItemEffect)
{
	m_eStateType = eStateType;
}

CPlayerState * CAchieveState::HandleInput(CPlayer * pPlayer)
{
	switch (m_eStateType)
	{
		case STATETYPE::STATETYPE_MAIN:
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

			if (pGameInstance->Key_Down('A'))
			{
				m_pGetItemEffect->Set_StartTimer(true);
				m_pGetItemChat->Set_ShouldDestroy(true);
				m_pGetItemChat = nullptr;
				
				RELEASE_INSTANCE(CGameInstance);

				return new CAchieveState(STATETYPE_END);
			}

			RELEASE_INSTANCE(CGameInstance);

			break;
		}
	}

	return nullptr;
}

CPlayerState * CAchieveState::Tick(CPlayer * pPlayer, _float fTimeDelta)
{
	pPlayer->Get_Model()->Play_Animation(fTimeDelta * 1.25f, m_bIsAnimationFinished, pPlayer->Is_AnimationLoop(pPlayer->Get_Model()->Get_CurrentAnimIndex()));
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
			return new CAchieveState(STATETYPE_MAIN, m_eItemId, m_pGetItemEffect);
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
		pPlayer->Set_CanPickup(false);

		Create_Item(pPlayer);
		break;
	case STATETYPE_MAIN:
		pPlayer->Get_Model()->Set_CurrentAnimIndex(CPlayer::ANIMID::ANIM_ITEM_GET_LOOP);

		Create_ChatGetItem(pPlayer);
		break;
	case STATETYPE_END:
		pPlayer->Get_Model()->Set_CurrentAnimIndex(CPlayer::ANIMID::ANIM_ITEM_GET_END);
		break;
	}

	pPlayer->Get_Transform()->Set_RotationY(180.f);
}

void CAchieveState::Exit(CPlayer * pPlayer)
{
	pPlayer->Get_Transform()->Set_RotationY(0.f);

	if (m_eStateType == STATETYPE_MAIN)
		pPlayer->Set_CanPickup(true);
}

void CAchieveState::Create_Item(CPlayer * pPlayer)
{
	CItem::ITEMDESC tItemDesc;
	ZeroMemory(&tItemDesc, sizeof(CItem::ITEMDESC));
	tItemDesc.m_eItemType = CItem::ITEMTYPE::TYPE_TREASURE;
	tItemDesc.mWorldMatrix = pPlayer->Get_Transform()->Get_World4x4();
	tItemDesc.m_eItemId = m_eItemId;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	pGameInstance->Add_GameObject(TEXT("Item_Treasure"), TEXT("Prototype_GameObject_Item"), pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Item"), &tItemDesc);

	RELEASE_INSTANCE(CGameInstance);

	Spawn_GetItemEffect(pPlayer);
}

void CAchieveState::Spawn_GetItemEffect(CPlayer * pPlayer)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CEffect::EFFECTDESC tEffectDesc;
	ZeroMemory(&tEffectDesc, sizeof(CEffect::EFFECTDESC));
	tEffectDesc.m_eEffectType = CEffect::EFFECT_TYPE::EFFECT_GET_ITEM;
	tEffectDesc.m_fEffectLifespan = 2.5f;
	tEffectDesc.m_pOwner = pPlayer;

	_matrix mWorldMatrix = pPlayer->Get_Transform()->Get_WorldMatrix();
	_matrix mTranslationMatrix = XMMatrixTranslation(0.f, .75f, 0.f);
	mWorldMatrix *= mTranslationMatrix;

	XMStoreFloat4x4(&tEffectDesc.m_WorldMatrix, mWorldMatrix);

	/* Spawn Get Item Effect. */
	if (FAILED(pGameInstance->Add_GameObject_Out(TEXT("Get_Item_Effect"), TEXT("Prototype_GameObject_Effect"), pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Effect"), (CGameObject*&)m_pGetItemEffect, &tEffectDesc)))
		return;

	RELEASE_INSTANCE(CGameInstance);
}

void CAchieveState::Create_ChatGetItem(CPlayer * pPlayer)
{
	CUI::UIDESC tUIDesc;
	ZeroMemory(&tUIDesc, sizeof(CUI::UIDESC));
	tUIDesc.m_fSizeX = 1161;
	tUIDesc.m_fSizeY = 250;
	tUIDesc.m_fX = g_iWinSizeX / 2;
	tUIDesc.m_fY = g_iWinSizeY - 175;
	tUIDesc.m_ePass = VTXTEXPASS::VTXTEX_UI_BLEND;

	switch (m_eItemId)
	{
	case ITEMID::ITEM_SHIELD:
		wcscpy_s(tUIDesc.m_pTextureName, MAX_PATH, TEXT("Prototype_Component_Texture_Chat_Get_Item_Shield"));
		break;
	case ITEMID::ITEM_SWORD:
		wcscpy_s(tUIDesc.m_pTextureName, MAX_PATH, TEXT("Prototype_Component_Texture_Chat_Get_Item_Sword"));
		break;
	case ITEMID::ITEM_ROCFEATHER:
		wcscpy_s(tUIDesc.m_pTextureName, MAX_PATH, TEXT("Prototype_Component_Texture_Chat_Get_Item_RocsFeather"));
		break;
	case ITEMID::ITEM_OCARINA:
		wcscpy_s(tUIDesc.m_pTextureName, MAX_PATH, TEXT("Prototype_Component_Texture_Chat_Get_Item_Ocarina"));
		break;
	}

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	pGameInstance->Add_GameObject_Out(TEXT("UI_Chat_Marin"), TEXT("Prototype_GameObject_UI_Chat"), pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_UI"), (CGameObject*&)m_pGetItemChat, &tUIDesc);

	RELEASE_INSTANCE(CGameInstance);
}