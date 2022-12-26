#pragma once

#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTransform;
class CModel;
END

BEGIN(Client)
class CBossblin final : public CMonster
{
public:
	enum MESHID
	{
		MESH_LEGS,
		MESH_SWORD,
		MESH_HANDS,
		MESH_BODY,
		MESH_SHIELD,
		MESH_END
	};

	enum ANIMID
	{
		ANIM_APPEAR,	
		ANIM_BRAKE,		
		ANIM_DAMAGE,	
		ANIM_DEAD,
		ANIM_DEAD_START,
		ANIM_DOWN,		
		ANIM_DOWN_END,			
		ANIM_DOWN_START,			
		ANIM_GUARD,
		ANIM_KYOROKYORO,			
		ANIM_MOVE_JUMP,		
		ANIM_SPEAR_THROW,	
		ANIM_TACKLE,	
		ANIM_TACKLE_END,			
		ANIM_TACKLE_START,			
		ANIM_WAIT,				
		ANIM_END
	};

	enum PARTS
	{
		PARTS_SPEAR,
		PARTS_END
	};

public:
	virtual _bool Is_AnimationLoop(_uint eAnimId) override;

private:
	CBossblin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBossblin(const CBossblin& rhs);
	virtual ~CBossblin() = default;

public:
	void Set_ShouldRenderWeapon(_bool bShouldRenderWeapon) { m_bShouldRenderWeapon = bShouldRenderWeapon; }
	_bool Get_IsProjectileAlive() { return m_bIsProjectileAlive; }
	void Set_IsProjectileAlive(_bool bIsAlive) { m_bIsProjectileAlive = bIsAlive; }
	CGameObject* Get_Part(CBossblin::PARTS ePart) { return m_vParts[ePart]; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual _uint Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual _float Take_Damage(float fDamage, void* DamageType, CGameObject* DamageCauser) override;

	void Spawn_GuardEffect(CGameObject* pDamageCauser);

private:
	HRESULT Ready_Parts();
	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT SetUp_ShaderResources() override;

private:
	virtual void AI_Behavior() override;
	virtual void TickState(_float fTimeDelta) override;
	virtual void LateTickState(_float fTimeDelta) override;

	void Check_Collision();

private:
	class CBossblinState* m_pBossblinState = nullptr;
	vector<CGameObject*> m_vParts;

private:
	_bool m_bShouldRenderWeapon = false;
	_bool m_bIsProjectileAlive = false;

public:
	static CBossblin* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};
END