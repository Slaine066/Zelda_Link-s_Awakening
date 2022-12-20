#include "stdafx.h"

#include "Level_MoriblinCave.h"
#include "GameInstance.h"
#include "CameraManager.h"
#include "Camera_Player.h"
#include "Camera_Dungeon.h"
#include "TriggerBox_Dynamic.h"
#include "UI_Manager.h"

CLevel_MoriblinCave::CLevel_MoriblinCave(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext),
	m_pUIManager(CUI_Manager::Get_Instance())
{
	Safe_AddRef(m_pUIManager);
}

HRESULT CLevel_MoriblinCave::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Load_Objects_FromFile()))
		return E_FAIL;

	if (FAILED(Load_Triggers_FromFile()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI()))
		return E_FAIL;

	/* For.Com_Navigation */
	if (FAILED(__super::Add_NavigationMesh(TEXT("Com_Navigation"), LEVEL_MORIBLINCAVE, TEXT("Prototype_Component_Navigation"), (CComponent**)&m_pNavigationMesh)))
		return E_FAIL;

	return S_OK;
}

void CLevel_MoriblinCave::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CLevel_MoriblinCave::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	SetWindowText(g_hWnd, TEXT("MoriblinCave Level."));

	/*CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Down('C'))
	{
		CCamera_Player* pCamera = (CCamera_Player*)CCameraManager::Get_Instance()->Get_CurrentCamera();
		pCamera->Set_ModeZoom(true);
		pCamera->Set_ZoomPosition(_float3(0.f, 0.f, -5.f));
	}
	if (pGameInstance->Key_Down('V'))
	{
		CCamera_Player* pCamera = (CCamera_Player*)CCameraManager::Get_Instance()->Get_CurrentCamera();
		pCamera->Set_ModeZoom(false);
	}
	if (pGameInstance->Key_Down('B'))
	{
		CCamera_Player* pCamera = (CCamera_Player*)(CCameraManager::Get_Instance()->Get_CurrentCamera());
		pCamera->Set_ModeShake(0.5f, 0.1f, 0.01f);
	}

	RELEASE_INSTANCE(CGameInstance);*/
}

HRESULT CLevel_MoriblinCave::Load_Objects_FromFile()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	HANDLE hFile = CreateFile(TEXT("../../Data/MapData/MoriblinCave.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == 0)
		return E_FAIL;

	// First read to get Objects count, so we can resize the vector.
	_ulong dwByte = 0;
	_uint iCounter = 0;

	CActor::MODELDESC tModelDesc;
	ZeroMemory(&tModelDesc, sizeof(CActor::MODELDESC));
	while (true)
	{
		ReadFile(hFile, &tModelDesc, sizeof(CActor::MODELDESC), &dwByte, nullptr);
		if (!dwByte)
			break;

		iCounter++;
	}
	CloseHandle(hFile);

	m_vInstancedObjects.resize(iCounter);

	// Second read to write in specific vector index (since we now the number of Objects and the vector is correctly resized).
	hFile = CreateFile(TEXT("../../Data/MapData/MoriblinCave.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == 0)
		return E_FAIL;

	for (_uint i = 0; i < iCounter; i++)
	{
		ReadFile(hFile, &m_vInstancedObjects[i], sizeof(CActor::MODELDESC), nullptr, nullptr);

		if (wcsstr(m_vInstancedObjects[i].wcObjName, TEXT("MoriblinCave")))
			pGameInstance->Add_GameObject(m_vInstancedObjects[i].wcObjName, TEXT("Prototype_GameObject_StaticObject"), LEVEL_MORIBLINCAVE, m_vInstancedObjects[i].wcLayerTag, &m_vInstancedObjects[i]);
		else if (!wcscmp(m_vInstancedObjects[i].wcObjName, TEXT("Link")))
			pGameInstance->Add_GameObject(m_vInstancedObjects[i].wcObjName, TEXT("Prototype_GameObject_Player"), LEVEL_MORIBLINCAVE, m_vInstancedObjects[i].wcLayerTag, &m_vInstancedObjects[i]);
		else if (!wcscmp(m_vInstancedObjects[i].wcObjName, TEXT("MoriblinSword")))
			pGameInstance->Add_GameObject(m_vInstancedObjects[i].wcObjName, TEXT("Prototype_GameObject_MoriblinSword"), LEVEL_MORIBLINCAVE, m_vInstancedObjects[i].wcLayerTag, &m_vInstancedObjects[i]);
		else if (!wcscmp(m_vInstancedObjects[i].wcObjName, TEXT("MoriblinSpear")))
			pGameInstance->Add_GameObject(m_vInstancedObjects[i].wcObjName, TEXT("Prototype_GameObject_MoriblinSpear"), LEVEL_MORIBLINCAVE, m_vInstancedObjects[i].wcLayerTag, &m_vInstancedObjects[i]);
		else if (!wcscmp(m_vInstancedObjects[i].wcObjName, TEXT("Bossblin")))
			pGameInstance->Add_GameObject(m_vInstancedObjects[i].wcObjName, TEXT("Prototype_GameObject_Bossblin"), LEVEL_MORIBLINCAVE, m_vInstancedObjects[i].wcLayerTag, &m_vInstancedObjects[i]);
		else if (!wcscmp(m_vInstancedObjects[i].wcObjName, TEXT("CaveRock")))
			pGameInstance->Add_GameObject(m_vInstancedObjects[i].wcObjName, TEXT("Prototype_GameObject_StaticObject"), LEVEL_MORIBLINCAVE, m_vInstancedObjects[i].wcLayerTag, &m_vInstancedObjects[i]);
		else if (!wcscmp(m_vInstancedObjects[i].wcObjName, TEXT("Treasure")))
			pGameInstance->Add_GameObject(m_vInstancedObjects[i].wcObjName, TEXT("Prototype_GameObject_Treasure"), LEVEL_MORIBLINCAVE, m_vInstancedObjects[i].wcLayerTag, &m_vInstancedObjects[i]);
	}
	CloseHandle(hFile);

	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CLevel_MoriblinCave::Load_Triggers_FromFile()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	HANDLE hFile = CreateFile(TEXT("../../Data/TriggerData/MoriblinCave.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == 0)
		return E_FAIL;

	_ulong dwByte = 0;

	CTriggerBox::TRIGGERBOXDESC tTriggerBoxDesc;
	
	while (true)
	{
		ZeroMemory(&tTriggerBoxDesc, sizeof(CTriggerBox::TRIGGERBOXDESC));
		ReadFile(hFile, &tTriggerBoxDesc, sizeof(CTriggerBox::TRIGGERBOXDESC), &dwByte, nullptr);

		if (!dwByte)
			break;
		
		CGameObject* pTriggerBox = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_TriggerBox_Dynamic"), &tTriggerBoxDesc);
		m_TriggerBoxes.push_back((CTriggerBox*)pTriggerBox);
	}
	CloseHandle(hFile);

	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CLevel_MoriblinCave::Ready_Layer_UI()
{
	m_pUIManager->Initialize();

	return S_OK;
}

HRESULT CLevel_MoriblinCave::Ready_Lights()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	LIGHTDESC LightDesc;

	/* For.Directional*/
	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));
	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(.4f, .4f, .4f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;

	/* Add More Lights Here .. 
	>> 
	*/

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_MoriblinCave::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

#pragma region Player_Camera
	CCamera_Player::CAMERADESC_DERIVED PlayerCamera;
	ZeroMemory(&PlayerCamera, sizeof(CCamera_Player::CAMERADESC_DERIVED));

	PlayerCamera.CameraDesc.vEye = _float4(0.f, 10.0f, -10.f, 1.f);
	PlayerCamera.CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);

	PlayerCamera.CameraDesc.fFovy = XMConvertToRadians(60.0f);
	PlayerCamera.CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	PlayerCamera.CameraDesc.fNear = 0.2f;
	PlayerCamera.CameraDesc.fFar = 500.f;

	PlayerCamera.CameraDesc.TransformDesc.fSpeedPerSec = 10.f;
	PlayerCamera.CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Camera_Player"), TEXT("Prototype_GameObject_Camera_Player"), LEVEL_MORIBLINCAVE, pLayerTag, &PlayerCamera)))
		return E_FAIL;
#pragma endregion Player_Camera

#pragma region Dungeon_Camera
	CCamera_Dungeon::CAMERADESC_DERIVED DungeonCamera;
	ZeroMemory(&DungeonCamera, sizeof(CCamera_Dungeon::CAMERADESC_DERIVED));
	
	DungeonCamera.CameraDesc.vEye = _float4(0.f, 10.0f, -10.f, 1.f);
	DungeonCamera.CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);

	DungeonCamera.CameraDesc.fFovy = XMConvertToRadians(60.0f);
	DungeonCamera.CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	DungeonCamera.CameraDesc.fNear = 0.2f;
	DungeonCamera.CameraDesc.fFar = 500.f;

	DungeonCamera.CameraDesc.TransformDesc.fSpeedPerSec = 10.f;
	DungeonCamera.CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Camera_Dungeon"), TEXT("Prototype_GameObject_Camera_Dungeon"), LEVEL_MORIBLINCAVE, pLayerTag, &DungeonCamera)))
		return E_FAIL;
#pragma endregion Dungeon_Camera

	CCameraManager::Get_Instance()->Ready_Camera(LEVEL::LEVEL_MORIBLINCAVE, CCameraManager::CAM_STATE::CAM_DUNGEON);
	CCamera_Dungeon* pCameraDungeon = dynamic_cast<CCamera_Dungeon*>(CGameInstance::Get_Instance()->Find_Object(LEVEL_MORIBLINCAVE, TEXT("Layer_Camera"), CCameraManager::CAM_DUNGEON));
	if (!pCameraDungeon)
		return E_FAIL;

	pCameraDungeon->Add_RoomCamera(_float3(0.f, 0.f, 0.f));		// MoriblinCave_1A
	pCameraDungeon->Add_RoomCamera(_float3(6.f, 0.f, 0.f));		// MoriblinCave_1B
	pCameraDungeon->Add_RoomCamera(_float3(12.f, 0.f, 0.f));	// MoriblinCave_1C
	pCameraDungeon->Add_RoomCamera(_float3(0.f, 0.f, -5.f));	// MoriblinCave_2A
	pCameraDungeon->Setup_DungeonCamera();

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_MoriblinCave::Ready_Layer_Effect(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	// ..

	Safe_Release(pGameInstance);

	return S_OK;
}

CLevel_MoriblinCave* CLevel_MoriblinCave::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_MoriblinCave* pInstance = new CLevel_MoriblinCave(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		ERR_MSG(TEXT("Failed to Create: CLevel_MoriblinCave"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_MoriblinCave::Free()
{
	__super::Free();

	Safe_Release(m_pUIManager);
}