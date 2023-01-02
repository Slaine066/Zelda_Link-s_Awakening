#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

/*
This is the Base Class for UI Elements.
Use this Class if you need a Simple piece of UI which does not have any specific behavior.
If you need a more complex UI Class with specific logic just derive from CUI and make a new one (see "CUI_Heart").
*/

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CUI : public CGameObject
{
public:
	typedef struct tagUIDescription
	{
		_float m_fX;
		_float m_fY;
		_float m_fSizeX;
		_float m_fSizeY;
		_tchar m_pTextureName[MAX_PATH];
		VTXTEXPASS m_ePass;
	} UIDESC;

protected:
	CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI(const CUI& rhs);
	virtual ~CUI() = default;

public:
	CTransform* Get_Transform() { return m_pTransformCom; }
	void Set_PositionX(_float fX) { m_tUIDesc.m_fX = fX; }
	void Set_PositionY(_float fY) { m_tUIDesc.m_fY = fY; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual _uint Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual HRESULT Ready_Components(void* pArg);
	virtual HRESULT SetUp_ShaderResources();

protected:
	CShader* m_pShaderCom = nullptr;
	CTexture* m_pTextureCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CVIBuffer_Rect*	m_pVIBufferCom = nullptr;

protected:
	UIDESC m_tUIDesc;
	_float4x4 m_ViewMatrix, m_ProjMatrix;

public:
	static CUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};
END