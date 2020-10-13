// InstallService.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "ServiceInstaller.h"
#include "UninstallServiceJudge.h"

#define  SERVICE_NAME  _T("PopupWindowsService")

std::wstring GetFilePath( const std::wstring& wsFullName )
{
	std::wstring::size_type nIndex1 = wsFullName.find_last_of(L"\\");
	std::wstring::size_type nIndex2 = wsFullName.find_last_of(L"/");
	if (std::wstring::npos == nIndex1)
	{
		nIndex1 = 0;
	}
	if (std::wstring::npos == nIndex2)
	{
		nIndex2 = 0;
	}
	std::wstring::size_type nIndex = max(nIndex1, nIndex2);
	return wsFullName.substr(0, nIndex);
}

std::wstring GetAppPath()
{
	wchar_t lpszFileName[MAX_PATH] = {0};
	::GetModuleFileName(NULL, lpszFileName, MAX_PATH);
	std::wstring strFullName = lpszFileName;

	return GetFilePath(strFullName);
}

void test_service_install()
{
	ServiceControl control;
	std::wstring app_path = GetAppPath();
	app_path += L"\\server.exe";
	if(control.InstallService(SERVICE_NAME, _T("Device push message service"),app_path.c_str()))
	{DLOG("服务创建成功");}
	if(	control.UpdateSvcDesc(SERVICE_NAME, _T("设备推送消息服务")))
	{DLOG("服务添加描述成功");}
	if(	control.StartService(SERVICE_NAME))
	{DLOG("服务启动成功");}
}

void test_service_Stop()
{
	::Sleep(3000);
	ServiceControl control;
	control.StopService(SERVICE_NAME, FALSE);
}

void test_service_delete()
{
	::Sleep(3000);
	ServiceControl control;
	control.StopService(SERVICE_NAME, FALSE);
	control.DeleteService(SERVICE_NAME);
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hInstNULL, LPSTR lpszCmdLine, int nCmdShow) 
{

	DLOG("进入InstallService Main函数");
	if (FALSE == IsZhwExist())
	{
		DLOG("进入删除测试：");
		test_service_delete();
	}
	else
	{
		test_service_install();
	}
	return 0;
}
