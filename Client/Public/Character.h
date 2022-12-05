#pragma once

#include "Actor.h"

BEGIN(Client)
class CCharacter abstract : public CActor
{
protected:
	typedef struct tagCharacterStats
	{
		_float m_fMaxHp;
		_float m_fCurrentHp;
		_float m_fAttackPower;
		_float m_fWalkSpeed;
		_float m_fRunSpeed;
	}STATS;

protected:
	CCharacter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCharacter(const CCharacter& rhs);
	virtual ~CCharacter() = default;

public:
	STATS Get_Stats() { return m_tStats; }

	void DecrementHp(_float fHp) 
	{ 
		if (m_tStats.m_fCurrentHp - fHp >= 0.f) 
			m_tStats.m_fCurrentHp -= fHp;
	}

protected:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual _uint Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	virtual void HandleFall() {};

protected:
	STATS m_tStats;
};
END