#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CUI abstract : public CGameObject
{
public:
	typedef struct tagUIDescription
	{
		_float m_fX;
		_float m_fY;
		_float m_fSizeX;
		_float m_fSizeY;
		_tchar m_pTextureName[MAX_PATH];
	} UIDESC;

protected:
	CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI(const CUI& rhs);
	virtual ~CUI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual _uint Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual HRESULT Ready_Components(void* pArg) PURE;
	virtual HRESULT SetUp_ShaderResources() PURE;

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
	virtual void Free() override;
};
END