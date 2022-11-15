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
	virtual _bool Is_AnimationLoop(_uint eAnimId) PURE;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	virtual void AI_Behavior() PURE;
	virtual void TickState(_float fTimeDelta) PURE;
	virtual void LateTickState(_float fTimeDelta) PURE;

public:
	virtual void Free() override;
};
END