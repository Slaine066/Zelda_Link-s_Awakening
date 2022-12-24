#pragma once

#include "Character.h"

BEGIN(Client)
class CMonster abstract : public CCharacter
{
protected:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster(const CMonster& rhs);
	virtual ~CMonster() = default;

public:
	_float Get_AggroRadius() { return m_fAggroRadius; }
	_float Get_PatrolRadius() { return m_fPatrolRadius; }
	_float Get_AttackRadius() { return m_fAttackRadius; }

	virtual _bool Is_AnimationLoop(_uint eAnimId) PURE;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual _uint Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	virtual void AI_Behavior() PURE;
	virtual void TickState(_float fTimeDelta) PURE;
	virtual void LateTickState(_float fTimeDelta) PURE;
	virtual void Compute_ShaderTimers(_float fTimeDelta);
	virtual void DropItems();

protected:
	CTexture* m_pDissolveTextureCom = nullptr;

	_float m_fAggroRadius = 0.f;
	_float m_fPatrolRadius = 0.f;
	_float m_fAttackRadius = 0.f;

public:
	virtual void Free() override;
};
END