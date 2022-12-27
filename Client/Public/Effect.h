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
		/* When Walking/Running. */
		EFFECT_SMOKE,			/* Rect Buffer Effect */	

		/* When Swinging the Sword. */
		EFFECT_SWISH,			/* Model Effect */
		EFFECT_SWORD_SLASH,		/* Model Effect */

		/* When Sword Hits. */
		EFFECT_HIT_RING,		/* Model Effect */
		EFFECT_HIT,				/* Model Effect */
		EFFECT_HIT_FLASH,		/* Rect Buffer Effect */

		/* When Guard Hits. */
		EFFECT_GUARD_RING,		/* Model Effect */
		EFFECT_GUARD,			/* Model Effect */

		/* When Bossblin starts Tackle. */
		EFFECT_SHOCKWAVE_RING,	/* Model Effect */
		EFFECT_SHOCKWAVE,		/* Model Effect */

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
		_float m_fEffectLifespan = 0.f;
		class CActor* m_pOwner = nullptr;
		_bool m_bIsPositionDynamic = false;
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

private:
	_bool Is_ModelEffect();
	_tchar* Get_TextureName();
	_tchar* Get_ModelPrototypeId();

protected:
	CShader* m_pShaderCom = nullptr;
	CTexture* m_pTextureCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CVIBuffer_Rect*	m_pVIBufferCom = nullptr;
	CModel* m_pModelCom = nullptr;

protected:
	EFFECTDESC m_tEffectDesc;

	VTXTEXPASS m_eShaderPass = VTXTEXPASS::VTXTEX_DEFAULT;
	VTXMODELPASS m_eShaderModelPass = VTXMODELPASS::VTXMODEL_DEFAULT;

	_float m_fEffectScale = 1.f;
	_float m_fEffectTimer = 0.f;

public:
	static CEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};
END