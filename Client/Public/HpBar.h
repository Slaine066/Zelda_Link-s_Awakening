#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CHpBar final : public CGameObject
{
private:
	CHpBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHpBar(const CHpBar& rhs);
	virtual ~CHpBar() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual _uint Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	void Get_PlayerHp();
	void Init_Hearts();
	void Compute_Hearts();

private:
	vector<CUI*> m_Hearts;
	_float m_fMaxHp = 0.f;
	_float m_fCurrentHp = 0.f;
	
public:
	static CHpBar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};
END