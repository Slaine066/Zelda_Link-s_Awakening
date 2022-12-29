#include "stdafx.h"

#include "UI_ItemChip.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "PipeLine.h"

CUI_ItemChip::CUI_ItemChip(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{

}

CUI_ItemChip::CUI_ItemChip(const CUI_ItemChip & rhs)
	: CUI(rhs)
{

}

HRESULT CUI_ItemChip::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_ItemChip::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

_uint CUI_ItemChip::Tick(_float fTimeDelta)
{
	if (FAILED(__super::Tick(fTimeDelta)))
		return E_FAIL;

	if (CInventory::Get_Instance()->Get_ItemCounter(m_iSlotIndex) == 0)
		m_bShouldDestroy = true;

	if (m_bShouldDestroy)
		return OBJ_DESTROY;

	return OBJ_NOEVENT;
}

_uint CUI_ItemChip::Late_Tick(_float fTimeDelta)
{
	if (FAILED(__super::Late_Tick(fTimeDelta)))
		return E_FAIL;

	return OBJ_NOEVENT;
}

HRESULT CUI_ItemChip::Render()
{
	switch (CUI_Manager::Get_Instance()->Get_Mode())
	{
		case CUI_Manager::MODE::MODE_GAME:
			if (m_eType == CHIP_TYPE::CHIP_INVENTORY)
				return S_OK;
			break;
		case CUI_Manager::MODE::MODE_INVENTORY:
			if (m_eType == CHIP_TYPE::CHIP_GAME)
				return S_OK;
			break;
		default:
			return S_OK;
	}

	if (!m_pShaderCom || !m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(VTXTEX_UI_BLEND);
	m_pVIBufferCom->Render();

	_uint iCounter = CInventory::Get_Instance()->Get_ItemCounter(m_iSlotIndex);
	wsprintf(m_szCounter, TEXT("%d"), iCounter);
	CGameInstance::Get_Instance()->Render_Font(TEXT("Quicksand-24"), m_szCounter, XMVectorSet(m_fScreenX, m_fScreenY, 0.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 1.f));

	return S_OK;
}

HRESULT CUI_ItemChip::Ready_Components(void * pArg)
{
	ZeroMemory(&m_tUIDesc, sizeof(UIDESC));
	memcpy(&m_tUIDesc, (UIDESC*)pArg, sizeof(UIDESC));

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;
	/* For.Com_Transform */
	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;
	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;
	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_ItemChip"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_ItemChip::SetUp_ShaderResources()
{
	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(0))))
		return E_FAIL;

	return S_OK;
}

_float2 CUI_ItemChip::Get_ChipPosition()
{
	_float2 vPosition = _float2(m_fScreenX, m_fScreenY);
	return vPosition;
}

CUI_ItemChip * CUI_ItemChip::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_ItemChip* pInstance = new CUI_ItemChip(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Create: CUI_ItemChip"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI_ItemChip::Clone(void * pArg)
{
	CUI_ItemChip* pInstance = new CUI_ItemChip(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Clone: CUI_ItemChip"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_ItemChip::Free()
{
	__super::Free();
}