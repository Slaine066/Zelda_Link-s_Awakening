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
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd, _uint iWinCX, _uint iWinCY);
	void Tick();
	void Compute_LocalRayInfo(class CTransform* pTransform, _vector& vRayPosition, _vector& vRayDirection);

private:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;

	HWND m_hWnd;
	_uint m_iWinCX, m_iWinCY;

	_float3	m_vRayDirection, m_vRayPosition;

public:
	virtual void Free() override;
};
END