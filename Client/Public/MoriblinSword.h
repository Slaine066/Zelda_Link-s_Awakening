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
private:
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
		ANIM_FIND,
		ANIM_GUARD,
		ANIM_KYOROKYORO,
		ANIM_PIYO,
		ANIM_STAGGER,
		ANIM_STANCE_WAIT,
		ANIM_STANCE_WALK,
		ANIM_WAIT,
		ANIM_WALK,
		ANIM_END
	};

	enum STATEID
	{
		STATE_IDLE,
		STATE_MOVING,
		STATE_ATTACKING,
		STATE_END
	};

private:
	CMoriblinSword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMoriblinSword(const CMoriblinSword& rhs);
	virtual ~CMoriblinSword() = default;

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
	virtual void Execute_State(_float fTimeDelta) override;
	virtual void Reset_State() override;

	virtual _bool Is_AnimationLoop(_uint eAnimId) override;

private:
	STATEID m_eCurrentState = STATE_IDLE;

public:
	static CMoriblinSword* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};
END