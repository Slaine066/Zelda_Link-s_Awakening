#pragma once

#include "Base.h"

BEGIN(Engine)
class ENGINE_DLL CKeysManager final : public CBase
{
	DECLARE_SINGLETON(CKeysManager)

private:
	CKeysManager();
	virtual ~CKeysManager() = default;

public:
	_bool Key_Down(int _Key);
	_bool Key_Pressing(int _Key);
	_bool Key_Up(int _Key);
	
private:
	bool m_bKeyState[VK_MAX];

public:
	virtual void Free() override;
};
END