#include "stdafx.h"

#include "Level_MoriblinCave.h"
#include "GameInstance.h"
#include "CameraManager.h"
#include "Camera_Player.h"
#include "Camera_Dungeon.h"
#include "TriggerBox_Dynamic.h"
#include "UI_Manager.h"
#include "Sky.h"
#include "Layer.h"
#include "Bossblin.h"
#include "Player.h"
#include "DungeonDoor.h"

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
	if (FAILED(__super::Add_NavigationMesh(TEXT("Com_Navigation"), LEVEL_MORIBLINCAVE, TEXT("Prototype_Component_Navigation"), (CComponent**)&m_pNavigationMesh)))
		return E_FAIL;

	CGameInstance::Get_Instance()->PlayBGM(TEXT("Moriblin_Cave.mp3"), 0.5f);
	CGameInstance::Get_Instance()->Set_CurrentBGM(TEXT("Moriblin_Cave.mp3"));

	return S_OK;
}

void CLevel_MoriblinCave::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Compute_CurrentRoom();
	Check_Doors(fTimeDelta);
}

void CLevel_MoriblinCave::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	SetWindowText(g_hWnd, TEXT("MoriblinCave Level."));
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
		else if (!wcscmp(m_vInstancedObjects[i].wcObjName, TEXT("DungeonDoor")))
			pGameInstance->Add_GameObject(m_vInstancedObjects[i].wcObjName, TEXT("Prototype_GameObject_DungeonDoor"), LEVEL_MORIBLINCAVE, m_vInstancedObjects[i].wcLayerTag, &m_vInstancedObjects[i]);
		else if (!wcscmp(m_vInstancedObjects[i].wcObjName, TEXT("Treasure")))
			pGameInstance->Add_GameObject(m_vInstancedObjects[i].wcObjName, TEXT("Prototype_GameObject_Treasure"), LEVEL_MORIBLINCAVE, m_vInstancedObjects[i].wcLayerTag, &m_vInstancedObjects[i]);
	}
	CloseHandle(hFile);

	Safe_Release(pGameInstance);

	Add_RoomPositions();
	Compute_RoomsData();

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

HRESULT CLevel_MoriblinCave::Ready_Sky()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CSky::SKYDESC tSkyDesc;
	tSkyDesc.m_eLevelId = CSky::SKYBOX::SKY_MORIBLINCAVE;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Sky"), TEXT("Prototype_GameObject_Sky"), LEVEL_MORIBLINCAVE, TEXT("Layer_Sky"), &tSkyDesc)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_MoriblinCave::Ready_Layer_UI()
{
	m_pUIManager->Initialize();
	m_pUIManager->ScreenFadeIn();

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
	LightDesc.vDiffuse = _float4(.8f, .8f, .8f, 1.f);
	LightDesc.vAmbient = _float4(.2f, .2f, .2f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;

	/* For.Point */

	/* First Room */
	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));
	LightDesc.eType = LIGHTDESC::TYPE_POINT;
	LightDesc.vPosition = _float4(-.75f, 1.f, -3.5f, 1.f);
	LightDesc.fRange = 2.f;
	LightDesc.vDiffuse = _float4(1.f, .6f, 0.f, 1.f);
	LightDesc.vAmbient = _float4(.3f, .3f, .3f, 1.f);
	LightDesc.vSpecular = LightDesc.vDiffuse;

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;

	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));
	LightDesc.eType = LIGHTDESC::TYPE_POINT;
	LightDesc.vPosition = _float4(.75f, 1.f, -3.5f, 1.f);
	LightDesc.fRange = 2.f;
	LightDesc.vDiffuse = _float4(1.f, .6f, 0.f, 1.f);
	LightDesc.vAmbient = _float4(.3f, .3f, .3f, 1.f);
	LightDesc.vSpecular = LightDesc.vDiffuse;

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;

	/* Second Room */
	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));
	LightDesc.eType = LIGHTDESC::TYPE_POINT;
	LightDesc.vPosition = _float4(-1.5f, 1.f, 1.5f, 1.f);
	LightDesc.fRange = 2.f;
	LightDesc.vDiffuse = _float4(1.f, .6f, 0.f, 1.f);
	LightDesc.vAmbient = _float4(.3f, .3f, .3f, 1.f);
	LightDesc.vSpecular = LightDesc.vDiffuse;

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;

	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));
	LightDesc.eType = LIGHTDESC::TYPE_POINT;
	LightDesc.vPosition = _float4(1.5f, 1.f, 1.5f, 1.f);
	LightDesc.fRange = 2.f;
	LightDesc.vDiffuse = _float4(1.f, .6f, 0.f, 1.f);
	LightDesc.vAmbient = _float4(.3f, .3f, .3f, 1.f);
	LightDesc.vSpecular = LightDesc.vDiffuse;

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;

	/* Third Room */
	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));
	LightDesc.eType = LIGHTDESC::TYPE_POINT;
	LightDesc.vPosition = _float4(4.5f, 1.f, 1.5f, 1.f);
	LightDesc.fRange = 2.f;
	LightDesc.vDiffuse = _float4(1.f, .6f, 0.f, 1.f);
	LightDesc.vAmbient = _float4(.3f, .3f, .3f, 1.f);
	LightDesc.vSpecular = LightDesc.vDiffuse;

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;

	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));
	LightDesc.eType = LIGHTDESC::TYPE_POINT;
	LightDesc.vPosition = _float4(7.5f, 1.f, 1.5f, 1.f);
	LightDesc.fRange = 2.f;
	LightDesc.vDiffuse = _float4(1.f, .6f, 0.f, 1.f);
	LightDesc.vAmbient = _float4(.3f, .3f, .3f, 1.f);
	LightDesc.vSpecular = LightDesc.vDiffuse;

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;

	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));
	LightDesc.eType = LIGHTDESC::TYPE_POINT;
	LightDesc.vPosition = _float4(4.5f, 1.f, -1.5f, 1.f);
	LightDesc.fRange = 2.f;
	LightDesc.vDiffuse = _float4(1.f, .6f, 0.f, 1.f);
	LightDesc.vAmbient = _float4(.3f, .3f, .3f, 1.f);
	LightDesc.vSpecular = LightDesc.vDiffuse;

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;

	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));
	LightDesc.eType = LIGHTDESC::TYPE_POINT;
	LightDesc.vPosition = _float4(7.5f, 1.f, -1.5f, 1.f);
	LightDesc.fRange = 2.f;
	LightDesc.vDiffuse = _float4(1.f, .6f, 0.f, 1.f);
	LightDesc.vAmbient = _float4(.3f, .3f, .3f, 1.f);
	LightDesc.vSpecular = LightDesc.vDiffuse;

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;

	/* Treasure Room */
	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));
	LightDesc.eType = LIGHTDESC::TYPE_POINT;
	LightDesc.vPosition = _float4(10.5f, 1.f, .5f, 1.f);
	LightDesc.fRange = 2.f;
	LightDesc.vDiffuse = _float4(1.f, .6f, 0.f, 1.f);
	LightDesc.vAmbient = _float4(.3f, .3f, .3f, 1.f);
	LightDesc.vSpecular = LightDesc.vDiffuse;

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;

	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));
	LightDesc.eType = LIGHTDESC::TYPE_POINT;
	LightDesc.vPosition = _float4(10.5f, 1.f, -.5f, 1.f);
	LightDesc.fRange = 2.f;
	LightDesc.vDiffuse = _float4(1.f, .6f, 0.f, 1.f);
	LightDesc.vAmbient = _float4(.3f, .3f, .3f, 1.f);
	LightDesc.vSpecular = LightDesc.vDiffuse;

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
	CCamera_Dungeon* pCameraDungeon = dynamic_cast<CCamera_Dungeon*>(CGameInstance::Get_Instance()->Find_Object(LEVEL_MORIBLINCAVE, TEXT("Layer_Camera")));
	if (!pCameraDungeon)
		return E_FAIL;

	for (auto& tDungeonRoom : m_DungeonRoomDescs)
	{
		_float3 vRoomPosition = _float3(tDungeonRoom->m_vRoomPosition.x, tDungeonRoom->m_vRoomPosition.y, tDungeonRoom->m_vRoomPosition.z);
		pCameraDungeon->Add_RoomPosition(vRoomPosition);
	}

	Compute_CurrentRoom();
	pCameraDungeon->Setup_DungeonCamera(_float3(m_pCurrentDungeonRoom->m_vRoomPosition.x, m_pCurrentDungeonRoom->m_vRoomPosition.y, m_pCurrentDungeonRoom->m_vRoomPosition.z));

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

void CLevel_MoriblinCave::Add_RoomPositions()
{
	DUNGEONROOMDESC* tDungeonRoomDesc = new DUNGEONROOMDESC;
	tDungeonRoomDesc->m_vRoomPosition = _float4(0.f, 0.f, 0.f, 1.f);	// MoriblinCave_1A
	m_DungeonRoomDescs.push_back(tDungeonRoomDesc);	

	tDungeonRoomDesc = new DUNGEONROOMDESC;
	tDungeonRoomDesc->m_vRoomPosition = _float4(6.f, 0.f, 0.f, 1.f);	// MoriblinCave_1B
	m_DungeonRoomDescs.push_back(tDungeonRoomDesc);

	tDungeonRoomDesc = new DUNGEONROOMDESC;
	tDungeonRoomDesc->m_vRoomPosition = _float4(12.f, 0.f, 0.f, 1.f);	// MoriblinCave_1C
	m_DungeonRoomDescs.push_back(tDungeonRoomDesc);

	tDungeonRoomDesc = new DUNGEONROOMDESC;
	tDungeonRoomDesc->m_vRoomPosition = _float4(0.f, 0.f, -5.f, 1.f);	// MoriblinCave_2A
	m_DungeonRoomDescs.push_back(tDungeonRoomDesc);
}

void CLevel_MoriblinCave::Compute_RoomsData()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/* Monsters Data */
	list<CGameObject*> Monsters = pGameInstance->Get_Layer(pGameInstance->Get_NextLevelIndex(), TEXT("Layer_Monster"))->Get_Objects();
	for (auto pObject : Monsters)
	{
		CMonster* pMonster = static_cast<CMonster*>(pObject);
	
		_uint iDungeonRoomIndex = 0;
		_float fPreviousDistance = 999.f;

		for (_uint i = 0; i < m_DungeonRoomDescs.size(); i++)
		{
			_vector vPosition = pMonster->Get_Transform()->Get_State(CTransform::STATE::STATE_TRANSLATION);
			_float fDistance = XMVectorGetX(XMVector4Length(XMLoadFloat4(&m_DungeonRoomDescs[i]->m_vRoomPosition) - vPosition));

			if (fDistance < fPreviousDistance)
			{
				iDungeonRoomIndex = i;
				fPreviousDistance = fDistance;
			}
		}
		
		m_DungeonRoomDescs[iDungeonRoomIndex]->m_RoomMonsters.push_back(pMonster);
	}

	/* Dungeon Doors Data */
	list<CGameObject*> Objects = pGameInstance->Get_Layer(pGameInstance->Get_NextLevelIndex(), TEXT("Layer_Objects"))->Get_Objects();
	for (auto pObject : Objects)
	{
		CDungeonDoor* pDungeonDoor = dynamic_cast<CDungeonDoor*>(pObject);
		if (pDungeonDoor)
			m_DungeonDoors.push_back(pDungeonDoor);
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CLevel_MoriblinCave::Compute_CurrentRoom()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	DUNGEONROOMDESC* pCurrentRoom = nullptr;
	_float fPreviousDistance = 999.f;

	for (auto& pDungeonRoom : m_DungeonRoomDescs)
	{
		CPlayer* pPlayer = (CPlayer*)pGameInstance->Find_Object(pGameInstance->Get_NextLevelIndex(), TEXT("Layer_Player"));
		_vector vPlayerPosition = pPlayer->Get_Transform()->Get_State(CTransform::STATE::STATE_TRANSLATION);

		_float fDistance = XMVectorGetX(XMVector3Length(XMLoadFloat4(&pDungeonRoom->m_vRoomPosition) - vPlayerPosition));

		if (fDistance < fPreviousDistance)
		{
			pCurrentRoom = pDungeonRoom;
			fPreviousDistance = fDistance;
		}
	}

	if (pCurrentRoom != m_pCurrentDungeonRoom && m_pCurrentDungeonRoom != nullptr)
		m_bIsRoomChanged = true;
	else
		m_bIsRoomChanged = false;

	m_pCurrentDungeonRoom = pCurrentRoom;

	RELEASE_INSTANCE(CGameInstance);
}

void CLevel_MoriblinCave::Remove_MonsterFromRoom(CMonster * pMonster)
{
	for (auto& iter = m_pCurrentDungeonRoom->m_RoomMonsters.begin(); iter != m_pCurrentDungeonRoom->m_RoomMonsters.end(); iter++)
	{
		if (pMonster == *iter)
		{
			m_pCurrentDungeonRoom->m_RoomMonsters.erase(iter);
			return;
		}
	}
}

void CLevel_MoriblinCave::Check_Doors(_float fTimeDelta)
{
	if (m_pCurrentDungeonRoom->m_RoomMonsters.empty() && !m_pCurrentDungeonRoom->m_bIsRoomClear)
	{
		for (auto& pDungeonDoor : m_DungeonDoors)
			pDungeonDoor->Open_Door();

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		pGameInstance->PlaySounds(TEXT("DungeonDoor_Open.wav"), SOUND_OBJECT, 1.f);
		RELEASE_INSTANCE(CGameInstance);

		m_pCurrentDungeonRoom->m_bIsRoomClear = true;
	}

	if (m_bShouldClose)
	{
		if (m_fCloseTimer > m_fCloseTimerMax)
		{
			for (auto& pDungeonDoor : m_DungeonDoors)
				pDungeonDoor->Close_Door();

			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			pGameInstance->PlaySounds(TEXT("DungeonDoor_Close.wav"), SOUND_OBJECT, 1.f);
			RELEASE_INSTANCE(CGameInstance);

			if (m_pCurrentDungeonRoom->m_vRoomPosition.x == 6.f)
			{
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				CPlayer* pPlayer = (CPlayer*)pGameInstance->Find_Object(LEVEL::LEVEL_MORIBLINCAVE, TEXT("Layer_Player"));
				if (pPlayer)
				{
					pPlayer->Set_Monster(m_pCurrentDungeonRoom->m_RoomMonsters.front());
					((CBossblin*)m_pCurrentDungeonRoom->m_RoomMonsters.front())->Show_Chat();
				}

				RELEASE_INSTANCE(CGameInstance);
			}

			m_fCloseTimer = 0.f;
			m_bShouldClose = false;
		}
		else
			m_fCloseTimer += fTimeDelta;
	}
}

void CLevel_MoriblinCave::Close_Door()
{
	if (!m_pCurrentDungeonRoom->m_RoomMonsters.empty())
		m_bShouldClose = true;
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

	CGameInstance::Get_Instance()->StopAll();

	Safe_Release(m_pUIManager);

	for (auto& pDungeonRoom : m_DungeonRoomDescs)
		Safe_Delete(pDungeonRoom);

	m_DungeonRoomDescs.clear();
}