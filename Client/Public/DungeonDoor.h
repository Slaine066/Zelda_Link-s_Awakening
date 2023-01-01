#pragma once

#include "Client_Defines.h"
#include "DynamicObject.h"

BEGIN(Client)
class CDungeonDoor final : public CDynamicObject
{
public:
	enum ANIMID
	{
		ANIM_CLOSE,
		ANIM_CLOSE_WIDE,
		ANIM_OPEN_IDLE,
		ANIM_OPEN_IDLE_WIDE,
		ANIM_OPEN,
		ANIM_OPEN_WIDE,
		ANIM_END
	};

private:
	CDungeonDoor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDungeonDoor(const CDungeonDoor& rhs);
	virtual ~CDungeonDoor() = default;

public:
	/* TODO: .. */

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

public:
	static CDungeonDoor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};
END