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
	_bool Get_DidInteract() { return m_bDidInteract; }
	void Set_DidInteract(_bool bdidInteract) { m_bDidInteract = bdidInteract; }

	virtual _bool Is_AnimationLoop(_uint eAnimId) PURE;
	virtual _bool CanInteract() PURE;
	virtual void Interact() PURE;
	virtual void Compute_ChatLine() PURE;
	virtual void Process_ChatLine() PURE;
	
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

protected:
	_bool m_bDidInteract = false;

public:
	virtual void Free() override;
};
END