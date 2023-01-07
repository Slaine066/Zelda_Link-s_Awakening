#pragma once

#include "Client_Defines.h"
#include "InteractableObject.h"

BEGIN(Client)
class CBed final : public CInteractableObject
{
public:
	enum ANIMID
	{
		ANIM_IDLE_SLEPT,
		ANIM_OUTBED,
		ANIM_SLEEP_LOOP,
		ANIM_WAKEUP_LOOP,
		ANIM_WAKEUP_START,
		ANIM_IDLE,
		ANIM_END
	};

private:
	CBed(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBed(const CBed& rhs);
	virtual ~CBed() = default;

public:
	virtual _bool CanInteract() override;
	virtual void Interact() override;

	void Set_Sleep();
	void Set_Wakeup();
	void Set_Getup();

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
	_bool Is_AnimationLoop(_uint eAnimId);
	void Reset_Animation();

private:
	_bool m_bIsAnimationFinished = false;

public:
	static CBed* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};
END