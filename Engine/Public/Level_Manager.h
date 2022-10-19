#pragma once

/*  ���� ȭ�鿡 ��������� ������ �ּҸ� �����Ѵ�. ���緹���� �ε����� ����.  */
/* ������ ������Ʈ ó���� �Ѵ�. */
/* ������ ����ó��.(x) */
/* ������������ ����ߴ� ���ҽ����� �����Ѵ�. */

#include "Base.h"

BEGIN(Engine)

class CLevel_Manager final : public CBase
{
	DECLARE_SINGLETON(CLevel_Manager)
public:
	CLevel_Manager();
	virtual ~CLevel_Manager() = default;

public:
	HRESULT Open_Level(unsigned int iLevelIndex, class CLevel* pNewLevel);
	void Tick(_float fTimeDelta);
	void Late_Tick(_float fTimeDelta);
	void Render();

private:
	class CLevel*				m_pCurrentLevel = nullptr;
	_uint						m_iLevelIndex;
public:
	virtual void Free() override;
};

END

