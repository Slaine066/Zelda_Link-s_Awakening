#include "VIBuffer_Navigation.h"

CVIBuffer_Navigation::CVIBuffer_Navigation(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Navigation::CVIBuffer_Navigation(const CVIBuffer_Navigation & rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Navigation::Initialize_Prototype(const _float3* pPoints)
{
#pragma region Vertices
	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));

	m_iStride = sizeof(VTXPOS);
	m_iNumVertices = 3;
	m_iNumVertexBuffers = 1;
	m_eFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	VTXPOS* pVertices = new VTXPOS[m_iNumVertices];
	memcpy(pVertices, pPoints, sizeof(VTXPOS) * m_iNumVertices);

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
#pragma endregion

#pragma region Indices
	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));

	m_iNumPrimitive = 3;
	m_BufferDesc.ByteWidth = sizeof(_ushort) * 4;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = sizeof(_ushort);

	_ushort* pIndices = new _ushort[4];

	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;
	pIndices[3] = 0;

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Navigation::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CVIBuffer_Navigation::Render()
{
	ID3D11Buffer* pBuffer[] = { m_pVB };
	_uint iStrides[] = { m_iStride };
	_uint iOffsets[] = { 0 };

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pBuffer, iStrides, iOffsets);
	m_pContext->IASetIndexBuffer(m_pIB, m_eFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_eTopology);

	m_pContext->DrawIndexed(4, 0, 0);

	return S_OK;
}

CVIBuffer_Navigation * CVIBuffer_Navigation::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _float3* pPoints)
{
	CVIBuffer_Navigation* pInstance = new CVIBuffer_Navigation(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pPoints)))
	{
		ERR_MSG(TEXT("Failed to Create: CVIBuffer_Navigation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_Navigation::Clone(void * pArg)
{
	CVIBuffer_Navigation* pInstance = new CVIBuffer_Navigation(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Clone: CVIBuffer_Navigation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Navigation::Free()
{
	__super::Free();
}