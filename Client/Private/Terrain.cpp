#include "stdafx.h"

#include "Terrain.h"
#include "GameInstance.h"

CTerrain::CTerrain(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CTerrain::CTerrain(const CTerrain & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTerrain::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTerrain::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Create_FilterTexture()))
		return E_FAIL;
	
	return S_OK;
}

_uint CTerrain::Tick(_float fTimeDelta)
{
	return OBJ_NOEVENT;
}

void CTerrain::Late_Tick(_float fTimeDelta)
{
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CTerrain::Render()
{
	if (m_pShaderCom == nullptr || m_pVIBufferCom == nullptr)
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);		// Gauraud Shader
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CTerrain::Ready_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxNorTex"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"), (CComponent**)&m_pTextureCom[TYPE_DIFFUSE])))
		return E_FAIL;

	/* For.Com_Brush */
	if (FAILED(__super::Add_Components(TEXT("Com_Brush"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Brush"), (CComponent**)&m_pTextureCom[TYPE_BRUSH])))
		return E_FAIL;

	/* For.Com_Filter */
	if (FAILED(__super::Add_Components(TEXT("Com_Filter"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Filter"), (CComponent**)&m_pTextureCom[TYPE_FILTER])))
		return E_FAIL;	

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::SetUp_ShaderResources()
{
	if (m_pShaderCom == nullptr)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	/* For.Lights */
	const LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(0);
	if (nullptr == pLightDesc)
		return E_FAIL;

	if (pLightDesc->eType == LIGHTDESC::TYPE_DIRECTIONAL)
	{
		if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
			return E_FAIL;

		m_iPassIndex = 0;
	}
	else
	{
		if (FAILED(m_pShaderCom->Set_RawValue("g_vLightPos", &pLightDesc->vPosition, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_RawValue("g_fLightRange", &pLightDesc->fRange, sizeof(_float))))
			return E_FAIL;

		m_iPassIndex = 1;
	}
		
	if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;	

	RELEASE_INSTANCE(CGameInstance);

	ID3D11ShaderResourceView* pSRVs[] = {
		m_pTextureCom[TYPE_DIFFUSE]->Get_SRV(0), 
		m_pTextureCom[TYPE_DIFFUSE]->Get_SRV(1),
	};

	if (FAILED(m_pShaderCom->Set_ShaderResourceViewArray("g_DiffuseTexture", pSRVs, 2)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_BrushTexture", m_pTextureCom[TYPE_BRUSH]->Get_SRV(0))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_FilterTexture", m_pTextureCom[TYPE_FILTER]->Get_SRV(0))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_FilterTexture", m_pFilterTexture)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::Create_FilterTexture()
{
	ID3D11Texture2D* pTexture2D = nullptr;

	D3D11_TEXTURE2D_DESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = 128;
	TextureDesc.Height = 128;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DYNAMIC;
	TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	TextureDesc.MiscFlags = 0;

	_uint* pPixel = new _uint[TextureDesc.Width * TextureDesc.Height];

	for (_uint i = 0; i < TextureDesc.Height; ++i)
	{
		for (_uint j = 0; j < TextureDesc.Width; ++j)
		{
			_uint iIndex = i * TextureDesc.Width + j;

			if(j < 64)
				pPixel[iIndex] = D3DCOLOR_ARGB(255, 255, 255, 255);
			else
				pPixel[iIndex] = D3DCOLOR_ARGB(255, 0, 0, 0);
		}
	}

	D3D11_SUBRESOURCE_DATA SubResourceData;
	ZeroMemory(&SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	SubResourceData.pSysMem = pPixel;
	SubResourceData.SysMemPitch = 128 * 4;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, &SubResourceData, &pTexture2D)))
		return E_FAIL;

	/*D3D11_MAP_WRITE_NO_OVERWRITE :	기존에 있던 값을 유지한상태로 메모리의 주소를 얻어오난다. */
	/*D3D11_MAP_WRITE_DISCARD :			기존에 있던 값은 날리고 메모리 주소를 얻어오난다. */

	D3D11_MAPPED_SUBRESOURCE SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	if (FAILED(m_pContext->Map(pTexture2D, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource)))
		return E_FAIL;

	//pPixel[0] = D3DCOLOR_ARGB(255, 255, 255, 0);
	memcpy(SubResource.pData, pPixel, sizeof(_uint) * TextureDesc.Width * TextureDesc.Height);

	m_pContext->Unmap(pTexture2D, 0);

	if (FAILED(DirectX::SaveDDSTextureToFile(m_pContext, pTexture2D, TEXT("../../Resources/Textures/Terrain/Newfilter.dds"))))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateShaderResourceView(pTexture2D, nullptr, &m_pFilterTexture)))
		return E_FAIL;

	Safe_Delete_Array(pPixel);
	Safe_Release(pTexture2D);

	return S_OK;
}

CTerrain* CTerrain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTerrain* pInstance = new CTerrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CTerrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTerrain::Clone(void * pArg)
{
	CTerrain* pInstance = new CTerrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CTerrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTerrain::Free()
{
	__super::Free();

	Safe_Release(m_pFilterTexture);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);

	for (_uint i = 0; i < TYPE_END; ++i)
		Safe_Release(m_pTextureCom[i]);

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}