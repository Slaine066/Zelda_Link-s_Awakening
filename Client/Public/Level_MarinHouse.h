#pragma once

#include "Client_Defines.h"
#include "Level.h"
#include "Actor.h"

BEGIN(Client)
class CLevel_MarinHouse final : public CLevel
{
private:
	CLevel_MarinHouse(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_MarinHouse() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);

public:
	HRESULT Load_Objects_FromFile();
	HRESULT Load_Triggers_FromFile();
	HRESULT Ready_Sky();
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Effect(const _tchar* pLayerTag);
	HRESULT Ready_Layer_UI();

private:
	vector<CActor::MODELDESC> m_vInstancedObjects;

	class CUI_Manager* m_pUIManager = nullptr;

	_float4 m_vLookPosition = _float4(0.f, 0.f, .25f, 1.f);

public:
	static CLevel_MarinHouse* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};
END