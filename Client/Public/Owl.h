#pragma once

#include "Client_Defines.h"
#include "Npc.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTransform;
class CModel;
END

BEGIN(Client)
class COwl final : public CNpc
{
public:
	enum MESHID
	{
		MESH_BEAK,		// 0
		MESH_BODY,		// 1
		MESH_EYE,		// 2 
		MESH_EYELID_0,	// 3
		MESH_EYELID_1,	// 4
		MESH_END
	};

	enum ANIMID
	{
		ANIM_FLY_ONPLACE,
		ANIM_FLY_ONPLACE_START,
		ANIM_FLY_ONPLACE_START_LOOP,
		ANIM_FLY_IN_END,
		ANIM_FLY_IN_START,
		ANIM_FLY_IN_START_LOOP,
		ANIM_FLY_OUT_END,
		ANIM_FLY_OUT_START,
		ANIM_FLY_TALK,
		ANIM_HIDDEN,
		ANIM_SERIOUS_TALK,
		ANIM_TALK,
		ANIM_IDLE,
		ANIM_END
	};

public:
	virtual _bool Is_AnimationLoop(_uint eAnimId) override;

private:
	COwl(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	COwl(const COwl& rhs);
	virtual ~COwl() = default;

public:
	virtual _bool CanInteract() override;
	virtual void Interact() override;
	virtual void Compute_ChatLine() override;
	virtual void Process_ChatLine() override;

	class CUI* Get_InteractButton() { return m_pInteractButton; }
	void Set_InteractButton(class CUI* pInteractButton) { m_pInteractButton = pInteractButton; }
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

private:
	virtual void AI_Behavior() override;
	virtual void TickState(_float fTimeDelta) override;
	virtual void LateTickState(_float fTimeDelta) override;

private:
	class COwlState* m_pOwlState = nullptr;

public:
	static COwl* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};
END