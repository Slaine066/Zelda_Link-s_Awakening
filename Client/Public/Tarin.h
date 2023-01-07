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
class CTarin final : public CNpc
{
public:
	enum MESHID
	{
		MESH_BACKBACK,			// 1
		MESH_FACE,				// 2
		MESH_BANANA,			// 0 
		MESH_MOUTH,				// 5
		MESH_NOSE,				// 6
		MESH_BACKBACK_STRAPS,	// 7
		MESH_BODY, 				// 3	
		MESH_EYES,				// 4
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
	CTarin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTarin(const CTarin& rhs);
	virtual ~CTarin() = default;

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
	virtual HRESULT Render_ShadowDepth() override;

private:
	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT SetUp_ShaderResources() override;
	virtual HRESULT SetUp_ShadowShaderResources() override;
	
private:
	virtual void AI_Behavior() override;
	virtual void TickState(_float fTimeDelta) override;
	virtual void LateTickState(_float fTimeDelta) override;

private:
	class CTarinState* m_pTarinState = nullptr;

public:
	static CTarin* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};
END