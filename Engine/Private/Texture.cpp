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
			return E_FAIL;

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
		ERR_MSG(TEXT("Failed to Created : CTexture"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CComponent * CTexture::Clone(void * pArg)
{
	CTexture*	pInstance = new CTexture(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CTexture"));
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
