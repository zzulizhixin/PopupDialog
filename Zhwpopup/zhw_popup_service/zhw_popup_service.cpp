// zhw_popup_service.cpp : 定义控制台应用程序的入口点。
//
#pragma once
#include "stdafx.h"
#include "service_gui.h"
#include "log.h"
#include "string_convert.h"

//std::wstring GetFilePath(const std::wstring& wsFullName)
//{
//	std::wstring::size_type nIndex1 = wsFullName.find_last_of(L"\\");
//	std::wstring::size_type nIndex2 = wsFullName.find_last_of(L"/");
//	if (std::wstring::npos == nIndex1)
//	{
//		nIndex1 = 0;
//	}
//	if (std::wstring::npos == nIndex2)
//	{
//		nIndex2 = 0;
//	}
//	std::wstring::size_type nIndex = max(nIndex1, nIndex2);
//	return wsFullName.substr(0, nIndex);
//}
//
//std::wstring GetAppPath()
//{
//	wchar_t lpszFileName[MAX_PATH] = { 0 };
//	::GetModuleFileName(NULL, lpszFileName, MAX_PATH);
//	std::wstring strFullName = lpszFileName;
//
//	return GetFilePath(strFullName);
//}

//测试
BOOL service_test()
{
	ServiceWithGUI obj;
	//DLOG("进入服务进程测试");
	//std::wstring app_path = GetAppPath();
	//app_path += L"\\pop.exe";
	////wchar_t* path = _T("\\pop.exe");//对话框进程路径
	//DLOG(ws2s(app_path));
	if (obj.StartServiceWithGUI())
	{
		DLOG("服务进程运行成功");
		return TRUE;
	}
	DLOG("服务进程运行失败");
	return FALSE;
}

int _tmain(int argc, _TCHAR* argv[])
{
	if(service_test())
	{
		DLOG("服务进程运行成功");
	}
	else
	{
		DLOG("服务进程运行失败");
	}

	return 0;
}
