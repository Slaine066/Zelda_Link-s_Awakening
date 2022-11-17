#pragma once

#include "Component.h"

BEGIN(Engine)
class ENGINE_DLL CTransform final : public CComponent
{
public:
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_TRANSLATION, STATE_END };

public:
	typedef struct tagTransformDesc
	{
		_float4x4	vInitialWorldMatrix;
		_float		fSpeedPerSec;
		_float		fRotationPerSec;
	}TRANSFORMDESC;

private:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

public:
	TRANSFORMDESC Get_TransformDesc() { return m_TransformDesc; }
	void Set_TransformDesc(const TRANSFORMDESC& TransformDesc) { m_TransformDesc = TransformDesc; }

	void Change_Speed(_float fSpeed) { m_TransformDesc.fSpeedPerSec = fSpeed; }

	_matrix Get_WorldMatrix() const { return XMLoadFloat4x4(&m_WorldMatrix); }
	_float4x4 Get_World4x4() const { return m_WorldMatrix; }

	const _float4x4* Get_World4x4Ptr() const { return &m_WorldMatrix; }

	_float4x4 Get_World4x4_TP() const {
		_float4x4 TransposeMatrix;
		XMStoreFloat4x4(&TransposeMatrix, XMMatrixTranspose(Get_WorldMatrix()));
		return TransposeMatrix;
	}

	_vector Get_State(STATE eState) const { return XMLoadFloat4((_float4*)&m_WorldMatrix.m[eState][0]); }	
	void Set_State(STATE eState, _fvector vState);

	_float Get_Scale(STATE eState) { return XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_WorldMatrix).r[eState])); }
	void Set_Scale(STATE eState, _float fScale);

	_float Get_CurrentRotationX() { return m_fCurrentRotationX; }
	_float Get_CurrentRotationY() { return m_fCurrentRotationY; }
	_float Get_CurrentRotationZ() { return m_fCurrentRotationZ; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

public:
	void Move_Straight(_float fTimeDelta);
	void Move_Backward(_float fTimeDelta);
	void Move_Left(_float fTimeDelta);
	void Move_Right(_float fTimeDelta);

	void Go_Straight(_float fTimeDelta, class CNavigation* pNavigation = nullptr);
	_bool Go_TargetPosition(_float fTimeDelta, _float3 vTargetPosition, _float fDistance, class CNavigation* pNavigation = nullptr);

	void Turn(_fvector vAxis, _float fTimeDelta);
	void Set_Rotation(_float3 fAngle);
	void Set_RotationY(_float fAngleY);
	
	void LookAt(_fvector vAt);
	void Attach_ToTarget(_fvector fTargetPosition, _fvector fDistance);
	

private:			
	TRANSFORMDESC m_TransformDesc;

	_float4x4 m_WorldMatrix;
	_float m_fCurrentRotationX = 0.f;
	_float m_fCurrentRotationY = 0.f;
	_float m_fCurrentRotationZ = 0.f;

public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END