#pragma once

#include "Client_Defines.h"
#include "Actor.h"

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
	M:	Open Inventory	(- on Switch) 
	N:	Open Map		(+ on Switch)
*/

BEGIN(Client)
class CPlayer final : public CActor
{
public:
	enum MESHID 
	{
		MESH_ARMS_LEGS,
		MESH_SWORD_B_HANDLE,
		MESH_SHIELD_B_MIRROR,
		MESH_EARS,
		MESH_HAIR,
		MESH_MAGIC_ROD,
		MESH_BELT,
		MESH_HOOKSHOT,
		MESH_SWORD_A,
		MESH_EYES,
		MESH_BODY,
		MESH_SWORD_A_HANDLE,
		MESH_OCARINA,
		MESH_SHIELD_A,
		MESH_MOUTH,
		MESH_SHOVEL,
		MESH_SWORD_B,
		MESH_SHIELD_B,
		MESH_BOOTS,
		MESH_FLIPPERS,
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
	_bool Is_AnimationLoop(_uint eAnimId);

private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT SetUp_ShaderResources() override;

private:
	void HandleInput();
	void TickState(_float fTimeDelta);
	void LateTickState(_float fTimeDelta);

private:
	class CPlayerState* m_pPlayerState = nullptr;

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};
END