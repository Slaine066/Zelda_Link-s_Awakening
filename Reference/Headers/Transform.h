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
		_float3		vInitialPosition = { 0, 0, 0 };
		float		fSpeedPerSec;
		float		fRotationPerSec;
	}TRANSFORMDESC;

private:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

public:
	void Set_TransformDesc(const TRANSFORMDESC& TransformDesc) { m_TransformDesc = TransformDesc; }

	/* 리턴받은 행렬을 이용해 연산을 해야할 때. */
	_matrix Get_WorldMatrix() const { return XMLoadFloat4x4(&m_WorldMatrix); }

	/* 리턴받은 행렬보관해야할 때  */
	_float4x4 Get_World4x4() const { return m_WorldMatrix; }

	/* 리턴받은 행렬을 셰이더에 던지기위해.  */
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
	void Go_Straight(_float fTimeDelta);
	void Go_Backward(_float fTimeDelta);
	void Go_Left(_float fTimeDelta);
	void Go_Right(_float fTimeDelta);	
	void Turn(_fvector vAxis, _float fTimeDelta);
	void Set_Rotation(_float3 fAngle);
	void LookAt(_fvector vAt);

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