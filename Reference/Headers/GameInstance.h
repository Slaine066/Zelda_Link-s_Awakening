#pragma once

#include "Graphic_Device.h"
#include "Input_Device.h"
#include "Level_Manager.h"
#include "Object_Manager.h"
#include "Timer_Manager.h"
#include "PipeLine.h"
#include "Component_Manager.h"
#include "Light_Manager.h"
#include "Font_Manager.h"
#include "Picking.h"
#include "CollisionManager.h"
#include "KeysManager.h"
#include "FrustumCulling.h"

BEGIN(Engine)
class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)

private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public: /* For.Engine */
	HRESULT Initialize_Engine(HINSTANCE hInst, _uint iNumLevels, const GRAPHIC_DESC& GraphicDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext);
	void Tick_Engine(_float fTimeDelta);
	void Clear(_uint iLevelIndex);

public: /* For.Graphic_Device */	
	ID3D11RenderTargetView* Get_BackBufferRTV();
	ID3D11DepthStencilView* Get_DepthStencilView();
	HRESULT Clear_BackBuffer_View(_float4 vClearColor);
	HRESULT Clear_DepthStencil_View();
	HRESULT Present();

public: /* For.Input_Device */
	_char Get_DIKState(_uchar eKeyID);
	_char Get_DIMKeyState(DIMK eMouseKeyID);
	_long Get_DIMMoveState(DIMM eMouseMoveID);

public: /* For.Timer_Manager */
	HRESULT Add_Timer(const _tchar* pTimerTag);
	_float Get_TimeDelta(const _tchar* pTimerTag);
	void Update_TimeDelta(const _tchar* pTimerTag);

public: /* For.Level_Manager */
	HRESULT Open_Level(_uint iLevelIndex, class CLevel* pNewLevel);
	_uint Get_CurrentLevelIndex();
	_uint Get_NextLevelIndex();
	void Set_NextLevel(_uint iNextLevelIndex);
	HRESULT Render_NavigationMesh();

public: /* For.Object_Manager */
	HRESULT Add_Prototype(const _tchar* pPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_GameObject(const _tchar* pObjName, const _tchar* pPrototypeTag, _uint iLevelIndex, const _tchar* pLayerTag, void* pArg = nullptr);
	class CGameObject* Clone_GameObject(const _tchar* pPrototypeTag, void* pArg = nullptr);
	class CGameObject* Find_Object(_uint iLevelIndex, const _tchar * pLayerTag, _uint iIndex = 0);
	class CComponent* Get_Component(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pComponentTag, _uint iIndex = 0);
	CObject_Manager::LAYERS Get_Layers(_uint iLevelIndex);
	void Delete_GameObject(CGameObject* pGameObj, _uint iLevelIndex, const _tchar* pLayerTag);

public: /* For.Component_Manager */
	HRESULT Add_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, void* pArg = nullptr);
	class CComponent* Find_Component_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag);

public: /* For. Collider Manager */
	HRESULT Add_CollisionGroup(CCollision_Manager::COLLISION_GROUP eCollisionGroup, class CGameObject* pGameObject);
	void Out_CollisionGroup(CCollision_Manager::COLLISION_GROUP eCollisionGroup, class CGameObject* pGameObject);
	_bool Collision_with_Group(CCollision_Manager::COLLISION_GROUP eGroup, class CCollider* pDamageCauser, CCollider::AIM eCollisionAim, OUT CGameObject& pDamagedObject);
	_bool Collision_Check_Group_Multi(CCollision_Manager::COLLISION_GROUP eGroup, class CCollider* pDamageCauser, CCollider::AIM eCollisionAim, OUT vector<CGameObject*>& pDamagedObjects);

	// Damage Functions
	static void Apply_Damage(_float fDamage, CGameObject* DamagedObj, CGameObject * DamageCauser, void* AttackType = nullptr);
	static void	Apply_Damage_Multi(_float fDamage, vector<CGameObject*>& vecDamagedObj, CGameObject * DamageCauser, void* AttackType = nullptr);

public: /* For.PipeLine */
	void Set_Transform(CPipeLine::TRANSFORMSTATE eState, _fmatrix TransformMatrix);
	_matrix Get_TransformMatrix(CPipeLine::TRANSFORMSTATE  eState);
	_float4x4 Get_TransformFloat4x4(CPipeLine::TRANSFORMSTATE  eState);
	_float4x4 Get_TransformFloat4x4_TP(CPipeLine::TRANSFORMSTATE  eState);
	_float4 Get_CamPosition();

public: /* For.Light_Manager */
	const LIGHTDESC* Get_LightDesc(_uint iIndex);
	HRESULT Add_Light(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHTDESC& LightDesc);

public: /* For.Font_Manager */
	HRESULT Add_Fonts(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFontTag, const _tchar* pFontFilePath);
	HRESULT Render_Font(const _tchar* pFontTag, const _tchar* pText, _fvector vPos, _fvector vColor);

public: /* For.KeysManager */
	_bool Key_Down(int _Key);
	_bool Key_Pressing(int _Key);
	_bool Key_Up(int _Key);

public: /* For.FrustumCulling */
	_bool IsIn_Frustum(_fvector vPosition, _float fRange = 0.f);
	
public:
	static void Release_Engine();

private:
	CGraphic_Device*	m_pGraphic_Device = nullptr;
	CInput_Device*		m_pInput_Device = nullptr;
	CLevel_Manager*		m_pLevel_Manager = nullptr;
	CObject_Manager*	m_pObject_Manager = nullptr;
	CTimer_Manager*		m_pTimer_Manager = nullptr;
	CComponent_Manager*	m_pComponent_Manager = nullptr;
	CPipeLine*			m_pPipeLine = nullptr;
	CLight_Manager*		m_pLight_Manager = nullptr;
	CFont_Manager*		m_pFont_Manager = nullptr;
	CPicking*			m_pPicking = nullptr;
	CCollision_Manager*	m_pCollision_Manager = nullptr;
	CKeysManager*		m_pKeys_Manager = nullptr;
	CFrustumCulling*	m_pFrustumCulling = nullptr;

public:
	virtual void Free() override;
};
END