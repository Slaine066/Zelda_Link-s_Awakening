#pragma once

#include "Engine_Defines.h"
#include "Bossblin.h"
#include "GameInstance.h"
#include "Player.h"

BEGIN(Client)
class CBossblinState
{
public:
	virtual ~CBossblinState() {};
	virtual CBossblinState* AI_Behavior(CBossblin* pBossblin) { return nullptr; };
	virtual CBossblinState* Tick(CBossblin* pBossblin, _float fTimeDelta) { return nullptr; };
	virtual CBossblinState* LateTick(CBossblin* pBossblin, _float fTimeDelta) { return nullptr; };

	virtual void Enter(CBossblin* pBossblin) {};
	virtual void Exit(CBossblin* pBossblin) {};

	CBossblinState* ChangeState(CBossblin* pBossblin, CBossblinState* pCurrentState, CBossblinState* pNewState)
	{
		if (pCurrentState)
		{
			pCurrentState->Exit(pBossblin);
			Safe_Delete(pCurrentState);
		}

		pNewState->Enter(pBossblin);
		return pNewState;
	}

protected:
	virtual void Find_Target(CBossblin * pBossblin, _bool bHasAggro = false)
	{
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		CGameObject* pGameObject = pGameInstance->Find_Object(pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Player"));
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameObject);
		if (!pPlayer)
			return;

		if (bHasAggro)
			m_pTarget = pPlayer;
		else
		{
			_float fDistance = XMVectorGetX(XMVector3Length(XMLoadFloat3(&pGameObject->Get_Position()) - XMLoadFloat3(&pBossblin->Get_Position())));
			if (fDistance < pBossblin->Get_AggroRadius())
			{
				m_pTarget = pPlayer;

				pBossblin->Get_Transform()->Change_Speed(pBossblin->Get_Stats().m_fRunSpeed);
			}
		}
	}

protected:
	_bool m_bIsAnimationFinished = false;
	class CPlayer* m_pTarget = nullptr;		/* If TRUE, has Aggro. */
};
END