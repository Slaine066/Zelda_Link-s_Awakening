#pragma once

/* 뼈. */
#include "Base.h"

BEGIN(Engine)

class CHierarchyNode final : public CBase
{
private:
	CHierarchyNode();
	virtual ~CHierarchyNode() = default;

public:
	const char* Get_Name() const {
		return m_szName;
	}

	void Set_OffsetMatrix(_fmatrix OffsetMatrix) {
		XMStoreFloat4x4(&m_OffsetMatrix, OffsetMatrix);
	}

	void Set_TransformationMatrix(_fmatrix TransformationMatrix) {
		XMStoreFloat4x4(&m_TransformationMatrix, TransformationMatrix);
	}

	_matrix Get_OffsetMatrix() const {
		return XMLoadFloat4x4(&m_OffsetMatrix);
	}

	_matrix Get_CombinedTransformationMatrix() const {
		return XMLoadFloat4x4(&m_CombinedTransformationMatrix);
	}

public:
	HRESULT Initialize(const aiNode* pNode, CHierarchyNode* pParent);
	void Invalidate_CombinedTransformationmatrix();

private:
	char				m_szName[MAX_PATH] = "";

	CHierarchyNode*		m_pParent = nullptr;

	/* 기본은 항등, 메시안에서(aiBone) 값을 채워준다. */
	_float4x4			m_OffsetMatrix;


	_float4x4			m_TransformationMatrix;

	/* m_TransformationMatrix * m_pParent->m_CombinedTransformationMatrix */
	_float4x4			m_CombinedTransformationMatrix;

public:
	static CHierarchyNode* Create(const aiNode* pNode, CHierarchyNode* pParent);
	virtual void Free() override;
};

END