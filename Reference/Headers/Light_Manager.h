#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CLight_Manager final : public CBase
{
	DECLARE_SINGLETON(CLight_Manager)
private:
	CLight_Manager();
	virtual ~CLight_Manager() = default;

public:
	const LIGHTDESC* Get_LightDesc(_uint iIndex);

	_float4x4 Get_ShadowLightViewMatrix();
	HRESULT Set_ShadowLightViewMat(_float4 vEye, _float4 vAt);

public:
	HRESULT Add_Light(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHTDESC& LightDesc);
	HRESULT Render_Lights(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
	void Clear();

private:
	list<class CLight*> m_Lights;

	_float4x4 m_mShadowLightViewMatrix;

public:
	virtual void Free() override;
};

END