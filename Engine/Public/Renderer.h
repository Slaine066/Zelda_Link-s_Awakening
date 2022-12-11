#pragma once

/* ȭ�鿡 �׷������� ��ü���� ��Ƽ� �׸��� ������� �����Ѵ�. ������� �����Լ��� ȣ�����ش�. */
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CRenderer final : public CComponent
{
public:
	enum RENDERGROUP {RENDER_PRIORITY, RENDER_NONALPHABLEND, RENDER_NONLIGHT, RENDER_ALPHABLEND, RENDER_UI, RENDER_END };

private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);	
	virtual ~CRenderer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;

public:
	HRESULT Add_RenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pGameObject);
	HRESULT Render_GameObjects();

#ifdef _DEBUG
	HRESULT Add_Debug(class CGameObject* pDebugObj);
	HRESULT Add_Debug(class CComponent* pDebugCom);
#endif // _DEBUG

private:
	list<class CGameObject*>				m_GameObjects[RENDER_END];
	typedef list<class CGameObject*>		GAMEOBJECTS;
	class CTargetManager*					m_pTargetManager = nullptr;
	class CLight_Manager*					m_pLight_Manager = nullptr;
	class CVIBuffer_Rect*					m_pVIBuffer = nullptr;
	class CShader*							m_pShader = nullptr;
	_float4x4								m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;

#ifdef _DEBUG
	list<class CGameObject*>				m_DebugObjects;
	list<class CComponent*>					m_DebugComponents;
#endif

private:
	HRESULT Render_Priority();
	HRESULT Render_NonAlphaBlend();
	HRESULT Render_Lights();
	HRESULT Render_Blend();
	HRESULT Render_NonLight();
	HRESULT Render_AlphaBlend();
	HRESULT Render_UI();

#ifdef _DEBUG
	HRESULT Render_Debug();
#endif // _DEBUG

public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg = nullptr)override;
	virtual void Free();
};
END