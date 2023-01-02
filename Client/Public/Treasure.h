#pragma once

#include "Client_Defines.h"
#include "InteractableObject.h"

BEGIN(Client)
class CTreasure final : public CInteractableObject
{
public:
	enum ANIMID
	{
		ANIM_APPEAR,
		ANIM_CLOSE,
		ANIM_CLOSE_IDLE,
		ANIM_OPEN_FAIL,
		ANIM_OPEN,
		ANIM_OPEN_IDLE,
		ANIM_END
	};

private:
	CTreasure(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTreasure(const CTreasure& rhs);
	virtual ~CTreasure() = default;

public:
	virtual _bool CanInteract() override;
	virtual void Interact() override;

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
	_bool Is_AnimationLoop(_uint eAnimId);
	void Reset_Animation();

private:
	_bool m_bIsAnimationFinished = false;
	_bool m_bItemDropped = false;

	class CUI* m_pInteractButton = nullptr;

public:
	static CTreasure* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};
END