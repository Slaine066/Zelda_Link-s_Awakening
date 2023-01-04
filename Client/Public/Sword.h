#pragma once

#include "Client_Defines.h"
#include "InteractableObject.h"

BEGIN(Client)
class CSword final : public CInteractableObject
{
private:
	CSword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSword(const CSword& rhs);
	virtual ~CSword() = default;

public:
	virtual _bool CanInteract() override;
	virtual void Interact() override;

	void Spawn_InteractButton();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual _uint Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT SetUp_ShaderResources() override;

	class CUI* m_pInteractButton = nullptr;

public:
	static CSword* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};
END