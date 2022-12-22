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
class CModel;
END

BEGIN(Client)
class CEffect : public CGameObject
{
public:
	enum EFFECT_TYPE
	{
		EFFECT_SMOKE,
		EFFECT_HIT,
		EFFECT_DEATH,
		EFFECT_GET_ITEM,
		EFFECT_BOMB_EXPLOSION,
		EFFECT_END
	};

	typedef struct tagEffectDescription
	{
		EFFECT_TYPE m_eEffectType = EFFECT_END;
		_float4x4 m_WorldMatrix;
		_tchar m_pTextureName[MAX_PATH];
		_float m_fEffectTTL = 0.f;
	} EFFECTDESC;

protected:
	CEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect(const CEffect& rhs);
	virtual ~CEffect() = default;

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
	CModel* m_pModelCom = nullptr;

protected:
	EFFECTDESC m_tEffectDesc;
	_float m_fEffectTimer = 0.f;

public:
	static CEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};
END