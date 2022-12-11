#pragma once

/* 카메라에서 갱신해주는 뷰행렬, 투영행려을 보관한다. */
/* 상황에 따라 리턶나다. */
#include "Base.h"

BEGIN(Engine)

class CPipeLine final : public CBase
{
	DECLARE_SINGLETON(CPipeLine)
public:
	enum TRANSFORMSTATE { D3DTS_VIEW, D3DTS_PROJ, D3DTS_END };
private:
	CPipeLine();
	virtual ~CPipeLine() = default;

public:
	void Set_Transform(TRANSFORMSTATE eState, _fmatrix TransformMatrix) {
		XMStoreFloat4x4(&m_PipeLineMatrix[eState], TransformMatrix);
	}

	_matrix Get_TransformMatrix(TRANSFORMSTATE eState) {
		return XMLoadFloat4x4(&m_PipeLineMatrix[eState]);
	}
	_float4x4 Get_TransformFloat4x4(TRANSFORMSTATE eState) {
		return m_PipeLineMatrix[eState];
	}
	_float4x4 Get_TransformFloat4x4_TP(TRANSFORMSTATE eState) {
		return m_PipeLineMatrix_TP[eState];
	}
	_float4x4 Get_TransformFloat4x4_Inverse(TRANSFORMSTATE eState) {
		return m_PipeLineMatrix_Inverse[eState];
	}
	_float4x4 Get_TransformFloat4x4_Inverse_TP(TRANSFORMSTATE eState) {
		return m_PipeLineMatrix_Inverse_TP[eState];
	}

	_float4 Get_CamPosition() {
		return m_vCamPosition;
	}



public:
	void Update();

private:
	_float4x4			m_PipeLineMatrix[D3DTS_END];
	_float4x4			m_PipeLineMatrix_TP[D3DTS_END];
	_float4x4			m_PipeLineMatrix_Inverse[D3DTS_END];
	_float4x4			m_PipeLineMatrix_Inverse_TP[D3DTS_END];
	_float4				m_vCamPosition;

public:
	virtual void Free() override;
};

END