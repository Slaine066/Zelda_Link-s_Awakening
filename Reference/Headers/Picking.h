#pragma once

#include "Base.h"

BEGIN(Engine)
class CPicking final : public CBase
{
	DECLARE_SINGLETON(CPicking)

public:
	CPicking();
	virtual ~CPicking() = default;

public:
	_float3 Get_RayPosition() { return m_vRayPosition; }

public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd, _uint iWinCX, _uint iWinCY);
	void Tick();
	void Compute_LocalRayInfo(class CTransform* pTransform);
	_bool Intersect(_fvector vPointA, _fvector vPointB, _fvector vPointC, _float3& pOut);

private:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;

	HWND m_hWnd;
	_uint m_iWinCX, m_iWinCY;

	_float3	m_vRayDirection, m_vRayPosition;
	_float3	m_vRayDirectionLocal, m_vRayPosistionLocal;

public:
	virtual void Free() override;
};
END