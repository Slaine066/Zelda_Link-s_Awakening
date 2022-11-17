#pragma once

#include "Engine_Defines.h"
#include "MoriblinSpear.h"
#include "GameInstance.h"
#include "Player.h"

BEGIN(Client)
class CMoriblinSpearState
{
public:
	virtual ~CMoriblinSpearState() {};
	virtual CMoriblinSpearState* AI_Behavior(CMoriblinSpear* pMoriblinSpear) { return nullptr; };
	virtual CMoriblinSpearState* Tick(CMoriblinSpear* pMoriblinSpear, _float fTimeDelta) { return nullptr; };
	virtual CMoriblinSpearState* LateTick(CMoriblinSpear* pMoriblinSpear, _float fTimeDelta) { return nullptr; };

	virtual void Enter(CMoriblinSpear* pMoriblinSpear) {};
	virtual void Exit(CMoriblinSpear* pMoriblinSpear) {};

	CMoriblinSpearState* ChangeState(CMoriblinSpear* pMoriblinSpear, CMoriblinSpearState* pCurrentState, CMoriblinSpearState* pNewState)
	{
		if (pCurrentState)
		{
			pCurrentState->Exit(pMoriblinSpear);
			Safe_Delete(pCurrentState);
		}

		pNewState->Enter(pMoriblinSpear);
		return pNewState;
	}

protected:
	virtual void Find_Target(CMoriblinSpear * pMoriblinSpear, _bool bHasAggro = false)
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
			_float fDistance = XMVectorGetX(XMVector3Length(XMLoadFloat3(&pGameObject->Get_Position()) - XMLoadFloat3(&pMoriblinSpear->Get_Position())));
			if (fDistance < pMoriblinSpear->Get_AggroRadius())
			{
				m_pTarget = pPlayer;

				pMoriblinSpear->Get_Transform()->Change_Speed(pMoriblinSpear->Get_Stats().m_fRunSpeed);
			}
		}
	}

protected:
	_bool m_bIsAnimationFinished = false;
	class CPlayer* m_pTarget = nullptr;		/* If TRUE, has Aggro. */
};
END