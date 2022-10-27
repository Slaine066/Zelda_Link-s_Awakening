#include "..\Public\HierarchyNode.h"

CHierarchyNode::CHierarchyNode()
{
}

HRESULT CHierarchyNode::Initialize(const aiNode * pNode, CHierarchyNode* pParent)
{
	strcpy_s(m_szName, pNode->mName.data);

	XMStoreFloat4x4(&m_OffsetMatrix, XMMatrixIdentity());

	/* 언제든 바뀔 수 있다. 애니메이션이 재생되면. */
	memcpy(&m_TransformationMatrix, &pNode->mTransformation, sizeof(_float4x4));

	XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));

	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());


	
	
	m_pParent = pParent;

	Safe_AddRef(m_pParent);

	return S_OK;
}

void CHierarchyNode::Invalidate_CombinedTransformationmatrix()
{
	if (nullptr != m_pParent)
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&m_pParent->m_CombinedTransformationMatrix));
}

CHierarchyNode * CHierarchyNode::Create(const aiNode* pNode, CHierarchyNode* pParent)
{
	CHierarchyNode*		pInstance = new CHierarchyNode();

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