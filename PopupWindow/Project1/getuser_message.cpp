#pragma warning(disable:4996)
#include "getuser_message.h"
#include "ReadJson.h"
#include <Iphlpapi.h>
#include <iptypes.h>

struct ASTAT
{
	ADAPTER_STATUS adapt;
	NAME_BUFFER NameBuff[30];
} Adapter;

wchar_t* GetMacAddress()
{
	LANA_ENUM lenum;
	static TCHAR szMACAddress[64];
	NCB Ncb = { 0 };
	Ncb.ncb_command = NCBENUM;
	Ncb.ncb_buffer = (UCHAR*)& lenum;
	Ncb.ncb_length = sizeof(lenum);
	if (Netbios(&Ncb) == 0)
	{
		for (int i = 0; i < lenum.length; ++i)
		{
			ZeroMemory(&Ncb, sizeof(Ncb));
			Ncb.ncb_command = NCBRESET;
			Ncb.ncb_lana_num = lenum.lana[i];
			if (Netbios(&Ncb) != 0)continue;
			ZeroMemory(&Ncb, sizeof(Ncb));
			Ncb.ncb_command = NCBASTAT;
			Ncb.ncb_lana_num = lenum.lana[i];
			lstrcpyA((LPSTR)(Ncb.ncb_callname), "*");
			Ncb.ncb_buffer = (UCHAR*)& Adapter;
			Ncb.ncb_length = sizeof(Adapter);
			if (Netbios(&Ncb) != 0)continue;
			wsprintf(szMACAddress, TEXT("%02X%02X%02X%02X%02X%02X"),
				Adapter.adapt.adapter_address[0],
				Adapter.adapt.adapter_address[1],
				Adapter.adapt.adapter_address[2],
				Adapter.adapt.adapter_address[3],
				Adapter.adapt.adapter_address[4],
				Adapter.adapt.adapter_address[5]);
		}
	}
	return szMACAddress;
}

std::string GetSysVersion()
{
	OSVERSIONINFO osver = { sizeof(OSVERSIONINFO) };
	GetVersionEx(&osver);
	std::string os_name;
	if (osver.dwMajorVersion == 5 && osver.dwMinorVersion == 0)
	{
		os_name = "windows2000";
	}
	else if (osver.dwMajorVersion == 5 && osver.dwMinorVersion == 1)
	{
		os_name = "windowsXP";
	}
	else if (osver.dwMajorVersion == 6 && osver.dwMinorVersion == 0)
	{
		os_name = "windows2003";
	}
	else if (osver.dwMajorVersion == 5 && osver.dwMinorVersion == 2)
	{
		os_name = "WindowsVista";
	}
	else if (osver.dwMajorVersion == 6 && osver.dwMinorVersion == 1)
	{
		os_name = "Windows7";
	}
	else
		os_name = "Windows10";
	return os_name;
}

BOOL GetUserId(std::string& user_id)
{
	HKEY hKey;		//注册表获取句柄
	LONG lRet = RegOpenKeyExW(
		HKEY_LOCAL_MACHINE,
		L"SOFTWARE\\WOW6432Node\\daofeng\\zuhaowan\\client",
		REG_OPTION_OPEN_LINK,
		KEY_ALL_ACCESS,
		&hKey
	);
	if (lRet != ERROR_SUCCESS)
	{
		::CloseHandle(hKey);
		return NULL;
	}
	//获取客户端路径
	TCHAR tchUserId[64];
	DWORD dwSize;
	dwSize = sizeof(tchUserId);
	lRet = RegQueryValueEx(hKey, L"username", NULL, NULL, (LPBYTE)tchUserId, &dwSize);
	if (lRet != ERROR_SUCCESS)
	{
		::CloseHandle(hKey);
		return FALSE;
	}
	user_id = ws2s(tchUserId);
	return TRUE;
}

