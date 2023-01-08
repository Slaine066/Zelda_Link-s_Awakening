#include "stdafx.h"

#include "Level_Field.h"
#include "GameInstance.h"
#include "CameraManager.h"
#include "Camera_Player.h"
#include "TriggerBox_Dynamic.h"
#include "UI_Manager.h"
#include "Sky.h"

CLevel_Field::CLevel_Field(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext),
	m_pUIManager(CUI_Manager::Get_Instance())
{
	Safe_AddRef(m_pUIManager);
}

HRESULT CLevel_Field::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Load_Objects_FromFile()))
		return E_FAIL;

	if (FAILED(Load_Triggers_FromFile()))
		return E_FAIL;

	if (FAILED(Ready_Sky()))
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
	if (FAILED(__super::Add_NavigationMesh(TEXT("Com_Navigation"), LEVEL_FIELD, TEXT("Prototype_Component_Navigation"), (CComponent**)&m_pNavigationMesh)))
		return E_FAIL;

	CGameInstance::Get_Instance()->PlayBGM(TEXT("Mabe_Village.mp3"), 0.5f);
	CGameInstance::Get_Instance()->Set_CurrentBGM(TEXT("Mabe_Village.mp3"));

	return S_OK;
}

void CLevel_Field::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);	
}

void CLevel_Field::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	SetWindowText(g_hWnd, TEXT("Field Level."));
}

HRESULT CLevel_Field::Load_Objects_FromFile()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	HANDLE hFile = CreateFile(TEXT("../../Data/MapData/Field.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == 0)
		return E_FAIL;

	// First read to get Objects count, so we can resize the vector.
	_ulong dwByte = 0;
 	_uint iCounter = 0;

	CActor::MODELDESC tModelDesc;
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
	hFile = CreateFile(TEXT("../../Data/MapData/Field.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == 0)
		return E_FAIL;

	for (_uint i = 0; i < iCounter; i++)
	{
		ReadFile(hFile, &m_vInstancedObjects[i], sizeof(CActor::MODELDESC), nullptr, nullptr);

		if (wcsstr(m_vInstancedObjects[i].wcObjName, TEXT("Field")))
			pGameInstance->Add_GameObject(m_vInstancedObjects[i].wcObjName, TEXT("Prototype_GameObject_StaticObject"), LEVEL_FIELD, m_vInstancedObjects[i].wcLayerTag, &m_vInstancedObjects[i]);
		else if (!wcscmp(m_vInstancedObjects[i].wcObjName, TEXT("Link")))
			pGameInstance->Add_GameObject(m_vInstancedObjects[i].wcObjName, TEXT("Prototype_GameObject_Player"), LEVEL_FIELD, m_vInstancedObjects[i].wcLayerTag, &m_vInstancedObjects[i]);
		else if (!wcscmp(m_vInstancedObjects[i].wcObjName, TEXT("MoriblinSword")))
			pGameInstance->Add_GameObject(m_vInstancedObjects[i].wcObjName, TEXT("Prototype_GameObject_MoriblinSword"), LEVEL_FIELD, m_vInstancedObjects[i].wcLayerTag, &m_vInstancedObjects[i]);
		else if (!wcscmp(m_vInstancedObjects[i].wcObjName, TEXT("MoriblinSpear")))
			pGameInstance->Add_GameObject(m_vInstancedObjects[i].wcObjName, TEXT("Prototype_GameObject_MoriblinSpear"), LEVEL_FIELD, m_vInstancedObjects[i].wcLayerTag, &m_vInstancedObjects[i]);
		else if (!wcscmp(m_vInstancedObjects[i].wcObjName, TEXT("Treasure")))
			pGameInstance->Add_GameObject(m_vInstancedObjects[i].wcObjName, TEXT("Prototype_GameObject_Treasure"), LEVEL_FIELD, m_vInstancedObjects[i].wcLayerTag, &m_vInstancedObjects[i]);
		else if (!wcscmp(m_vInstancedObjects[i].wcObjName, TEXT("Sword")))
			pGameInstance->Add_GameObject(m_vInstancedObjects[i].wcObjName, TEXT("Prototype_GameObject_Sword"), LEVEL_FIELD, m_vInstancedObjects[i].wcLayerTag, &m_vInstancedObjects[i]);
		else if (!wcscmp(m_vInstancedObjects[i].wcObjName, TEXT("Owl")))
			pGameInstance->Add_GameObject(m_vInstancedObjects[i].wcObjName, TEXT("Prototype_GameObject_Owl"), LEVEL_FIELD, m_vInstancedObjects[i].wcLayerTag, &m_vInstancedObjects[i]);
	}
	CloseHandle(hFile);

	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CLevel_Field::Load_Triggers_FromFile()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	HANDLE hFile = CreateFile(TEXT("../../Data/TriggerData/Field.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
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

HRESULT CLevel_Field::Ready_Sky()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CSky::SKYDESC tSkyDesc;
	tSkyDesc.m_eLevelId = CSky::SKYBOX::SKY_FIELD;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Sky"), TEXT("Prototype_GameObject_Sky"), LEVEL_FIELD, TEXT("Layer_Sky"), &tSkyDesc)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Field::Ready_Layer_UI()
{
	m_pUIManager->Initialize();
	m_pUIManager->ScreenFadeIn();
	m_pUIManager->LevelNameFadeIn(LEVEL::LEVEL_FIELD);

	return S_OK;
}

HRESULT CLevel_Field::Ready_Lights()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	LIGHTDESC LightDesc;

	/* For.Directional*/
	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));
	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(.35f, .35f, .35f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;

	/* Add More Lights Here ..
	>>
	*/

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Field::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CCamera_Player::CAMERADESC_DERIVED CameraDesc;
	ZeroMemory(&CameraDesc, sizeof(CCamera_Player::CAMERADESC_DERIVED));

	CameraDesc.CameraDesc.vEye = _float4(0.f, 10.0f, -10.f, 1.f);
	CameraDesc.CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);

	CameraDesc.CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.CameraDesc.fNear = 0.2f;
	CameraDesc.CameraDesc.fFar = 500.f;

	CameraDesc.CameraDesc.TransformDesc.fSpeedPerSec = 10.f;
	CameraDesc.CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Camera_Player"), TEXT("Prototype_GameObject_Camera_Player"), LEVEL_FIELD, pLayerTag, &CameraDesc)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	CCameraManager::Get_Instance()->Ready_Camera(LEVEL::LEVEL_FIELD, CCameraManager::CAM_STATE::CAM_PLAYER);

	return S_OK;
}

HRESULT CLevel_Field::Ready_Layer_Effect(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	// ..

	Safe_Release(pGameInstance);
	
	return S_OK;
}

CLevel_Field* CLevel_Field::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Field* pInstance = new CLevel_Field(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		ERR_MSG(TEXT("Failed to Create: CLevel_Field"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Field::Free()
{
	__super::Free();

	CGameInstance::Get_Instance()->StopAll();

	Safe_Release(m_pUIManager);
}