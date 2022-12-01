#include "stdafx.h"

#include "UI.h"
#include "GameInstance.h"

CUI::CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{

}

CUI::CUI(const CUI & rhs)
	: CGameObject(rhs)
{

}

HRESULT CUI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI::Initialize(void * pArg)
{
	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixTranspose(XMMatrixIdentity()));
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f)));

	return S_OK;
}

_uint CUI::Tick(_float fTimeDelta)
{
	m_pTransformCom->Set_Scale(CTransform::STATE_RIGHT, m_tUIDesc.m_fSizeX);
	m_pTransformCom->Set_Scale(CTransform::STATE_UP, m_tUIDesc.m_fSizeY);
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_tUIDesc.m_fX - g_iWinSizeX * 0.5f, - m_tUIDesc.m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

	return OBJ_NOEVENT;
}

_uint CUI::Late_Tick(_float fTimeDelta)
{
	if (m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return OBJ_NOEVENT;
}

HRESULT CUI::Render()
{
	if (!m_pShaderCom || !m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

void CUI::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}