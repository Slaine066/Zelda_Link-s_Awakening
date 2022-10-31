#include "HierarchyNode.h"

CHierarchyNode::CHierarchyNode()
{
}

HRESULT CHierarchyNode::Initialize(const aiNode * pNode, CHierarchyNode* pParent)
{
	strcpy_s(m_szName, pNode->mName.data);
	memcpy(&m_TransformationMatrix, &pNode->mTransformation, sizeof(_float4x4));

	XMStoreFloat4x4(&m_OffsetMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));
	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

	m_pParent = pParent;

	Safe_AddRef(m_pParent);

	return S_OK;
}

void CHierarchyNode::Invalidate_CombinedTransformationmatrix()
{
	if (m_pParent != nullptr)
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&m_pParent->m_CombinedTransformationMatrix));
	else
		m_CombinedTransformationMatrix = m_TransformationMatrix;
}

CHierarchyNode* CHierarchyNode::Create(const aiNode* pNode, CHierarchyNode* pParent)
{
	CHierarchyNode* pInstance = new CHierarchyNode();

	if (FAILED(pInstance->Initialize(pNode, pParent)))
	{
		ERR_MSG(TEXT("Failed to Created : CTransform"));
		Safe_Release(pInstance);
	}
	return pInstance;	
}

void CHierarchyNode::Free()
{
	Safe_Release(m_pParent);
}