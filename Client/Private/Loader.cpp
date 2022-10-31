#include "stdafx.h"

#include "Loader.h"
#include "GameInstance.h"
#include "Camera_Dynamic.h"
#include "Player.h"

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

unsigned int APIENTRY Thread_Main(void* pArg)
{
	CLoader* pLoader = (CLoader*)pArg;

	EnterCriticalSection(&pLoader->Get_CriticalSection());

	switch (pLoader->Get_NextLevelID())
	{
	case LEVEL_LOGO:
		pLoader->Loading_ForLogoLevel();
		break;
	case LEVEL_GAMEPLAY:
		pLoader->Loading_ForGamePlayLevel();
		break;
	}

	LeaveCriticalSection(&pLoader->Get_CriticalSection());

	return 0;
}

HRESULT CLoader::Initialize(LEVEL eNextLevel)
{
	CoInitializeEx(nullptr, 0);

	m_eNextLevel = eNextLevel;

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, Thread_Main, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;
		
	return S_OK;
}

HRESULT CLoader::Loading_ForLogoLevel()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	Safe_AddRef(pGameInstance);

#pragma region Loading_Textures
	lstrcpy(m_szLoadingText, TEXT("Loading Textures.."));
	
	// >
	// .. Add Above ..
#pragma endregion Loading_Textures

#pragma region Loading_Models
	lstrcpy(m_szLoadingText, TEXT("Loading Models.."));
	
	// >
	// .. Add Above ..
#pragma endregion Loading_Models

#pragma region Loading_Shaders
	lstrcpy(m_szLoadingText, TEXT("Loading Shaders.."));	
	
	// >
	// .. Add Above ..
#pragma endregion Loading_Shaders

#pragma region Loading_Objects
	lstrcpy(m_szLoadingText, TEXT("Loading Objects.."));
	
	// >
	// .. Add Above ..
#pragma endregion Loading_Objects

	m_isFinished = true;
	lstrcpy(m_szLoadingText, TEXT("Loading Completed."));
	
	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_ForGamePlayLevel()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (pGameInstance == nullptr)
		return E_FAIL;

	Safe_AddRef(pGameInstance);

#pragma region Loading_Textures
	lstrcpy(m_szLoadingText, TEXT("Loading Textures.."));

	// >
	// .. Add Above ..
#pragma endregion Loading_Textures

#pragma region Loading_Models
	lstrcpy(m_szLoadingText, TEXT("Loading Models.."));

	/*For.Prototype_Component_Model_Fiona*/
	_matrix PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Fiona"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../Resources/Meshes/Fiona/Fiona.fbx", PivotMatrix))))
		return E_FAIL;

	// >
	// .. Add Above ..
#pragma endregion Loading_Models

#pragma region Loading_Shaders
	lstrcpy(m_szLoadingText, TEXT("Loading_Shaders.."));

	/* For.Prototype_Component_Shader_VtxNorTex*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxNorTex"), 
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Shaderfiles/Shader_VtxNorTex.hlsl"), VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxModel */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Shaderfiles/Shader_VtxModel.hlsl"), VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxAnimModel */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Shaderfiles/Shader_VtxAnimModel.hlsl"), VTXANIMMODEL_DECLARATION::Elements, VTXANIMMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	// >
	// .. Add Above ..
#pragma endregion Loading_Shaders

#pragma region Loading_Objects
	lstrcpy(m_szLoadingText, TEXT("Loading_Objects.."));

	/*For.Prototype_GameObject_Camera_Dynamic */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Dynamic"),
		CCamera_Dynamic::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For.Prototype_GameObject_Player*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// >
	// .. Add Above ..
#pragma endregion Loading_Objects

	m_isFinished = true;
	lstrcpy(m_szLoadingText, TEXT("Loading Completed."));

	Safe_Release(pGameInstance);

	return S_OK;
}

CLoader* CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevel)
{
	CLoader* pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevel)))
	{
		ERR_MSG(TEXT("Failed to Created : CLoader"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	CloseHandle(m_hThread);

	DeleteCriticalSection(&m_CriticalSection);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}