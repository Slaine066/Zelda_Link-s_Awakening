#include "..\Public\PipeLine.h"

IMPLEMENT_SINGLETON(CPipeLine)

CPipeLine::CPipeLine()
{
}

void CPipeLine::Update()
{
	XMStoreFloat4(&m_vCamPosition, 
		XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_PipeLineMatrix[D3DTS_VIEW])).r[3]);

	for (_uint i = 0; i < 2; ++i)
	{
		XMStoreFloat4x4(&m_PipeLineMatrix_TP[i], 
			XMMatrixTranspose(XMLoadFloat4x4(&m_PipeLineMatrix[i])));

		XMStoreFloat4x4(&m_PipeLineMatrix_Inverse[i],
			XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_PipeLineMatrix[i])));

		XMStoreFloat4x4(&m_PipeLineMatrix_Inverse_TP[i],
			XMMatrixTranspose(XMLoadFloat4x4(&m_PipeLineMatrix_Inverse[i])));
	}
}

void CPipeLine::Free()
{
}
