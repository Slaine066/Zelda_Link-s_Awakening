#include "stdafx.h"

#include "Level_GamePlay.h"
#include "GameInstance.h"
#include "Camera_Dynamic.h"

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Load_From_File()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	/* For.Com_Navigation */
	if (FAILED(__super::Add_NavigationMesh(TEXT("Com_Navigation"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation_Field"), (CComponent**)&m_pNavigationMesh)))
		return E_FAIL;

	/*if (FAILED(Ready_Layer_UI()))
		return E_FAIL;*/

	/*if (FAILED(Ready_Layer_Effect()))
		return E_FAIL;*/

	return S_OK;
}

void CLevel_GamePlay::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);	
}

void CLevel_GamePlay::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	SetWindowText(g_hWnd, TEXT("Gameplay Level."));
}

HRESULT CLevel_GamePlay::Load_From_File()
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
	hFile = CreateFile(TEXT("../../Data/MapData/Field.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == 0)
		return E_FAIL;

	for (_uint i = 0; i < iCounter; i++)
	{
		ReadFile(hFile, &m_vInstancedObjects[i], sizeof(CActor::MODELDESC), nullptr, nullptr);

		if (wcsstr(m_vInstancedObjects[i].wcObjName, TEXT("Field")))
			pGameInstance->Add_GameObject(m_vInstancedObjects[i].wcObjName, TEXT("Prototype_GameObject_StaticObject"), LEVEL_GAMEPLAY, m_vInstancedObjects[i].wcLayerTag, &m_vInstancedObjects[i]);
		else if (!wcscmp(m_vInstancedObjects[i].wcObjName, TEXT("Link")))
			pGameInstance->Add_GameObject(m_vInstancedObjects[i].wcObjName, TEXT("Prototype_GameObject_Player"), LEVEL_GAMEPLAY, m_vInstancedObjects[i].wcLayerTag, &m_vInstancedObjects[i]);
		else if (!wcscmp(m_vInstancedObjects[i].wcObjName, TEXT("MoriblinSword")))
			pGameInstance->Add_GameObject(m_vInstancedObjects[i].wcObjName, TEXT("Prototype_GameObject_MoriblinSword"), LEVEL_GAMEPLAY, m_vInstancedObjects[i].wcLayerTag, &m_vInstancedObjects[i]);
		else if (!wcscmp(m_vInstancedObjects[i].wcObjName, TEXT("MoriblinSpear")))
			pGameInstance->Add_GameObject(m_vInstancedObjects[i].wcObjName, TEXT("Prototype_GameObject_MoriblinSpear"), LEVEL_GAMEPLAY, m_vInstancedObjects[i].wcLayerTag, &m_vInstancedObjects[i]);
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	// ..

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Lights()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	LIGHTDESC LightDesc;

	/* For.Directional*/
	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.3f, 0.3f, 0.3f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);	

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;

	///* For.Point */
	//ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

	//LightDesc.eType = LIGHTDESC::TYPE_POINT;
	//LightDesc.vPosition = _float4(10.f, 3.f, 10.f, 1.f);
	//LightDesc.fRange = 7.f;	
	//LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vAmbient = _float4(0.3f, 0.3f, 0.3f, 1.f);
	//LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	//if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
	//	return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CCamera_Dynamic::CAMERADESC_DERIVED CameraDesc;
	ZeroMemory(&CameraDesc, sizeof(CCamera_Dynamic::CAMERADESC_DERIVED));

	CameraDesc.iTest = 10;

	CameraDesc.CameraDesc.vEye = _float4(0.f, 10.0f, -10.f, 1.f);
	CameraDesc.CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);

	CameraDesc.CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.CameraDesc.fNear = 0.2f;
	CameraDesc.CameraDesc.fFar = 500.f;

	CameraDesc.CameraDesc.TransformDesc.fSpeedPerSec = 10.f;
	CameraDesc.CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Camera_Dynamic"), TEXT("Prototype_GameObject_Camera_Dynamic"), LEVEL_GAMEPLAY, pLayerTag, &CameraDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Effect(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	// ..

	Safe_Release(pGameInstance);
	
	return S_OK;
}

//HRESULT CLevel_GamePlay::Ready_NavigationMesh()
//{
//	HANDLE hFile = CreateFile(TEXT("../../Data/NavigationData/Field.dat"), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
//	if (!hFile)
//		return E_FAIL;
//
//	_float3	vPoints[3];
//	_ulong dwByte = 0;
//
//	/* Create Cells. */
//	ZeroMemory(vPoints, sizeof(_float3) * 3);
//	vPoints[0] = _float3(0.f, 0.f, 5.f);
//	vPoints[1] = _float3(5.f, 0.f, 0.f);
//	vPoints[2] = _float3(0.f, 0.f, 0.f);
//	WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
//
//	ZeroMemory(vPoints, sizeof(_float3) * 3);
//	vPoints[0] = _float3(0.f, 0.f, 5.f);
//	vPoints[1] = _float3(5.f, 0.f, 5.f);
//	vPoints[2] = _float3(5.f, 0.f, 0.f);
//	WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
//
//	ZeroMemory(vPoints, sizeof(_float3) * 3);
//	vPoints[0] = _float3(0.f, 0.f, 10.f);
//	vPoints[1] = _float3(5.f, 0.f, 5.f);
//	vPoints[2] = _float3(0.f, 0.f, 5.f);
//	WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
//
//	ZeroMemory(vPoints, sizeof(_float3) * 3);
//	vPoints[0] = _float3(5.f, 0.f, 5.f);
//	vPoints[1] = _float3(10.f, 0.f, 0.f);
//	vPoints[2] = _float3(5.f, 0.f, 0.f);
//	WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
//
//	CloseHandle(hFile);
//
//	return S_OK;
//}

CLevel_GamePlay* CLevel_GamePlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_GamePlay* pInstance = new CLevel_GamePlay(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		ERR_MSG(TEXT("Failed to Created : CLevel_GamePlay"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GamePlay::Free()
{
	__super::Free();
}