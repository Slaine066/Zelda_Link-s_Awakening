#include "Renderer.h"
#include "GameObject.h"
#include "TargetManager.h"
#include "Light_Manager.h"
#include "VIBuffer_Rect.h"
#include "Shader.h"
#include "Component.h"
#include "PipeLine.h"

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
	, m_pTargetManager(CTargetManager::Get_Instance())
	, m_pLight_Manager(CLight_Manager::Get_Instance())
{
	Safe_AddRef(m_pLight_Manager);
	Safe_AddRef(m_pTargetManager);
}

HRESULT CRenderer::Initialize_Prototype()
{
	if (!m_pTargetManager)
		return E_FAIL;

	D3D11_VIEWPORT ViewportDesc;
	ZeroMemory(&ViewportDesc, sizeof ViewportDesc);

	_uint iNumViewports = 1;
	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	/* 렌더타겟들을 추가한다. */

	/* For.Target_Diffuse */
	if (FAILED(m_pTargetManager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Diffuse"), ViewportDesc.Width, ViewportDesc.Height,
		DXGI_FORMAT_R8G8B8A8_UNORM, &_float4(1.f, 1.f, 1.f, 0.f))))
		return E_FAIL;

	/* For.Target_Normal */
	if (FAILED(m_pTargetManager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Normal"), ViewportDesc.Width, ViewportDesc.Height,
		DXGI_FORMAT_R8G8B8A8_UNORM, &_float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	/* For.Target_Depth */
	if (FAILED(m_pTargetManager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Depth"), ViewportDesc.Width, ViewportDesc.Height,
		DXGI_FORMAT_R32G32B32A32_FLOAT, &_float4(0.0f, 0.0f, 0.0f, 0.0f))))
		return E_FAIL;

	/* For.Target_Specular */
	if (FAILED(m_pTargetManager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Specular"), ViewportDesc.Width, ViewportDesc.Height,
		DXGI_FORMAT_R8G8B8A8_UNORM, &_float4(.3f, .3f, .3f, 1.f))))
		return E_FAIL;

	/* For.Target_Shade */
	if (FAILED(m_pTargetManager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Shade"), ViewportDesc.Width, ViewportDesc.Height,
		DXGI_FORMAT_R8G8B8A8_UNORM, &_float4(0.0f, 0.0f, 0.0f, 1.f))))
		return E_FAIL;

	/* For.Target_Reflection */
	if (FAILED(m_pTargetManager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Reflection"), ViewportDesc.Width, ViewportDesc.Height,
		DXGI_FORMAT_R8G8B8A8_UNORM, &_float4(0.0f, 0.0f, 0.0f, 0.0f))))
		return E_FAIL;

	/* For.MRT_Deferred */
	if (FAILED(m_pTargetManager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Normal"))))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Depth"))))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Specular"))))
		return E_FAIL;

	/* For.MRT_LightAcc */
	if (FAILED(m_pTargetManager->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Reflection"))))
		return E_FAIL;

	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../../ShaderFiles/Shader_Deferred.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f)));

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
	m_WorldMatrix._11 = ViewportDesc.Width;
	m_WorldMatrix._22 = ViewportDesc.Height;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_WorldMatrix)));

#ifdef _DEBUG
	if (FAILED(m_pTargetManager->Ready_Debug(TEXT("Target_Diffuse"), 100.f, 100.f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Ready_Debug(TEXT("Target_Normal"), 100.f, 300.f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Ready_Debug(TEXT("Target_Depth"), 100.f, 500.f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Ready_Debug(TEXT("Target_Specular"), 100.f, 700.f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Ready_Debug(TEXT("Target_Shade"), 300.f, 100.f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Ready_Debug(TEXT("Target_Reflection"), 300.f, 300.f, 200.f, 200.f)))
		return E_FAIL;
#endif

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
	if (FAILED(Render_Lights()))
		return E_FAIL;
	if (FAILED(Render_Blend()))
		return E_FAIL;
	if (FAILED(Render_NonLight()))
		return E_FAIL;
	if (FAILED(Render_AlphaBlend()))
		return E_FAIL;

#ifdef _DEBUG	
	if (FAILED(Render_Debug()))
		return E_FAIL;
#endif // _DEBUG

	if (FAILED(Render_UI()))
		return E_FAIL;

	return S_OK;
}

#ifdef _DEBUG
HRESULT CRenderer::Add_Debug(CGameObject * pDebugObj)
{
	m_DebugObjects.push_back(pDebugObj);

	Safe_AddRef(pDebugObj);

	return S_OK;
}
HRESULT CRenderer::Add_Debug(CComponent * pDebugCom)
{
	m_DebugComponents.push_back(pDebugCom);

	Safe_AddRef(pDebugCom);

	return S_OK;
}
#endif // _DEBUG

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
	if (!m_pTargetManager)
		return E_FAIL;

	/* 현재까지는 백버퍼가 셋팅되어있었지만.
	빛연산ㅇ에 필요한 정보를 받아오기위해 MRT_Deferred타겟들을 바인딩한다. */
	/* Target_Diffuse, Target_Normal에 그린다. */
	if (FAILED(m_pTargetManager->Begin_MRT(m_pContext, TEXT("MRT_Deferred"))))
		return E_FAIL;

	for (auto& pGameObject : m_GameObjects[RENDER_NONALPHABLEND])
	{
		if (nullptr != pGameObject)
		{
			pGameObject->Render();
			Safe_Release(pGameObject);
		}
	}

	m_GameObjects[RENDER_NONALPHABLEND].clear();

	if (FAILED(m_pTargetManager->End_MRT(m_pContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Lights()
{
	if (!m_pTargetManager || !m_pLight_Manager)
		return E_FAIL;

	/* Target_Shader에 그린다. */
	if (FAILED(m_pTargetManager->Begin_MRT(m_pContext, TEXT("MRT_LightAcc"))))
		return E_FAIL;

	/* 노말 렌더타겟의 SRV를 셰이더에 바인딩 한다. */
	if (FAILED(m_pTargetManager->Bind_ShaderResource(TEXT("Target_Normal"), m_pShader, "g_NormalTexture")))
		return E_FAIL;

	if (FAILED(m_pTargetManager->Bind_ShaderResource(TEXT("Target_Depth"), m_pShader, "g_DepthTexture")))
		return E_FAIL;

	if (FAILED(m_pTargetManager->Bind_ShaderResource(TEXT("Target_Specular"), m_pShader, "g_SpecularTexture")))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	CPipeLine* pPipeLine = GET_INSTANCE(CPipeLine);

	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrixInv", &pPipeLine->Get_TransformFloat4x4_Inverse_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrixInv", &pPipeLine->Get_TransformFloat4x4_Inverse_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_RawValue("g_vCamPosition", &pPipeLine->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	RELEASE_INSTANCE(CPipeLine);

	if (FAILED(m_pLight_Manager->Render_Lights(m_pShader, m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(m_pTargetManager->End_MRT(m_pContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Blend()
{
	/* 백버퍼에 그린다. */
	if (!m_pTargetManager)
		return E_FAIL;

	if (FAILED(m_pTargetManager->Bind_ShaderResource(TEXT("Target_Diffuse"), m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Bind_ShaderResource(TEXT("Target_Shade"), m_pShader, "g_ShadeTexture")))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Bind_ShaderResource(TEXT("Target_Reflection"), m_pShader, "g_ReflectionTexture")))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	m_pShader->Begin(3);
	m_pVIBuffer->Render();

	return S_OK;
}

HRESULT CRenderer::Render_NonLight()
{
	for (auto& pGameObject : m_GameObjects[RENDER_NONLIGHT])
	{
		if (nullptr != pGameObject)
		{
			pGameObject->Render();
			Safe_Release(pGameObject);
		}
	}

	m_GameObjects[RENDER_NONLIGHT].clear();

	return S_OK;
}

HRESULT CRenderer::Render_AlphaBlend()
{
	m_GameObjects[RENDER_ALPHABLEND].sort([](CGameObject* pSour, CGameObject* pDest)
	{
		return pSour->Get_CamDistance() > pDest->Get_CamDistance();
	});

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

HRESULT CRenderer::Render_Debug()
{
	if (!m_pShader || !m_pVIBuffer)
		return E_FAIL;

	for (auto& pGameObject : m_DebugObjects)
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	for (auto& pComponent : m_DebugComponents)
	{
		if (nullptr != pComponent)
			pComponent->Render();

		Safe_Release(pComponent);
	}

	m_DebugComponents.clear();

	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pTargetManager->Render_Debug(TEXT("MRT_Deferred"), m_pShader, m_pVIBuffer)))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Render_Debug(TEXT("MRT_LightAcc"), m_pShader, m_pVIBuffer)))
		return E_FAIL;

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

	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pShader);

	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pTargetManager);
}