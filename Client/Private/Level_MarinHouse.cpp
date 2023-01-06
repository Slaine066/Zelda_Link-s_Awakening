#include "stdafx.h"

#include "Level_MarinHouse.h"
#include "GameInstance.h"
#include "CameraManager.h"
#include "Camera_Static.h"
#include "TriggerBox_Dynamic.h"
#include "UI_Manager.h"
#include "Sky.h"

CLevel_MarinHouse::CLevel_MarinHouse(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext),
	m_pUIManager(CUI_Manager::Get_Instance())
{
	Safe_AddRef(m_pUIManager);
}

HRESULT CLevel_MarinHouse::Initialize()
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
	if (FAILED(__super::Add_NavigationMesh(TEXT("Com_Navigation"), LEVEL_MARINHOUSE, TEXT("Prototype_Component_Navigation"), (CComponent**)&m_pNavigationMesh)))
		return E_FAIL;

	return S_OK;
}

void CLevel_MarinHouse::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CLevel_MarinHouse::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	SetWindowText(g_hWnd, TEXT("MarinHouse Level."));
}

HRESULT CLevel_MarinHouse::Load_Objects_FromFile()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	HANDLE hFile = CreateFile(TEXT("../../Data/MapData/MarinHouse.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
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
	hFile = CreateFile(TEXT("../../Data/MapData/MarinHouse.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == 0)
		return E_FAIL;

	for (_uint i = 0; i < iCounter; i++)
	{
		ReadFile(hFile, &m_vInstancedObjects[i], sizeof(CActor::MODELDESC), nullptr, nullptr);

		if (wcsstr(m_vInstancedObjects[i].wcObjName, TEXT("MarinHouse")))
			pGameInstance->Add_GameObject(m_vInstancedObjects[i].wcObjName, TEXT("Prototype_GameObject_StaticObject"), LEVEL_MARINHOUSE, m_vInstancedObjects[i].wcLayerTag, &m_vInstancedObjects[i]);
		else if (!wcscmp(m_vInstancedObjects[i].wcObjName, TEXT("Link")))
			pGameInstance->Add_GameObject(m_vInstancedObjects[i].wcObjName, TEXT("Prototype_GameObject_Player"), LEVEL_MARINHOUSE, m_vInstancedObjects[i].wcLayerTag, &m_vInstancedObjects[i]);
		else if (!wcscmp(m_vInstancedObjects[i].wcObjName, TEXT("Bed")))
			pGameInstance->Add_GameObject(m_vInstancedObjects[i].wcObjName, TEXT("Prototype_GameObject_Bed"), LEVEL_MARINHOUSE, m_vInstancedObjects[i].wcLayerTag, &m_vInstancedObjects[i]);
		else if (!wcscmp(m_vInstancedObjects[i].wcObjName, TEXT("Tarin")))
			pGameInstance->Add_GameObject(m_vInstancedObjects[i].wcObjName, TEXT("Prototype_GameObject_Tarin"), LEVEL_MARINHOUSE, m_vInstancedObjects[i].wcLayerTag, &m_vInstancedObjects[i]);
		else if (!wcscmp(m_vInstancedObjects[i].wcObjName, TEXT("Marin")))
			pGameInstance->Add_GameObject(m_vInstancedObjects[i].wcObjName, TEXT("Prototype_GameObject_Marin"), LEVEL_MARINHOUSE, m_vInstancedObjects[i].wcLayerTag, &m_vInstancedObjects[i]);
	}
	CloseHandle(hFile);

	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CLevel_MarinHouse::Load_Triggers_FromFile()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	HANDLE hFile = CreateFile(TEXT("../../Data/TriggerData/MarinHouse.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
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

HRESULT CLevel_MarinHouse::Ready_Sky()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CSky::SKYDESC tSkyDesc;
	tSkyDesc.m_eLevelId = CSky::SKYBOX::SKY_MARINHOUSE;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Sky"), TEXT("Prototype_GameObject_Sky"), LEVEL_MARINHOUSE, TEXT("Layer_Sky"), &tSkyDesc)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_MarinHouse::Ready_Layer_UI()
{
	m_pUIManager->Initialize();
	m_pUIManager->ScreenFadeIn();

	return S_OK;
}

HRESULT CLevel_MarinHouse::Ready_Lights()
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

HRESULT CLevel_MarinHouse::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CCamera_Static::CAMERADESC_DERIVED CameraDesc;
	ZeroMemory(&CameraDesc, sizeof(CCamera_Static::CAMERADESC_DERIVED));

	CameraDesc.CameraDesc.vEye = _float4(0.f, 10.0f, -10.f, 1.f);
	CameraDesc.CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);

	CameraDesc.CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.CameraDesc.fNear = 0.2f;
	CameraDesc.CameraDesc.fFar = 500.f;

	CameraDesc.CameraDesc.TransformDesc.fSpeedPerSec = 10.f;
	CameraDesc.CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Camera_Static"), TEXT("Prototype_GameObject_Camera_Static"), LEVEL_MARINHOUSE, pLayerTag, &CameraDesc)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	CCameraManager::Get_Instance()->Ready_Camera(LEVEL::LEVEL_MARINHOUSE, CCameraManager::CAM_STATE::CAM_STATIC);
	CCamera_Static* pCameraStatic = dynamic_cast<CCamera_Static*>(CGameInstance::Get_Instance()->Find_Object(LEVEL_MARINHOUSE, TEXT("Layer_Camera")));
	if (!pCameraStatic)
		return E_FAIL;

	pCameraStatic->Set_LookPosition(m_vLookPosition);

	return S_OK;
}

HRESULT CLevel_MarinHouse::Ready_Layer_Effect(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	// ..

	Safe_Release(pGameInstance);

	return S_OK;
}

CLevel_MarinHouse* CLevel_MarinHouse::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_MarinHouse* pInstance = new CLevel_MarinHouse(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		ERR_MSG(TEXT("Failed to Create: CLevel_MarinHouse"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_MarinHouse::Free()
{
	__super::Free();

	Safe_Release(m_pUIManager);
}