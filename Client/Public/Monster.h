#pragma once

#include "Client_Defines.h"
#include "Actor.h"

BEGIN(Client)
class CMonster abstract : public CActor
{
protected:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster(const CMonster& rhs);
	virtual ~CMonster() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	virtual void Execute_State(_float fTimeDelta) = 0;
	virtual void Reset_State() = 0;

	virtual _bool Is_AnimationLoop(_uint eAnimId) = 0;

protected:
	DIRID m_eCurrentDir = DIR_STRAIGHT;

	_bool m_bIsAnimationFinished = false;

public:
	virtual void Free() override;
};
END