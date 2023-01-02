#pragma once

#include "PlayerState.h"
#include "Effect.h"
#include "UI_Chat.h"

BEGIN(Client)
BEGIN(Player)
class CAchieveState : public CPlayerState
{
public:
	CAchieveState(STATETYPE eStateType, ITEMID eItemId = ITEMID::ITEM_END, CEffect* pGetItemEffect = nullptr);

	virtual CPlayerState* HandleInput(CPlayer* pPlayer) override;
	virtual CPlayerState* Tick(CPlayer* pPlayer, _float fTimeDelta) override;
	virtual CPlayerState* LateTick(CPlayer* pPlayer, _float fTimeDelta) override;

	virtual void Enter(CPlayer* pPlayer) override;
	virtual void Exit(CPlayer* pPlayer) override;

private:
	void Create_Item(CPlayer * pPlayer);
	void Spawn_GetItemEffect(CPlayer * pPlayer);
	void Create_ChatGetItem(CPlayer * pPlayer);

private:
	ITEMID m_eItemId;
	CEffect* m_pGetItemEffect = nullptr;
	CUI_Chat* m_pGetItemChat = nullptr;
};
END
END