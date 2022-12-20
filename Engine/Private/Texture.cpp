#include "..\Public\Texture.h"


CTexture::CTexture(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)	
{

}

CTexture::CTexture(const CTexture & rhs)
	: CComponent(rhs)
	, m_SRVs(rhs.m_SRVs)
{
	for (auto& pSRV : m_SRVs)
		Safe_AddRef(pSRV);
}

ID3D11ShaderResourceView * CTexture::Get_SRV(_uint iIndex)
{
	if (m_SRVs.empty() || iIndex > m_SRVs.size())
		return nullptr; 

	return m_SRVs[iIndex];
}

HRESULT CTexture::Initialize_Prototype(const _tchar* pTextureFilePath, _uint iNumTextures)
{
	_tchar			szFullPath[MAX_PATH] = TEXT("");

	m_SRVs.reserve(iNumTextures);
	
	for (_uint i = 0; i < iNumTextures; ++i)
	{
		wsprintf(szFullPath, pTextureFilePath, i);

		_tchar		szDrive[MAX_PATH] = TEXT("");
		_tchar		szExt[MAX_PATH] = TEXT("");

		/* 경로를 분해한다. */
		_wsplitpath_s(szFullPath, nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);

		HRESULT			hr = 0;		

		ID3D11ShaderResourceView*			pSRV = nullptr;

		if (!lstrcmp(szExt, TEXT(".dds")))
			hr = DirectX::CreateDDSTextureFromFile(m_pDevice, szFullPath, nullptr, &pSRV);
		else
			hr = DirectX::CreateWICTextureFromFile(m_pDevice, szFullPath, nullptr, &pSRV);

		if (FAILED(hr))
			return S_OK; /* Should return E_FAIL, but the Create_Material() function forces loading certain Textures, which can also be missing.
						 Since we don't want Textures error popping up everytime a Texture is missing we just return S_OK. 
						 (The best scenario would be not forcing the loading of specific Texture inside the Create_Material() function. */

		m_SRVs.push_back(pSRV);
	}

	return S_OK; 
}

HRESULT CTexture::Initialize(void * pArg)
{

	return S_OK;
}

CTexture * CTexture::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pTextureFilePath, _uint iNumTextures)
{
	CTexture*	pInstance = new CTexture(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pTextureFilePath, iNumTextures)))
	{
		ERR_MSG(TEXT("Failed to Create: CTexture"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CComponent * CTexture::Clone(void * pArg)
{
	CTexture*	pInstance = new CTexture(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Clone: CTexture"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTexture::Free()
{
	__super::Free();

	for (auto& pSRV : m_SRVs)
		Safe_Release(pSRV);

	m_SRVs.clear();

	
}
