#pragma once

#include "Client_Defines.h"
#include "Actor.h"

/* Class for Interactable Objects, that is:
- Object with Animations (but with no Movement);
- Object with specific Logic;
- Object which can be Interacted with.

A good example would be a:
- Treasure Chest;
- Door. */

BEGIN(Client)
class CInteractableObject abstract : public CActor
{
protected:
	CInteractableObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInteractableObject(const CInteractableObject& rhs);
	virtual ~CInteractableObject() = default;

public:
	INTERACTABLEID Get_InteractableType() { return m_eInteractableType; }
	_bool Get_DidInteract() { return m_bDidInteract; }
	void Set_DidInteract(_bool bdidInteract) { m_bDidInteract = bdidInteract; }

public:
	virtual _bool CanInteract() PURE;
	virtual void Interact() PURE;

protected:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual _uint Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT SetUp_ShaderResources() override;

protected:
	_uint Compute_LevelIndex(_tchar* pObjName);

protected:
	INTERACTABLEID m_eInteractableType = INTERACTABLE_END;
	_bool m_bDidInteract = false;

public:
	virtual void Free() override;
};
END