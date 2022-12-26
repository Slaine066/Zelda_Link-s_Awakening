#pragma once

#include "Engine_Defines.h"
#include "Bossblin.h"
#include "GameInstance.h"
#include "Player.h"

BEGIN(Client)
class CBossblinState
{
public:
	enum STATETYPE
	{
		STATETYPE_START,
		STATETYPE_MAIN,
		STATETYPE_END,
		STATETYPE_DEFAULT
	};

	enum STATE_ID
	{
		STATE_IDLE,
		STATE_MOVE,
		STATE_BREAK,
		STATE_ATTACK_TACKLE,
		STATE_ATTACK_SPEAR,
		STATE_HIT,
		STATE_GUARD,
		STATE_DOWN,
		STATE_DIE,
		STATE_END,
	};

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

public:
	STATE_ID Get_StateId() { return m_eStateId; }
	STATETYPE Get_StateType() { return m_eStateType; }
	_bool Has_Aggro() { return m_pTarget; }

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
			_vector vPlayerPosition = pPlayer->Get_Transform()->Get_State(CTransform::STATE::STATE_TRANSLATION);
			_vector vPosition = pBossblin->Get_Transform()->Get_State(CTransform::STATE::STATE_TRANSLATION);

			_float fDistance = XMVectorGetX(XMVector3Length(vPlayerPosition - vPosition));
			if (fDistance < pBossblin->Get_AggroRadius())
			{
				m_pTarget = pPlayer;

				pBossblin->Get_Transform()->Change_Speed(pBossblin->Get_Stats().m_fRunSpeed);
			}
		}
	}

	_bool Is_InAttackRadius(CBossblin * pBossblin)
	{
		if (!m_pTarget)
			return false;

		_vector vPlayerPosition = m_pTarget->Get_Transform()->Get_State(CTransform::STATE::STATE_TRANSLATION);
		_vector vPosition = pBossblin->Get_Transform()->Get_State(CTransform::STATE::STATE_TRANSLATION);

		_float fDistance = XMVectorGetX(XMVector3Length(vPlayerPosition - vPosition));
		if (fDistance <= pBossblin->Get_AttackRadius())
			return true;
		else
			return false;
	}

protected:
	STATETYPE m_eStateType = STATETYPE_DEFAULT;
	STATE_ID m_eStateId = STATE_END;
	_bool m_bIsAnimationFinished = false;
	_bool m_bCanShake = true;
	class CPlayer* m_pTarget = nullptr;		/* If TRUE, has Aggro. */
};
END