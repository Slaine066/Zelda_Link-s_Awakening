#include "..\Public\Light.h"

CLight::CLight(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CLight::Initialize(const LIGHTDESC & LightDesc)
{
	m_LightDesc = LightDesc;

	return S_OK;
}

CLight * CLight::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const LIGHTDESC & LightDesc)
{
	CLight*	pInstance = new CLight(pDevice, pContext);

	if (FAILED(pInstance->Initialize(LightDesc)))
	{
		ERR_MSG(TEXT("Failed to Created : CLight"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLight::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
