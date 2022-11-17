#include "GameInstance.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
	: m_pGraphic_Device(CGraphic_Device::Get_Instance())
	, m_pInput_Device(CInput_Device::Get_Instance())
	, m_pLevel_Manager(CLevel_Manager::Get_Instance())
	, m_pObject_Manager(CObject_Manager::Get_Instance())
	, m_pTimer_Manager(CTimer_Manager::Get_Instance())
	, m_pComponent_Manager(CComponent_Manager::Get_Instance())	
	, m_pPipeLine(CPipeLine::Get_Instance())
	, m_pLight_Manager(CLight_Manager::Get_Instance())
	, m_pFont_Manager(CFont_Manager::Get_Instance())
	, m_pPicking(CPicking::Get_Instance())
	, m_pCollision_Manager(CCollision_Manager::Get_Instance())
	, m_pKeys_Manager(CKeysManager::Get_Instance())
	, m_pFrustumCulling(CFrustumCulling::Get_Instance())
{	
	Safe_AddRef(m_pFrustumCulling);
	Safe_AddRef(m_pFont_Manager);
	Safe_AddRef(m_pLight_Manager);
	Safe_AddRef(m_pPipeLine);
	Safe_AddRef(m_pComponent_Manager);
	Safe_AddRef(m_pTimer_Manager);
	Safe_AddRef(m_pObject_Manager);
	Safe_AddRef(m_pLevel_Manager);
	Safe_AddRef(m_pInput_Device);
	Safe_AddRef(m_pGraphic_Device); 
	Safe_AddRef(m_pPicking);
	Safe_AddRef(m_pCollision_Manager);
	Safe_AddRef(m_pKeys_Manager);
}

HRESULT CGameInstance::Initialize_Engine(HINSTANCE hInst, _uint iNumLevels, const GRAPHIC_DESC& GraphicDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext)
{
	if (nullptr == m_pGraphic_Device || 
		nullptr == m_pObject_Manager ||
		nullptr == m_pPicking)
		return E_FAIL;

	/* 그래픽 디바이스를 초기화한다. */
	if (FAILED(m_pGraphic_Device->Ready_Graphic_Device(GraphicDesc.hWnd, GraphicDesc.eWinMode, GraphicDesc.iWinSizeX, GraphicDesc.iWinSizeY, ppDevice, ppContext)))
		return E_FAIL;

	/* 입력 디바이스를 초기화한다. */
	if (FAILED(m_pInput_Device->Initialize(hInst, GraphicDesc.hWnd)))
		return E_FAIL;

	/* 사운드 디바이스를 초기화한다. */

	// Picking Initialize
	if (FAILED(m_pPicking->Initialize(*ppDevice, *ppContext, GraphicDesc.hWnd, GraphicDesc.iWinSizeX, GraphicDesc.iWinSizeY)))
		return E_FAIL;

	/* 컨테이너의 공간을 확보해둔다. */
	if (FAILED(m_pObject_Manager->Reserve_Container(iNumLevels)))
		return E_FAIL;

	if (FAILED(m_pComponent_Manager->Reserve_Container(iNumLevels)))
		return E_FAIL;

	if (FAILED(m_pFrustumCulling->Initialize()))
		return E_FAIL;

	return S_OK;
}

void CGameInstance::Tick_Engine(_float fTimeDelta)
{
	if (!m_pLevel_Manager || !m_pObject_Manager || !m_pCollision_Manager)
		return;

	m_pInput_Device->Update();

	m_pLevel_Manager->Tick(fTimeDelta);
	m_pObject_Manager->Tick(fTimeDelta);

	m_pPipeLine->Update();
	m_pPicking->Tick();
	m_pFrustumCulling->Transform_ToWorldSpace();

	m_pLevel_Manager->Late_Tick(fTimeDelta);
	m_pObject_Manager->Late_Tick(fTimeDelta);

	m_pCollision_Manager->Reset_ColliderGroup(); // Be sure to call it last, after all the Late_Tick functions.
}

void CGameInstance::Clear(_uint iLevelIndex)
{
	if (nullptr == m_pObject_Manager || 
		nullptr == m_pComponent_Manager)
		return;

	m_pComponent_Manager->Clear(iLevelIndex);
	m_pObject_Manager->Clear(iLevelIndex);
}

ID3D11RenderTargetView * CGameInstance::Get_BackBufferRTV()
{
	return m_pGraphic_Device->Get_BackBufferRTV();
}

ID3D11DepthStencilView * CGameInstance::Get_DepthStencilView()
{
	return m_pGraphic_Device->Get_DepthStencilView();
}

HRESULT CGameInstance::Clear_BackBuffer_View(_float4 vClearColor)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->Clear_BackBuffer_View(vClearColor);

	return S_OK;
}

HRESULT CGameInstance::Clear_DepthStencil_View()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Clear_DepthStencil_View();
}

HRESULT CGameInstance::Present()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Present();
}

_char CGameInstance::Get_DIKState(_uchar eKeyID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIKState(eKeyID);	
}

_char CGameInstance::Get_DIMKeyState(DIMK eMouseKeyID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMKeyState(eMouseKeyID);
}

_long CGameInstance::Get_DIMMoveState(DIMM eMouseMoveID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMMoveState(eMouseMoveID);
}

HRESULT CGameInstance::Add_Timer(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	return m_pTimer_Manager->Add_Timer(pTimerTag);	
}

_float CGameInstance::Get_TimeDelta(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return 0.0f;

	return m_pTimer_Manager->Get_TimeDelta(pTimerTag);
}

void CGameInstance::Update_TimeDelta(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return;

	m_pTimer_Manager->Update(pTimerTag);
}

HRESULT CGameInstance::Open_Level(unsigned int iLevelIndex, CLevel * pNewLevel)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Open_Level(iLevelIndex, pNewLevel);
}

_uint CGameInstance::Get_CurrentLevelIndex()
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Get_CurrentLevelIndex();
}

HRESULT CGameInstance::Render_NavigationMesh()
{
	if (!m_pLevel_Manager)
		return E_FAIL;

	m_pLevel_Manager->Render_NavigationMesh();

	return S_OK;
}

HRESULT CGameInstance::Add_Prototype(const _tchar * pPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Prototype(pPrototypeTag, pPrototype);	
}

HRESULT CGameInstance::Add_GameObject(const _tchar* pObjName, const _tchar * pPrototypeTag, _uint iLevelIndex, const _tchar * pLayerTag, void* pArg)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_GameObject(pObjName, pPrototypeTag, iLevelIndex, pLayerTag, pArg);
}

CGameObject * CGameInstance::Clone_GameObject(const _tchar * pPrototypeTag, void * pArg)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Clone_GameObject(pPrototypeTag, pArg);
}

CComponent * CGameInstance::Get_Component(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pComponentTag, _uint iIndex)
{
	if (nullptr == m_pObject_Manager)
		return nullptr; 

	return m_pObject_Manager->Get_Component(iLevelIndex, pLayerTag, pComponentTag, iIndex);
}

CObject_Manager::LAYERS CGameInstance::Get_Layers(_uint iLevelIndex)
{
	return m_pObject_Manager->Get_Layers(iLevelIndex);
}

void CGameInstance::Delete_GameObject(CGameObject* pGameObj, _uint iLevelIndex, const _tchar * pLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return;

	m_pObject_Manager->Remove_GameObject(pGameObj, iLevelIndex, pLayerTag);
}

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const _tchar * pPrototypeTag, CComponent * pPrototype)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	return m_pComponent_Manager->Add_Prototype(iLevelIndex, pPrototypeTag, pPrototype);	
}

CComponent * CGameInstance::Clone_Component(_uint iLevelIndex, const _tchar * pPrototypeTag, void * pArg)
{
	if (nullptr == m_pComponent_Manager)
		return nullptr;

	return m_pComponent_Manager->Clone_Component(iLevelIndex, pPrototypeTag, pArg);
}

CComponent * CGameInstance::Find_Component_Prototype(_uint iLevelIndex, const _tchar * pPrototypeTag)
{
	if (nullptr == m_pComponent_Manager)
		return nullptr;

	return m_pComponent_Manager->Find_Component(iLevelIndex, pPrototypeTag);
}

HRESULT CGameInstance::Add_CollisionGroup(CCollision_Manager::COLLISION_GROUP eCollisionGroup, CGameObject * pGameObject)
{
	if (!m_pCollision_Manager)
		return E_FAIL;

	return m_pCollision_Manager->Add_CollisionGroup(eCollisionGroup, pGameObject);
}

void CGameInstance::Out_CollisionGroup(CCollision_Manager::COLLISION_GROUP eCollisionGroup, CGameObject * pGameObject)
{
	if (!m_pCollision_Manager)
		return;

	m_pCollision_Manager->Remove_CollisionGroup(eCollisionGroup, pGameObject);
}

_bool CGameInstance::Collision_with_Group(CCollision_Manager::COLLISION_GROUP eGroup, class CCollider* pDamageCauser, CCollider::AIM eCollisionAim, OUT CGameObject& pDamagedObject)
{
	if (!m_pCollision_Manager)
		return false;

	return m_pCollision_Manager->Collision_with_Group(eGroup, pDamageCauser, eCollisionAim, pDamagedObject);
}

_bool CGameInstance::Collision_Check_Group_Multi(CCollision_Manager::COLLISION_GROUP eGroup, class CCollider* pDamageCauser, CCollider::AIM eCollisionAim, OUT vector<CGameObject*>& pDamagedObjects)
{
	if (!m_pCollision_Manager)
		return false;

	return m_pCollision_Manager->Collision_Check_Group_Multi(eGroup, pDamageCauser, eCollisionAim, pDamagedObjects);
}

void CGameInstance::Apply_Damage(_float fDamage, CGameObject * DamagedObj, CGameObject * DamageCauser, void * AttackType)
{
	DamagedObj->Take_Damage(fDamage, AttackType, DamageCauser);
}

void CGameInstance::Apply_Damage_Multi(_float fDamage, vector<CGameObject*>& vecDamagedObj, CGameObject * DamageCauser, void * AttackType)
{
	for (auto& iter = vecDamagedObj.begin(); iter != vecDamagedObj.end();)
	{
		(*iter)->Take_Damage(fDamage, AttackType, DamageCauser);
		iter++;
	}
}

CGameObject * CGameInstance::Find_Object(_uint iLevelIndex, const _tchar * pLayerTag, _uint iIndex)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_Object(iLevelIndex, pLayerTag, iIndex);
}

void CGameInstance::Set_Transform(CPipeLine::TRANSFORMSTATE eState, _fmatrix TransformMatrix)
{
	if (nullptr == m_pPipeLine)
		return;

	m_pPipeLine->Set_Transform(eState, TransformMatrix);
}

_matrix CGameInstance::Get_TransformMatrix(CPipeLine::TRANSFORMSTATE eState)
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();

	return m_pPipeLine->Get_TransformMatrix(eState);
}

_float4x4 CGameInstance::Get_TransformFloat4x4(CPipeLine::TRANSFORMSTATE eState)
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_TransformFloat4x4(eState);
}

_float4x4 CGameInstance::Get_TransformFloat4x4_TP(CPipeLine::TRANSFORMSTATE eState)
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_TransformFloat4x4_TP(eState);
}

_float4 CGameInstance::Get_CamPosition()
{
	if (nullptr == m_pPipeLine)
		return _float4();

	return m_pPipeLine->Get_CamPosition();
}

const LIGHTDESC * CGameInstance::Get_LightDesc(_uint iIndex)
{
	if (nullptr == m_pLight_Manager)
		return nullptr;

	return m_pLight_Manager->Get_LightDesc(iIndex);	
}

HRESULT CGameInstance::Add_Light(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const LIGHTDESC & LightDesc)
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	return m_pLight_Manager->Add_Light(pDevice, pContext, LightDesc);	
}

HRESULT CGameInstance::Add_Fonts(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFontTag, const _tchar * pFontFilePath)
{
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	return m_pFont_Manager->Add_Fonts(pDevice, pContext, pFontTag, pFontFilePath);	
}

HRESULT CGameInstance::Render_Font(const _tchar * pFontTag, const _tchar * pText, _fvector vPos, _fvector vColor)
{
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	return m_pFont_Manager->Render_Font(pFontTag, pText, vPos, vColor);
}

_bool CGameInstance::Key_Pressing(int _Key)
{
	if (nullptr == m_pKeys_Manager)
		return false;

	return m_pKeys_Manager->Key_Pressing(_Key);
}

_bool CGameInstance::Key_Up(int _Key)
{
	if (nullptr == m_pKeys_Manager)
		return false;

	return m_pKeys_Manager->Key_Up(_Key);
}

_bool CGameInstance::IsIn_Frustum(_fvector vPosition, _float fRange)
{
	if (!m_pFrustumCulling)
		return false;

	return m_pFrustumCulling->IsIn_Frustum(vPosition, fRange);
}

_bool CGameInstance::Key_Down(int _Key)
{
	if (nullptr == m_pKeys_Manager)
		return false;

	return m_pKeys_Manager->Key_Down(_Key);
}

void CGameInstance::Release_Engine()
{
	CGameInstance::Get_Instance()->Destroy_Instance();
	CLevel_Manager::Get_Instance()->Destroy_Instance();
	CObject_Manager::Get_Instance()->Destroy_Instance();
	CComponent_Manager::Get_Instance()->Destroy_Instance();
	CPipeLine::Get_Instance()->Destroy_Instance();
	CLight_Manager::Get_Instance()->Destroy_Instance();
	CTimer_Manager::Get_Instance()->Destroy_Instance();
	CInput_Device::Get_Instance()->Destroy_Instance();
	CFont_Manager::Get_Instance()->Destroy_Instance();
	CGraphic_Device::Get_Instance()->Destroy_Instance();
	CPicking::Get_Instance()->Destroy_Instance();
	CCollision_Manager::Get_Instance()->Destroy_Instance();
	CFrustumCulling::Get_Instance()->Destroy_Instance();
	CKeysManager::Get_Instance()->Destroy_Instance();
}

void CGameInstance::Free()
{
	Safe_Release(m_pFrustumCulling);
	Safe_Release(m_pKeys_Manager);
	Safe_Release(m_pCollision_Manager);
	Safe_Release(m_pPicking);
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pGraphic_Device);
}