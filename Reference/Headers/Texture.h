#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTexture final : public CComponent
{
private:
	CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTexture(const CTexture& rhs);
	virtual ~CTexture() = default;

public:
	ID3D11ShaderResourceView* Get_SRV(_uint iIndex = 0) const {
		return m_SRVs[iIndex];
	}

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pTextureFilePath, _uint iNumTextures);
	virtual HRESULT Initialize(void* pArg);

private:
	/* ID3D11ShaderResourceView : ID3D11Texture2D로 생성할 수 있는 객체 타입 */
	/* 셰이더 전역변수에 전달 될 수 있는 리소스이다. */
	vector<ID3D11ShaderResourceView*>			m_SRVs;
	typedef vector<ID3D11ShaderResourceView*>	SRVS;

public:
	static CTexture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTextureFilePath, _uint iNumTextures = 1);
	virtual CComponent* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END