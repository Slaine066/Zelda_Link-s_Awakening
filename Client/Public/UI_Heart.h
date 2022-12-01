#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CUI_Heart final : public CUI
{
public:
	enum HEART_TYPE { HEART_FULL, HEART_EMPTY, HEART_HALF, HEART_END };

private:
	CUI_Heart(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Heart(const CUI_Heart& rhs);
	virtual ~CUI_Heart() = default;
	
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual _uint Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual HRESULT Ready_Components(void* pArg);
	virtual HRESULT SetUp_ShaderResources();

public:
	HEART_TYPE Get_HeartType() { return m_eType; }
	void Set_HeartType(HEART_TYPE eType) { m_eType = eType; }

private:
	HEART_TYPE m_eType = HEART_END;

public:
	static CUI_Heart* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};
END