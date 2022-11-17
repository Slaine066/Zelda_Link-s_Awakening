#pragma once

#include "Client_Defines.h"
#include "Actor.h"

BEGIN(Engine)
class CShader;
class CCollider;
class CRenderer;
class CTransform;
class CModel;
class CHierarchyNode;
END

BEGIN(Client)
class CWeapon final : public CActor
{
public:
	typedef struct tagWeaponDesc
	{
		CHierarchyNode*	pSocket = nullptr;
		const _float4x4* pParentWorldMatrix;
		_float4x4 SocketPivotMatrix;
		_tchar* pModelPrototypeId;
		_bool bIsPlayerWeapon;
		CCollider::COLLIDERDESC tColliderDesc;
	}WEAPONDESC;

private:
	CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon(const CWeapon& rhs);
	virtual ~CWeapon() = default;

public:
	_float4x4 Get_CombinedWorldMatrix() { return m_CombinedWorldMatrix; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT SetUp_ShaderResources() override;

	virtual void Update_Colliders() override;

private:
	WEAPONDESC m_WeaponDesc;
	_float4x4 m_CombinedWorldMatrix;

public:
	static CWeapon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};
END