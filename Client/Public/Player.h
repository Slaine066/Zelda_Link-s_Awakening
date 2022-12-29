#pragma once

#include "Character.h"

/* 
Player Inputs
	ก่:	Go Straight		(ก่ on Switch)
	ก้:	Go Backward		(ก้ on Switch)
	ก็:	Go Left			(ก็ on Switch)
	กๆ:	Go Right		(กๆ on Switch)
	A:	Interact		(A on Switch)
	S:	Attack			(B on Switch)
	Z:	Use Item 1		(X on Switch)
	X:	Use Item 2		(Y on Switch)
	Q:	?				(L on Switch)
	W:	Guard			(R on Switch)
	M:	Open Map		(- on Switch) 
	N:	Open Inventory	(+ on Switch)
*/

BEGIN(Client)
class CPlayer final : public CCharacter
{
public:
	enum MESHID
	{
		MESH_ARMS_LEGS,			// 3
		MESH_SWORD_B_HANDLE,	// 19
		MESH_SHIELD_B_MIRROR,	// 15
		MESH_EARS,				// 7
		MESH_HAIR,				// 11
		MESH_MAGIC_ROD,			// 0
		MESH_BELT,				// 4
		MESH_HOOKSHOT,			// 12
		MESH_SWORD_A,			// 16
		MESH_EYES,				// 8
		MESH_BODY,				// 5
		MESH_SWORD_A_HANDLE,	// 17
		MESH_OCARINA,			// 1
		MESH_SHIELD_A,			// 13
		MESH_MOUTH,				// 9
		MESH_SHOVEL,			// 2
		MESH_SWORD_B,			// 18
		MESH_SHIELD_B,			// 14
		MESH_BOOTS,				// 6
		MESH_FLIPPERS,			// 10
		MESH_END
	};

	enum ANIMID
	{
		ANIM_BOW_END,
		ANIM_BOW_START,
		ANIM_CARRY,
		ANIM_CARRY_FAIL,
		ANIM_DEATH,
		ANIM_DAMAGE_AIR,
		ANIM_DAMAGE_BACK,
		ANIM_DAMAGE_FRONT,
		ANIM_FALL_FROM_TOP,
		ANIM_FALL_HOLE,
		ANIM_IDLE,
		ANIM_IDLE_BARE,
		ANIM_IDLE_CARRY,
		ANIM_ITEM_CARRY,
		ANIM_ITEM_GET_END,
		ANIM_ITEM_GET_LOOP,
		ANIM_ITEM_GET_START,
		ANIM_ITEM_PUT,
		ANIM_JUMP,
		ANIM_JUMP_SLASH,
		ANIM_KEY_OPEN,
		ANIM_LADDER_DOWN,
		ANIM_LADDER_DOWN_START,
		ANIM_LADDER_UP,
		ANIM_LADDER_UP_END,
		ANIM_LADDER_UP_START,
		ANIM_LADDER_WAIT,
		ANIM_LAND,
		ANIM_MOVE_CARRY,
		ANIM_PULL,
		ANIM_PULL_START,
		ANIM_PULL_IDLE,
		ANIM_PUSH,
		ANIM_PUSH_START,
		ANIM_PUSH_IDLE,
		ANIM_REVODOOR_IN,
		ANIM_ROD_END,
		ANIM_ROD_START,
		ANIM_RUN,
		ANIM_RUN_BARE,
		ANIM_SHIELD_END,
		ANIM_SHIELD_HOLD_BACK,
		ANIM_SHIELD_HOLD_FRONT,
		ANIM_SHIELD_HOLD_LEFT,
		ANIM_SHIELD_HOLD_RIGHT,
		ANIM_SHIELD_LOOP,
		ANIM_SHIELD_START,
		ANIM_SLASH,
		ANIM_SLASH_HOLD_BACK,
		ANIM_SLASH_HOLD_END,
		ANIM_SLASH_HOLD_FRONT,
		ANIM_SLASH_HOLD_LEFT,
		ANIM_SLASH_HOLD_LOOP,
		ANIM_SLASH_HOLD_RIGHT,
		ANIM_SLASH_HOLD_START,
		ANIM_TALK,
		ANIM_THROW,
		ANIM_WALK,
		ANIM_WALK_BARE,
		ANIM_WARP_END,
		ANIM_WARP_START,
		ANIM_END
	};

public:
	class CPlayerState* Get_State();
	void Set_State(class CPlayerState* pPlayerState) { m_pPlayerState = pPlayerState; }
	_bool Is_AnimationLoop(_uint eAnimId);
	void Set_IsInvincible(_bool bIsInvincible) { m_bIsInvincible = bIsInvincible; }
	class CInteractableObject* Get_InteractableObject() { return m_pCurrentInteractableObject; }
	void Set_InteractableObject(class CInteractableObject* pInteractableObj) { m_pCurrentInteractableObject = pInteractableObj; }
	void Set_AchieveState();
	_bool Get_CanPickup() { return m_bCanPickup; }
	void Set_CanPickup(_bool bCanPickup) { m_bCanPickup = bCanPickup; }

private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual _uint Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual _float Take_Damage(float fDamage, void* DamageType, CGameObject* DamageCauser) override;

	class CPlayerState* Use_Item(_bool bIsX = true);
	void Spawn_GuardEffect();

private:
	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT SetUp_ShaderResources() override;

private:
	void HandleInput();
	void TickState(_float fTimeDelta);
	void LateTickState(_float fTimeDelta);
	void HandleInvincibility(_float fTimeDelta);
	virtual void HandleFall(_float fTimeDelta) override;

private:
	DIRID m_eDirection = DIRID::DIR_END;

	class CPlayerState* m_pPlayerState = nullptr;

	/* Invincibility Variables */
	_float m_fInvincibleTimer = 0.f;
	_bool m_bIsInvincible = false;

	class CInteractableObject* m_pCurrentInteractableObject = nullptr;
	_bool m_bCanPickup = true;

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};
END