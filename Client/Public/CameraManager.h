#pragma once

#include "Camera_Player.h"

BEGIN(Client)
class CCameraManager final : public CBase
{
	DECLARE_SINGLETON(CCameraManager)

public:
	enum CAM_STATE { CAM_PLAYER, CAM_DUNGEON, CAM_END };

public:
	CCameraManager();
	virtual ~CCameraManager() = default;

public:
	CCamera* Get_CurrentCamera() { return m_pCurrentCamera; }
	CAM_STATE Get_CamState() { return m_eCamState; }

public:
	void Set_CurrentCamera(CCamera* _pCamera) { m_pCurrentCamera = _pCamera; }
	void Set_CamState(CAM_STATE _eState);

public:
	HRESULT Ready_Camera(LEVEL eLevelIndex, CAM_STATE eCamState);

private:
	CCamera* m_pCurrentCamera = nullptr;
	LEVEL m_eCurrentLevel;
	CAM_STATE m_eCamState = CAM_END;
	
public:
	virtual void Free() override;
};
END