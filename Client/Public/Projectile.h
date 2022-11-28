#pragma once

#include "Client_Defines.h"
#include "Actor.h"

BEGIN(Client)
class CProjectile final : public CActor
{
public:
	enum PROJECTILE_TYPE
	{
		PROJECTILE_MORIBLINSPEAR,
		PROJECTILE_BOSSBLINSPEAR,
		PROJECTILE_END
	};

	typedef struct tagProjectileDescription
	{
		CActor* pOwner;
		PROJECTILE_TYPE eProjectileType;
		_bool bIsPlayerProjectile;
		_float fProjectileSpeed;
		_tchar* pModelPrototypeId;
		_float4x4 mWorldMatrix;
		CCollider::COLLIDERDESC tColliderDesc;
		_float3 vTargetPosition;
	}PROJECTILEDESC;

private:
	CProjectile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CProjectile(const CProjectile& rhs);
	virtual ~CProjectile() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual _uint Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT SetUp_ShaderResources() override;

private:
	void MoriblinSpear_Tick(_float fTimeDelta);
	void BossblinSpear_Tick(_float fTimeDelta);
	void MoriblinSpear_Collision();
	void BossblinSpear_Collision();

private:
	PROJECTILEDESC m_tProjectileDesc;
	_float m_fAliveTimer = 0.f;

public:
	static CProjectile* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};
END