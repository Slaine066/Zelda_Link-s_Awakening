#pragma once

#include "Actor.h"

BEGIN(Client)
class CCharacter abstract : public CActor
{
protected:
	typedef struct tagCharacterStats
	{
		_uint m_fMaxHp;
		_uint m_fCurrentHp;
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

protected:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	STATS m_tStats;
};
END