#pragma once
#include "stdafx.h"
#include "check_time.h"
#include "string_convert.h"

BOOL GetCloseTimeLong(time_t& close_time_long)
{
	HKEY hKey;		//注册表获取句柄
	LONG lRet = RegOpenKeyExW(
		HKEY_LOCAL_MACHINE,
		L"SOFTWARE\\WOW6432Node\\daofeng\\zuhaowan",
		REG_OPTION_OPEN_LINK,
		KEY_ALL_ACCESS,
		&hKey
	);
	if (lRet != ERROR_SUCCESS)
	{
		::CloseHandle(hKey);
		return FALSE;
	}
	//获取客户端路径
	TCHAR stop_time[64];//注册表stop_time;
	DWORD dwSize;
	dwSize = sizeof(stop_time);
	lRet = RegQueryValueEx(hKey, L"stop_time", NULL, NULL, (LPBYTE)stop_time, &dwSize);
	if (lRet != ERROR_SUCCESS)
	{
		::CloseHandle(hKey);
		return FALSE;
	}
	__int64 getStopTime = _ttoi(stop_time);				//字符串转化
	time_t time_now = time(NULL);
	close_time_long = time_now - getStopTime;		//时间差
	return TRUE;
}

BOOL CheckTimeOut(const time_t& close_time_long, const PopupMessage& pop_message)
{
	//后台接收的limit时间的天数转为秒数
	time_t limit_time;
	limit_time = (atoi(pop_message.start_up_limit.c_str()) * 60 * 60 );
	if (close_time_long > limit_time)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CheckMessagePop(const PopupMessage& pop_message)
{
	if (0 == pop_message.code)
	{
		return FALSE;
	}
	else
		return TRUE;
}

BOOL CheckUserOperation(time_t& click_time_long, int& click_type)
{
	HKEY hKey;		//注册表获取句柄
	LONG lRet = RegOpenKeyExW(
		HKEY_LOCAL_MACHINE,
		L"SOFTWARE\\WOW6432Node\\daofeng\\zuhaowan\\user_operation",
		REG_OPTION_OPEN_LINK,
		KEY_ALL_ACCESS,
		&hKey
	);
	if (lRet != ERROR_SUCCESS)
	{
		::CloseHandle(hKey);
		return FALSE;
	}
	DWORD dwClickTime;
	DWORD dwSize;
	dwSize = sizeof(dwClickTime);
	lRet = RegQueryValueEx(hKey, L"click_time", NULL, NULL, (LPBYTE)& dwClickTime, &dwSize);
	if (lRet != ERROR_SUCCESS)
	{
		::CloseHandle(hKey);
		return FALSE;
	}
	time_t time_now = time(NULL);
	click_time_long = time_now - dwClickTime;//时间差

	char strClickType[256];//注册表click_type;
	dwSize = sizeof(strClickType);
	lRet = RegQueryValueEx(hKey, L"click_type", NULL, NULL, (LPBYTE)& strClickType, &dwSize);
	if (lRet != ERROR_SUCCESS)
	{
		::CloseHandle(hKey);
		return FALSE;
	}

	click_type = atoi(strClickType);//操作类型
	return TRUE;
}

BOOL WriteServerStatus(const std::string& server_status)
{
	HKEY hKey;		//注册表获取句柄
	LONG lRet = RegOpenKeyExW(
		HKEY_LOCAL_MACHINE,
		L"SOFTWARE\\WOW6432Node\\daofeng\\zuhaowan",
		REG_OPTION_OPEN_LINK,
		KEY_ALL_ACCESS,
		&hKey);
	if (lRet != ERROR_SUCCESS)
	{
		::CloseHandle(hKey);
		return FALSE;
	}
	HKEY hTempKey;

	if (ERROR_SUCCESS == ::RegCreateKey(hKey, _T("user_operation"), &hTempKey))
	{
		if (ERROR_SUCCESS == ::RegSetValueEx(hTempKey, _T("server_status"), 0, REG_SZ, (LPBYTE)server_status.c_str(), server_status.length() + 1))
		{
			::RegCloseKey(hKey);
			::RegCloseKey(hTempKey);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CheckServerStatus(int& server_status)
{
	HKEY hKey;		//注册表获取句柄
	LONG lRet = RegOpenKeyExW(
		HKEY_LOCAL_MACHINE,
		L"SOFTWARE\\WOW6432Node\\daofeng\\zuhaowan\\user_operation",
		REG_OPTION_OPEN_LINK,
		KEY_ALL_ACCESS,
		&hKey
	);
	if (lRet != ERROR_SUCCESS)
	{
		::CloseHandle(hKey);
		return FALSE;
	}
	DWORD dwSize;
	char strServerStatus[256];//注册表server_status;
	dwSize = sizeof(strServerStatus);


	lRet = RegQueryValueEx(hKey, L"server_status", NULL, NULL, (LPBYTE)& strServerStatus, &dwSize);
	if (lRet != ERROR_SUCCESS)
	{
		::CloseHandle(hKey);
		return FALSE;
	}
	server_status = atoi(strServerStatus);
	return TRUE;
}

BOOL WriteLastAskServerTime()
{
	HKEY hKey;		//注册表获取句柄
	LONG lRet = RegOpenKeyExW(
		HKEY_LOCAL_MACHINE,
		L"SOFTWARE\\WOW6432Node\\daofeng\\zuhaowan",
		REG_OPTION_OPEN_LINK,
		KEY_ALL_ACCESS,
		&hKey);
	if (lRet != ERROR_SUCCESS)
	{
		::CloseHandle(hKey);
		return FALSE;
	}
	HKEY hTempKey;
	if (ERROR_SUCCESS == ::RegCreateKey(hKey, _T("user_operation"), &hTempKey))
	{
			DWORD scurTime = (DWORD)time(NULL);
			if (ERROR_SUCCESS == ::RegSetValueEx(hTempKey, _T("last_ask_server"), 0, REG_DWORD, (LPBYTE)&scurTime, sizeof(scurTime)))
			{
				::RegCloseKey(hKey);
				::RegCloseKey(hTempKey);
				return TRUE;
			}
	}
	return FALSE;
}

BOOL CheckLastAskServerTime(time_t& last_ask_server_long)
{
	HKEY hKey;		//注册表获取句柄
	LONG lRet = RegOpenKeyExW(
		HKEY_LOCAL_MACHINE,
		L"SOFTWARE\\WOW6432Node\\daofeng\\zuhaowan\\user_operation",
		REG_OPTION_OPEN_LINK,
		KEY_ALL_ACCESS,
		&hKey
	);
	if (lRet != ERROR_SUCCESS)
	{
		::CloseHandle(hKey);
		return FALSE;
	}
	DWORD dwLastAskServerTime;
	DWORD dwSize;
	dwSize = sizeof(dwLastAskServerTime);
	lRet = RegQueryValueEx(hKey, L"last_ask_server", NULL, NULL, (LPBYTE)& dwLastAskServerTime, &dwSize);
	if (lRet != ERROR_SUCCESS)
	{
		::CloseHandle(hKey);
		return FALSE;
	}
	time_t time_now = time(NULL);
	last_ask_server_long = time_now - dwLastAskServerTime;//时间差
	return TRUE;
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

