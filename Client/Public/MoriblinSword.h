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
class CMoriblinSword final : public CMonster
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
		ANIM_DAMAGE_BACK,
		ANIM_DAMAGE_FRONT,
		ANIM_DEAD_BACK,
		ANIM_DEAD_FRONT,
		ANIM_DEAD_FIRE,
		ANIM_FALL,
		ANIM_FIND,			// When Aggroed
		ANIM_GUARD,			// When Hit
		ANIM_KYOROKYORO,
		ANIM_PIYO,			// When Stunned		(from Powder?)
		ANIM_STAGGER,		// When Break Guard
		ANIM_STANCE_WAIT,	// Idle				(while in Aggro)
		ANIM_STANCE_WALK,	// Move / Attack	(while in Aggro)
		ANIM_WAIT,			// Idle
		ANIM_WALK,			// Move
		ANIM_END
	};

public:
	virtual _bool Is_AnimationLoop(_uint eAnimId) override;

private:
	CMoriblinSword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMoriblinSword(const CMoriblinSword& rhs);
	virtual ~CMoriblinSword() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual _uint Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual _float Take_Damage(float fDamage, void* DamageType, CGameObject* DamageCauser) override;

private:
	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT SetUp_ShaderResources() override;

private:
	virtual void AI_Behavior() override;
	virtual void TickState(_float fTimeDelta) override;
	virtual void LateTickState(_float fTimeDelta) override;

private:
	class CMoriblinSwordState* m_pMoriblinSwordState = nullptr;

public:
	static CMoriblinSword* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};
END