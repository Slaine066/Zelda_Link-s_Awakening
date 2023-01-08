#pragma once

#include "Client_Defines.h"
#include "Actor.h"

BEGIN(Client)
class CItem : public CActor
{
public:
	enum ITEMTYPE
	{
		TYPE_DROP,
		TYPE_TREASURE,
		TYPE_END
	};

	enum ANIMID
	{
		ANIM_FLOAT,
		ANIM_END
	};

	typedef struct tagEquipmentDescription
	{
		ITEMTYPE m_eItemType = TYPE_END;
		ITEMID m_eItemId = ITEM_END;
		CModel::TYPE m_eModelType = CModel::TYPE::TYPE_END;
		_float4x4 mWorldMatrix;
		_bool m_bAnimOnPickup = false;
	} ITEMDESC;

protected:
	CItem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItem(const CItem& rhs);
	virtual ~CItem() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual _uint Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_ShadowDepth() override;

	void Setup_Item(void * pArg);
	virtual HRESULT Ready_Components(void* pArg);
	virtual HRESULT SetUp_ShaderResources();
	virtual HRESULT SetUp_ShadowShaderResources() override;

	virtual void Compute_ShaderTimers(_float fTimeDelta);

public:
	void Set_IsSpendable(_bool bIsSpendable) { m_bIsSpendable = bIsSpendable; }
	ITEMDESC Get_ItemDesc() { return m_tItemDesc; }
	_bool Is_AnimationLoop(_uint eAnimId);
	_tchar* Get_ModelPrototypeId(ITEMID eItemId);
	_bool Check_Collision();
	void PlaySound();

protected:
	ITEMDESC m_tItemDesc;
	_bool m_bIsAnimationFinished = false;
	_bool m_bIsSpendable = false;

	VTXMODELPASS m_eShaderPass = VTXMODELPASS::VTXMODEL_DEFAULT;

	_float m_fDissolveTimer = 0.f;
	_float m_fDissolveLifespan = 1.5f;
	CTexture* m_pDissolveTextureCom = nullptr;

public:
	static CItem* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};
END