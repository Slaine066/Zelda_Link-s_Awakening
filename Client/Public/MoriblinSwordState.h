#pragma once

#include "Engine_Defines.h"
#include "MoriblinSword.h"
#include "GameInstance.h"
#include "Player.h"

BEGIN(Client)
class CMoriblinSwordState
{
public:
	enum STATE_ID
	{
		STATE_IDLE,
		STATE_MOVE,
		STATE_AGGRO,
		STATE_ATTACK,
		STATE_HIT,
		STATE_GUARD,
		STATE_STAGGER,
		STATE_FALL,
		STATE_DIE,
		STATE_END,
	};

public:
	virtual ~CMoriblinSwordState() {};
	virtual CMoriblinSwordState* AI_Behavior(CMoriblinSword* pMoriblinSword) { return nullptr; };
	virtual CMoriblinSwordState* Tick(CMoriblinSword* pMoriblinSword, _float fTimeDelta) { return nullptr; };
	virtual CMoriblinSwordState* LateTick(CMoriblinSword* pMoriblinSword, _float fTimeDelta) { return nullptr; };

	virtual void Enter(CMoriblinSword* pMoriblinSword) {};
	virtual void Exit(CMoriblinSword* pMoriblinSword) {};

	CMoriblinSwordState* ChangeState(CMoriblinSword* pMoriblinSword, CMoriblinSwordState* pCurrentState, CMoriblinSwordState* pNewState)
	{
		if (pCurrentState)
		{
			pCurrentState->Exit(pMoriblinSword);
			Safe_Delete(pCurrentState);
		}

		pCurrentState = pNewState;
		pCurrentState->Enter(pMoriblinSword);

		return pCurrentState;
	}

public:
	STATE_ID Get_StateId() { return m_eStateId; }
	_bool Has_Aggro() { return m_pTarget; }
	void Reset_Target() { m_pTarget = nullptr; }

protected:
	virtual void Find_Target(CMoriblinSword * pMoriblinSword, _bool bHasAggro = false)
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
			_float fDistance = XMVectorGetX(XMVector3Length(XMLoadFloat3(&pGameObject->Get_Position()) - XMLoadFloat3(&pMoriblinSword->Get_Position())));
			if (fDistance < pMoriblinSword->Get_AggroRadius())
			{
				m_pTarget = pPlayer;

				pMoriblinSword->Get_Transform()->Change_Speed(pMoriblinSword->Get_Stats().m_fRunSpeed);
			}
		}
	}

protected:
	STATE_ID m_eStateId = STATE_END;
	_bool m_bIsAnimationFinished = false;
	class CPlayer* m_pTarget = nullptr;		/* If TRUE, has Aggro. */
	_bool m_bHasSpottedTarget = false;
};
END