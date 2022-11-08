//#pragma once
//
//#include "VIBuffer.h"
//
///* navigation안에 포함되어있는 쎌하나를 그리기위한 정점, 인덱스 버퍼이다. */
//
//BEGIN(Engine)
//
//class ENGINE_DLL CVIBuffer_Navigation final : public CVIBuffer
//{
//protected:
//	CVIBuffer_Navigation(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
//	CVIBuffer_Navigation(const CVIBuffer_Navigation& rhs);
//	virtual ~CVIBuffer_Navigation() = default;
//
//public:
//	virtual HRESULT Initialize_Prototype(const _float3* pPoints);
//	virtual HRESULT Initialize(void* pArg);
//	virtual HRESULT Render() override;
//
//public:
//	static CVIBuffer_Navigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints);
//	virtual CComponent* Clone(void* pArg = nullptr);
//	virtual void Free() override;
//};
//
//END
