#include "stdafx.h"

#include "CameraManager.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CCameraManager);

CCameraManager::CCameraManager()
{
}

void CCameraManager::Set_CamState(CAM_STATE eCamState)
{
	CCamera* pCamera = dynamic_cast<CCamera*>(CGameInstance::Get_Instance()->Find_Object(m_eCurrentLevel, TEXT("Layer_Camera"), eCamState));
	if (nullptr == pCamera)
		return;

	m_pCurrentCamera = pCamera;
	m_eCamState = eCamState;
}

HRESULT CCameraManager::Ready_Camera(LEVEL eLevelIndex, CAM_STATE eCamState)
{
	CCamera* pCamera = dynamic_cast<CCamera*>(CGameInstance::Get_Instance()->Find_Object(eLevelIndex, TEXT("Layer_Camera")));
	if (!pCamera)
		return E_FAIL;

	m_pCurrentCamera = pCamera;
	m_eCurrentLevel = eLevelIndex;
	m_eCamState = eCamState;
	
	return S_OK;
}

void CCameraManager::Free()
{

}