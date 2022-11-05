#include "KeysManager.h"

IMPLEMENT_SINGLETON(CKeysManager)

CKeysManager::CKeysManager()
{
	ZeroMemory(m_bKeyState, sizeof(m_bKeyState));
}

bool CKeysManager::Key_Down(int _Key)
{
	if (!m_bKeyState[_Key] && (GetAsyncKeyState(_Key) & 0x8000))
	{
		m_bKeyState[_Key] = !m_bKeyState[_Key];
		return true;
	}

	if (m_bKeyState[_Key] && !(GetAsyncKeyState(_Key) & 0x8000))
		m_bKeyState[_Key] = !m_bKeyState[_Key];

	return false;
}

bool CKeysManager::Key_Pressing(int _Key)
{
	if (GetAsyncKeyState(_Key) & 0x8000)
		return true;

	return false;
}

bool CKeysManager::Key_Up(int _Key)
{
	if (m_bKeyState[_Key] && !(GetAsyncKeyState(_Key) & 0x8000))
	{
		m_bKeyState[_Key] = !m_bKeyState[_Key];
		return true;
	}

	if (!m_bKeyState[_Key] && (GetAsyncKeyState(_Key) & 0x8000))
		m_bKeyState[_Key] = !m_bKeyState[_Key];

	return false;
}

void CKeysManager::Free()
{
}