#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)
class CCamera_Dungeon final : public CCamera
{
public:
	enum CAM_MODE { MODE_DUNGEON, MODE_ZOOMIN, MODE_ZOOMOUT, MODE_SHAKING, MODE_ENDING };

	typedef struct tagCameraDesc_Derived
	{
		CCamera::CAMERADESC	CameraDesc;
	}CAMERADESC_DERIVED;

private:
	CCamera_Dungeon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Dungeon(const CCamera_Dungeon& rhs);
	virtual ~CCamera_Dungeon() = default;

public:
	void Add_RoomPosition(_float3 vPosition) { m_RoomPositions.push_back(vPosition); }

	_float3 Get_CurrentCameraPosition() { return m_vCurrentRoomPosition; }

	void Set_ModeZoom(_bool bZoom) { bZoom ? m_eCamMode = MODE_ZOOMIN : m_eCamMode = MODE_ZOOMOUT; }
	void Set_ZoomPosition(_float3 vZoomPosition) { m_vZoomPosition = vZoomPosition; }
	void Set_ModeShake(_float fPower, _float fVelocity, _float fVelocityDecrement)
	{
		m_eCamMode = MODE_SHAKING;
		m_fPower = fPower;
		m_fVelocity = fVelocity;
		m_fVelocityDecrement = fVelocityDecrement;
	};
	
	void Setup_DungeonCamera(_float3 vCurrentRoomPosition);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual _uint Tick(_float fTimeDelta)override;
	virtual _uint Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;


private:
	void Dungeon_Camera(_float fTimeDelta);
	void ZoomIn_Camera(_float fTimeDelta, _float3 vZoomPosition);
	void ZoomOut_Camera(_float fTimeDelta);
	void Shaking_Camera(_float fTimeDelta);

private:
	CAM_MODE m_eCamMode = MODE_DUNGEON;

	vector<_float3> m_RoomPositions;
	_float3 m_vCurrentRoomPosition;
	_bool m_bMoving = false;

	/* Zoom Variables */
	_float3 m_vZoomPosition;

	/* Shake Variables */
	_float m_fPower = .5f;
	_float m_fVelocity = 0.1f;
	_float m_fVelocityDecrement = 0.01f;
	_uint m_iShakeCount = 0;

public:
	static CCamera_Dungeon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END