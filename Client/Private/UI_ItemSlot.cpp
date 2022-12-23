#include "stdafx.h"

#include "UI_ItemSlot.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "PipeLine.h"

CUI_ItemSlot::CUI_ItemSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{

}

CUI_ItemSlot::CUI_ItemSlot(const CUI_ItemSlot & rhs)
	: CUI(rhs)
{

}

HRESULT CUI_ItemSlot::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_ItemSlot::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

_uint CUI_ItemSlot::Tick(_float fTimeDelta)
{
	if (FAILED(__super::Tick(fTimeDelta)))
		return E_FAIL;

	return OBJ_NOEVENT;
}

_uint CUI_ItemSlot::Late_Tick(_float fTimeDelta)
{
	if (FAILED(__super::Late_Tick(fTimeDelta)))
		return E_FAIL;

	return OBJ_NOEVENT;
}

HRESULT CUI_ItemSlot::Render()
{
	switch (CUI_Manager::Get_Instance()->Get_Mode())
	{
		case CUI_Manager::MODE::MODE_GAME:
			if (m_eType == SLOT_TYPE::SLOT_INVENTORY || m_eType == SLOT_TYPE::SLOT_INVENTORY_X || m_eType == SLOT_TYPE::SLOT_INVENTORY_Y)
				return S_OK;
			break;
		case CUI_Manager::MODE::MODE_INVENTORY:
			if (m_eType == SLOT_TYPE::SLOT_GAME_X || m_eType == SLOT_TYPE::SLOT_GAME_Y)
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

	return S_OK;
}

HRESULT CUI_ItemSlot::Ready_Components(void * pArg)
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
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_ItemSlot"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_ItemSlot::SetUp_ShaderResources()
{
	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(m_eType))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_Hovered", &m_bIsHovered, sizeof(_bool))))
		return E_FAIL;

	return S_OK;
}

_float2 CUI_ItemSlot::Get_SlotPosition()
{
	_float2 vPosition = _float2(m_fScreenX, m_fScreenY); 
	return vPosition;
}

CUI_ItemSlot * CUI_ItemSlot::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_ItemSlot* pInstance = new CUI_ItemSlot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Create: CUI_ItemSlot"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI_ItemSlot::Clone(void * pArg)
{
	CUI_ItemSlot* pInstance = new CUI_ItemSlot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Clone: CUI_ItemSlot"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_ItemSlot::Free()
{
	__super::Free();
}