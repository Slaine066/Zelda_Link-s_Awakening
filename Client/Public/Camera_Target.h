//#pragma once
//
//#include "Camera.h"
//
//BEGIN(Client)
//class CCamera_Target final : public CCamera
//{
//public:
//	enum CAM_MODE { MODE_ZOOM, MODE_FOLLOW, MODE_ZOOMOUT, MODE_GO, MODE_POSITION, MODE_RETURN, MODE_IDLE };
//
//	typedef struct tagCameraDesc_Derived
//	{
//		_float3						vDistance = _float3(0, 6, -10);
//		CCamera::CAMERADESC			CameraDesc;
//	}CAMERADESC_DERIVED;
//
//private:
//	CCamera_Target(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
//	CCamera_Target(const CCamera_Target& rhs);
//	virtual ~CCamera_Target() = default;
//
//public:
//	virtual HRESULT Initialize_Prototype();
//	virtual HRESULT Initialize(void* pArg);
//	virtual _uint Tick(_float fTimeDelta);
//	virtual _uint Late_Tick(_float fTimeDelta);
//
//public:
//	void Set_TalkingMode(_bool type) { if (type)m_eCamMode = MODE_ZOOM; else m_eCamMode = MODE_ZOOMOUT; }
//	void Set_GoingMode(_bool type) { if (type)m_eCamMode = MODE_GO; else m_eCamMode = MODE_RETURN; }
//	void Set_PositionMode(_bool type) { if (type)m_eCamMode = MODE_POSITION; else m_eCamMode = MODE_RETURN; }
//	void Set_Target(class CGameObject* pGameObject) { m_pTarget = pGameObject; }
//	void Set_Position(_float3 pPosition) { m_vPosition = pPosition; }
//	void Set_OffSetDistance(_float3 pPosition) { m_vOffSetDistance = pPosition; }
//
//private:
//	void Idle_Camera(_float fTimeDelta);
//	void Target_Camera(_float fTimeDelta, CGameObject* pGameObject);
//	void OutTarget_Camera(_float fTimeDelta);
//	void Target_Follow(_float fTimeDelta, CGameObject* pGameObject);
//	void Going_Target(_float fTimeDelta, CGameObject* pGameObject);
//	void Position_Target(_float fTimeDelta, _float3 pPosition);
//	void Return_Camera(_float fTimeDelta);
//
//private:
//	CAM_MODE m_eCamMode = MODE_IDLE;
//	_long m_lMouseWheel = 0;
//	_float3 m_vDistance;
//	_float3	m_vInitDistance;
//
//private:
//	class CGameObject* m_pTarget = nullptr;
//	_float3 m_TargetPos;
//	_bool m_bTalkingMode = false;
//	_bool m_bOutZoom = false;
//	_bool m_bChange = false;
//	_float3 m_vPosition;
//	_float3	m_vOffSetDistance;
//
//public:
//	static CCamera_Target* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
//	virtual CCamera* Clone(void* pArg);
//	virtual void Free() override;
//
//};
//END