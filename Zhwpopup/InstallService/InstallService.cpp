// InstallService.cpp : �������̨Ӧ�ó������ڵ㡣
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
	{DLOG("���񴴽��ɹ�");}
	if(	control.UpdateSvcDesc(SERVICE_NAME, _T("�豸������Ϣ����")))
	{DLOG("������������ɹ�");}
	if(	control.StartService(SERVICE_NAME))
	{DLOG("���������ɹ�");}
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

	DLOG("����InstallService Main����");
	if (FALSE == IsZhwExist())
	{
		DLOG("����ɾ�����ԣ�");
		test_service_delete();
	}
	else
	{
		test_service_install();
	}
	return 0;
}
