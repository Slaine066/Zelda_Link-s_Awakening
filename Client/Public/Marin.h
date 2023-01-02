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
class CMarin final : public CNpc
{
public:
	enum MESHID
	{
		MESH_BODY,
		MESH_HAIR_FLOWER,
		MESH_HAIR,
		MESH_EYES,
		MESH_HEAD,
		MESH_MOUTH,
		MESH_LEGS,
		MESH_PENDANT,
		MESH_END
	};

	enum ANIMID
	{
		ANIM_TALK,
		ANIM_WAIT,
		ANIM_WALK,
		ANIM_END
	};

public:
	virtual _bool Is_AnimationLoop(_uint eAnimId) override;

private:
	CMarin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMarin(const CMarin& rhs);
	virtual ~CMarin() = default;

public:
	virtual _bool CanInteract() override;
	virtual void Interact() override;
	virtual void Compute_ChatLine() override;

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
	class CMarinState* m_pMarinState = nullptr;

	class CUI* m_pInteractButton = nullptr;

	_uint m_iChatLineIndex = 1;
	class CUI_Chat* m_pCurrentChat = nullptr;

public:
	static CMarin* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};
END