#include "..\Public\Renderer.h"
#include "GameObject.h"

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{

}

HRESULT CRenderer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRenderer::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CRenderer::Add_RenderGroup(RENDERGROUP eRenderGroup, CGameObject * pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_GameObjects[eRenderGroup].push_back(pGameObject);

	Safe_AddRef(pGameObject);	

	return S_OK;
}

HRESULT CRenderer::Render_GameObjects()
{
	if (FAILED(Render_Priority()))
		return E_FAIL;
	if (FAILED(Render_NonAlphaBlend()))
		return E_FAIL;
	if (FAILED(Render_AlphaBlend()))
		return E_FAIL;
	if (FAILED(Render_UI()))
		return E_FAIL;


	return S_OK;
}

HRESULT CRenderer::Render_Priority()
{
	for (auto& pGameObject : m_GameObjects[RENDER_PRIORITY])
	{
		if (nullptr != pGameObject)
		{
			pGameObject->Render();
			Safe_Release(pGameObject);
		}
	}

	m_GameObjects[RENDER_PRIORITY].clear();

	return S_OK;
}

HRESULT CRenderer::Render_NonAlphaBlend()
{
	for (auto& pGameObject : m_GameObjects[RENDER_NONALPHABLEND])
	{
		if (nullptr != pGameObject)
		{
			pGameObject->Render();
			Safe_Release(pGameObject);
		}
	}

	m_GameObjects[RENDER_NONALPHABLEND].clear();
	return S_OK;
}

HRESULT CRenderer::Render_AlphaBlend()
{
	
	//m_GameObjects[RENDER_ALPHABLEND].sort([](CGameObject* pSour, CGameObject* pDest)
	//{
	//	return pSour->Get_CamDistance() > pDest->Get_CamDistance();
	//});

	for (auto& pGameObject : m_GameObjects[RENDER_ALPHABLEND])
	{
		if (nullptr != pGameObject)
		{
			pGameObject->Render();
			Safe_Release(pGameObject);
		}
	}

	m_GameObjects[RENDER_ALPHABLEND].clear();

	return S_OK;
}

HRESULT CRenderer::Render_UI()
{
	for (auto& pGameObject : m_GameObjects[RENDER_UI])
	{
		if (nullptr != pGameObject)
		{
			pGameObject->Render();
			Safe_Release(pGameObject);
		}
	}

	m_GameObjects[RENDER_UI].clear();
	return S_OK;
}

CRenderer * CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRenderer*	pInstance = new CRenderer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CRenderer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CRenderer::Clone(void * pArg)
{
	AddRef();

	return this;
}

void CRenderer::Free()
{
	__super::Free();

}
