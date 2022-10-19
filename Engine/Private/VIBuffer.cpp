#include "..\Public\VIBuffer.h"

CVIBuffer::CVIBuffer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
}

CVIBuffer::CVIBuffer(const CVIBuffer & rhs)
	: CComponent(rhs)
	, m_pVB(rhs.m_pVB)
	, m_pIB(rhs.m_pIB)
	, m_iNumVertices(rhs.m_iNumVertices)
	, m_iStride(rhs.m_iStride)
	, m_iNumVertexBuffers(rhs.m_iNumVertexBuffers)
	, m_iIndicesByte(rhs.m_iIndicesByte)
	, m_iNumPrimitive(rhs.m_iNumPrimitive)
	, m_iNumIndicesPerPrimitive(rhs.m_iNumIndicesPerPrimitive)
	, m_eFormat(rhs.m_eFormat)
	, m_eTopology(rhs.m_eTopology)
{	
	Safe_AddRef(m_pVB);
	Safe_AddRef(m_pIB);
}

HRESULT CVIBuffer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CVIBuffer::Render()
{
	ID3D11Buffer*		pBuffer[] = {
		m_pVB, 		
	};

	_uint				iStrides[] = {
		m_iStride,
	};

	_uint				iOffsets[] = {
		0, 
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pBuffer, iStrides, iOffsets);
	m_pContext->IASetIndexBuffer(m_pIB, m_eFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_eTopology);
	// m_pContext->IASetInputLayout();

	m_pContext->DrawIndexed(m_iNumIndicesPerPrimitive * m_iNumPrimitive, 0, 0);

	return S_OK;
}

HRESULT CVIBuffer::Create_VertexBuffer()
{
	/* 메모리 공간을 할당한다.(Vertices) */
	return m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubResourceData, &m_pVB);
}

HRESULT CVIBuffer::Create_IndexBuffer()	
{
	return m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubResourceData, &m_pIB);
}

void CVIBuffer::Free()
{
	__super::Free();

	Safe_Release(m_pVB);
	Safe_Release(m_pIB);
}
