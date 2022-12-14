#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)
class CCamera_Player final : public CCamera
{
public:
	enum CAM_MODE { MODE_PLAYER, MODE_ZOOMIN, MODE_ZOOMOUT, MODE_ENDING };

	typedef struct tagCameraDesc_Derived
	{
		CCamera::CAMERADESC	CameraDesc;
	}CAMERADESC_DERIVED;

private:
	CCamera_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Player(const CCamera_Player& rhs);
	virtual ~CCamera_Player() = default;

public:
	void Set_ModeZoom(_bool bZoom) { bZoom ? m_eCamMode = MODE_ZOOMIN : m_eCamMode = MODE_ZOOMOUT; }
	void Set_ZoomPosition(_float3 vZoomPosition) { m_vZoomPosition = vZoomPosition; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual _uint Tick(_float fTimeDelta)override;
	virtual _uint Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;

private:
	void Player_Camera(_float fTimeDelta);
	void ZoomIn_Camera(_float fTimeDelta, _float3 vZoomPosition);
	void ZoomOut_Camera(_float fTimeDelta);

private:
	CAM_MODE m_eCamMode = MODE_PLAYER;
	_float3 m_vZoomPosition;

public:
	static CCamera_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END