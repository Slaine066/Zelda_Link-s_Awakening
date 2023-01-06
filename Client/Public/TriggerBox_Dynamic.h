#pragma once

#include "Client_Defines.h"
#include "TriggerBox.h"

BEGIN(Engine)
END

BEGIN(Client)
class CTriggerBox_Dynamic final : public CTriggerBox
{
private:
	CTriggerBox_Dynamic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTriggerBox_Dynamic(const CTriggerBox_Dynamic& rhs);
	virtual ~CTriggerBox_Dynamic() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual _uint Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	void Field_Triggers(_float fTimeDelta);
	void MoriblinCave_Triggers(_float fTimeDelta);
	void BottleGrotto_Triggers(_float fTimeDelta);
	void MarinHouse_Triggers(_float fTimeDelta);

private:
	_float m_fTimer = 0.f;

public:
	static CTriggerBox_Dynamic* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END