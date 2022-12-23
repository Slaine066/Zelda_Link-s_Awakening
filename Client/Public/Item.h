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
		CModel::TYPE m_eModelType;
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

	void Setup_Item(void * pArg);
	virtual HRESULT Ready_Components(void* pArg);
	virtual HRESULT SetUp_ShaderResources();

public:
	ITEMDESC Get_ItemDesc() { return m_tItemDesc; }
	_bool Is_AnimationLoop(_uint eAnimId);
	_tchar* Get_ModelPrototypeId(ITEMID eItemId);
	_bool Check_Collision();

protected:
	ITEMDESC m_tItemDesc;
	_bool m_bIsAnimationFinished = false;

public:
	static CItem* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};
END