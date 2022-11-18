#include "TriggerBox.h"
#include "DebugDraw.h"
#include "GameInstance.h"

CTriggerBox::CTriggerBox(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CTriggerBox::CTriggerBox(const CTriggerBox & rhs)
	: CGameObject(rhs)
	, m_pBatch(rhs.m_pBatch)
	, m_pEffect(rhs.m_pEffect)
	, m_pInputLayout(rhs.m_pInputLayout)
{
	Safe_AddRef(m_pInputLayout);
}

HRESULT CTriggerBox::Initialize_Prototype()
{
#ifdef _DEBUG
	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);
	m_pEffect = new BasicEffect(m_pDevice);

	m_pEffect->SetVertexColorEnabled(true);

	const void*	pShaderbyteCode = nullptr;
	size_t iShaderByteCodeLength = 0;
	m_pEffect->GetVertexShaderBytecode(&pShaderbyteCode, &iShaderByteCodeLength);

	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderbyteCode, iShaderByteCodeLength, &m_pInputLayout)))
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT CTriggerBox::Initialize(void * pArg)
{
	if (!pArg)
		return E_FAIL;

	ZeroMemory(&m_tTriggerDesc, sizeof(TRIGGERDESC));
	memcpy(&m_tTriggerDesc, pArg, sizeof(TRIGGERDESC));

	m_pTriggerBox = new BoundingBox(_float3(0.f, 0.f, 0.f), _float3(0.5f, 0.5f, 0.5f));
	m_pTriggerBox->Transform(*m_pTriggerBox, XMLoadFloat4x4(&m_tTriggerDesc.mWorldMatrix));

	return S_OK;
}

_uint CTriggerBox::Tick(_float fTimeDelta)
{
	if (Collision(m_tTriggerDesc.pTargetCollider))
		m_tTriggerDesc.TriggerCallback();

	return OBJ_NOEVENT;
}

HRESULT CTriggerBox::Render()
{
	m_pBatch->Begin();
	m_pContext->IASetInputLayout(m_pInputLayout);
	m_pEffect->SetWorld(XMMatrixIdentity());

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	m_pEffect->SetView(pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
	m_pEffect->SetProjection(pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));
	RELEASE_INSTANCE(CGameInstance);

	m_pEffect->Apply(m_pContext);

	// Render Green Collider when not Collided \ Render Red Collider when Collided.
	_vector	vColor = /*m_isCollision == true ? XMVectorSet(.88f, .19f, .38f, 1.f) : */XMVectorSet(.31f, .78f, .47f, 1.f);

	DX::Draw(m_pBatch, *m_pTriggerBox, vColor);
		
	m_pBatch->End();

	return S_OK;
}

_bool CTriggerBox::Collision(CCollider * pTargetCollider)
{
	m_isCollision = false;

	switch (pTargetCollider->Get_Type())
	{
	case CCollider::TYPE::TYPE_AABB:
		m_isCollision = m_pTriggerBox->Intersects(*pTargetCollider->Get_AABB());
		break;
	case CCollider::TYPE::TYPE_OBB:
		m_isCollision = m_pTriggerBox->Intersects(*pTargetCollider->Get_OBB());
		break;
	case CCollider::TYPE::TYPE_SPHERE:
		m_isCollision = m_pTriggerBox->Intersects(*pTargetCollider->Get_Sphere());
		break;
	}

	return m_isCollision;
}

CTriggerBox * CTriggerBox::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTriggerBox* pInstance = new CTriggerBox(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Create: CTriggerBox"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTriggerBox::Clone(void * pArg)
{
	CTriggerBox* pInstance = new CTriggerBox(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Clone: CTriggerBox"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTriggerBox::Free()
{
	__super::Free();

	Safe_Delete(m_pBatch);
	Safe_Delete(m_pEffect);
	Safe_Release(m_pInputLayout);

	/*	Safe_Delete(m_pTriggerBox);	*/
}