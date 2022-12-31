#pragma once

#include "Character.h"

BEGIN(Client)
class CNpc abstract : public CCharacter
{
protected:
	CNpc(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNpc(const CNpc& rhs);
	virtual ~CNpc() = default;

public:
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

public:
	virtual void Free() override;
};
END