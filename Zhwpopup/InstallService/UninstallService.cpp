#include "log.h"
#include "UninstallService.h"

BOOL IsFileExist(const std::wstring& csFile)
{
	DWORD dwAttrib = GetFileAttributes(csFile.c_str());
	return INVALID_FILE_ATTRIBUTES != dwAttrib && 0 == (dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
}

BOOL IsZhwExist()
{
	BOOL result = FALSE;
	HKEY hKey;		//ע����ȡ���
	LONG lRet = RegOpenKeyExW(
		HKEY_LOCAL_MACHINE,
		_T("SOFTWARE\\WOW6432Node\\daofeng\\zuhaowan"),
		REG_OPTION_OPEN_LINK,
		KEY_ALL_ACCESS,
		&hKey
	);
	if (lRet != ERROR_SUCCESS)
	{
		DLOG("[PopupAdvert:CheckZhwExist]��ע���SOFTWARE\\WOW6432Node\\daofeng\\zuhaowanʧ��");
		return result;
	}

	//��ȡ�ͻ���·��
	TCHAR tchClientPath[64];
	DWORD dwSize;
	dwSize = sizeof(tchClientPath);
	lRet = RegQueryValueEx(hKey, _T("main"), NULL, NULL, (LPBYTE)tchClientPath, &dwSize);
	if (lRet != ERROR_SUCCESS)
	{
		DLOG("[PopupAdvert:CheckZhwExist]��ע���SOFTWARE\\WOW6432Node\\daofeng\\zuhaowan\\main�ͻ���·��ʧ��");
		return result;
	}
	if (IsFileExist(tchClientPath))
	{
		result = TRUE;
	}
	CloseHandle(hKey);
	return result;
}

