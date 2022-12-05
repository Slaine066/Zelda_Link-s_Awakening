#pragma once

#include "Client_Defines.h"
#include "InteractableObject.h"
#include "CollisionManager.h"

BEGIN(Client)
class CFallingTile final : public CInteractableObject
{
	CFallingTile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFallingTile(const CFallingTile& rhs);
	virtual ~CFallingTile() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual _uint Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	_bool CheckCollision(CCollision_Manager::COLLISION_GROUP eCollisionGroup, _float fTimeDelta);
	_bool ShouldDestroy();

private:
	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT SetUp_ShaderResources() override;

private:
	_float m_fDestroyTimer = 0.f;

public:
	static CFallingTile* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};
END