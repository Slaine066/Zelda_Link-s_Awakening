#include "TargetManager.h"
#include "RenderTarget.h"

IMPLEMENT_SINGLETON(CTargetManager)

CTargetManager::CTargetManager()
{

}

HRESULT CTargetManager::Add_RenderTarget(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT eFormat, const _float4 * pColor)
{
	if (nullptr != Find_RenderTarget(pTargetTag))
		return E_FAIL;

	CRenderTarget* pRenderTarget = CRenderTarget::Create(pDevice, pContext, iSizeX, iSizeY, eFormat, pColor);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	m_RenderTargets.emplace(pTargetTag, pRenderTarget);

	return S_OK;
}

HRESULT CTargetManager::Add_MRT(const _tchar * pMRTTag, const _tchar * pTargetTag)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(pTargetTag);

	if (nullptr == pRenderTarget)
		return E_FAIL;

	list<CRenderTarget*>* pMRTList = Find_MRT(pMRTTag);

	if (nullptr == pMRTList)
	{
		list<CRenderTarget*> MRTList;
		MRTList.push_back(pRenderTarget);

		m_MRTs.emplace(pMRTTag, MRTList);
	}
	else
		pMRTList->push_back(pRenderTarget);

	Safe_AddRef(pRenderTarget);

	return S_OK;
}

HRESULT CTargetManager::Begin_MRT(ID3D11DeviceContext* pContext, const _tchar * pMRTTag)
{
	list<CRenderTarget*>* pMRTList = Find_MRT(pMRTTag);
	if (nullptr == pMRTList)
		return E_FAIL;

	pContext->OMGetRenderTargets(1, &m_pOldRTV, &m_pOldDSV);

	ID3D11RenderTargetView* pRTVs[8] = { nullptr };

	_uint iNumRenderTargets = 0;

	for (auto& pRenderTarget : *pMRTList)
	{
		pRenderTarget->Clear();
		pRTVs[iNumRenderTargets++] = pRenderTarget->Get_RTV();
	}

	pContext->OMSetRenderTargets(iNumRenderTargets, pRTVs, m_pOldDSV);

	return S_OK;
}

HRESULT CTargetManager::End_MRT(ID3D11DeviceContext * pContext)
{
	pContext->OMSetRenderTargets(1, &m_pOldRTV, m_pOldDSV);

	Safe_Release(m_pOldRTV);
	Safe_Release(m_pOldDSV);

	return S_OK;
}

HRESULT CTargetManager::Bind_ShaderResource(const _tchar * pTargetTag, CShader * pShader, const char * pConstantName)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(pTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Bind_ShaderResource(pShader, pConstantName);
}

#ifdef _DEBUG
HRESULT CTargetManager::Ready_Debug(const _tchar * pTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(pTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Ready_Debug(fX, fY, fSizeX, fSizeY);
}

HRESULT CTargetManager::Render_Debug(const _tchar * pMRTTag, class CShader* pShader, CVIBuffer_Rect * pVIBuffer)
{
	list<class CRenderTarget*>* pMRTList = Find_MRT(pMRTTag);

	for (auto& pTarget : *pMRTList)
	{
		if (nullptr != pTarget)
			pTarget->Render_Debug(pShader, pVIBuffer);
	}

	return S_OK;
}
#endif // _DEBUG

CRenderTarget * CTargetManager::Find_RenderTarget(const _tchar * pTargetTag)
{
	auto iter = find_if(m_RenderTargets.begin(), m_RenderTargets.end(), CTag_Finder(pTargetTag));

	if (iter == m_RenderTargets.end())
		return nullptr;

	return iter->second;
}

list<class CRenderTarget*>* CTargetManager::Find_MRT(const _tchar * pMRTTag)
{
	auto iter = find_if(m_MRTs.begin(), m_MRTs.end(), CTag_Finder(pMRTTag));

	if (iter == m_MRTs.end())
		return nullptr;

	return &iter->second;
}

void CTargetManager::Free()
{
	for (auto& Pair : m_MRTs)
	{
		for (auto& pRenderTarget : Pair.second)
		{
			Safe_Release(pRenderTarget);
		}
		Pair.second.clear();
	}
	m_MRTs.clear();

	for (auto& Pair : m_RenderTargets)
		Safe_Release(Pair.second);

	m_RenderTargets.clear();

	Safe_Release(m_pOldRTV);
	Safe_Release(m_pOldDSV);
}