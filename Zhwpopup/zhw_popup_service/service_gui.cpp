#pragma once
#include "stdafx.h"
#include "service_gui.h"
#include "start_gui.h"
#include "log.h"
#include "check_time.h"
#include "string_convert.h"
#include "GetHttp.h"
#include <Shlobj.h>
#include "PopupAdvert.h"
#include "open_process.h"
#define __WFUNC__ TEXT(__FUNCTION__)

//const std::string strUrL = "http://www1.zuhaowan.com:98/InterfaceApi/ClientPop/getPopout";
//LPCTSTR json_path = ServiceWithGUI::GetJsonFilePath();

SERVICE_STATUS_HANDLE  ServiceWithGUI::service_status_handle_ = NULL;
//std::wstring ServiceWithGUI::bin_path_;
SERVICE_STATUS ServiceWithGUI::service_status_={0};

ServiceWithGUI::ServiceWithGUI()
{

}

ServiceWithGUI::~ServiceWithGUI()
{
}

BOOL ServiceWithGUI::StartServiceWithGUI(/*const std::wstring& bin_path*/)
{
	//bin_path_ = bin_path;
	//DLOG(ws2s(bin_path));

	SERVICE_TABLE_ENTRY ServiceTable[2];
	ServiceTable[0].lpServiceName = L"PopupWindowsService";
	ServiceTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)ServiceMain;
	ServiceTable[1].lpServiceName = NULL;
	ServiceTable[1].lpServiceProc = NULL;
	DLOG("[service process] StartServiceWithGUI");

	if (!::StartServiceCtrlDispatcher(ServiceTable))
	{
		DLOG("[service process] StartServiceCtrlDispatcher 错误");
		return FALSE;
	}
	return TRUE;
}

VOID WINAPI ServiceWithGUI::ServiceMain(DWORD argc, LPTSTR* argv)
{
	DLOG("[service process]进入服务进程主函数");
	service_status_handle_ = ::RegisterServiceCtrlHandler(L"", ServiceCtrlHandler);
	if (service_status_handle_ == 0)
	{
		DLOG("[service process]进入服务进程主函数,service_status_handle 为0");
		return;
	}

	service_status_.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	service_status_.dwServiceSpecificExitCode = 0;
	ReportSvcStatus(SERVICE_START_PENDING, NO_ERROR, 3000);


	//弹窗广告模块

	PopupAdvert popAdvert;
	if (popAdvert.PopupJudge())
	{
		DLOG("[service process]弹窗服务执行成功");
	}


	ReportSvcStatus(SERVICE_RUNNING, NO_ERROR, 0);
	while (service_status_.dwCurrentState != SERVICE_STOP_PENDING)
	{
		Sleep(500);
	}
	ReportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0);
	return;
}

VOID WINAPI ServiceWithGUI::ServiceCtrlHandler(DWORD Opcode)
{
	switch (Opcode)
	{
	case SERVICE_CONTROL_PAUSE:
		DLOG("[service process] 暂停");
		ReportSvcStatus(SERVICE_PAUSED, NO_ERROR, 0);
		break;
	case SERVICE_CONTROL_CONTINUE:
		DLOG("[service process] 继续");
		ReportSvcStatus(SERVICE_RUNNING, NO_ERROR, 0);
		break;
	case SERVICE_CONTROL_STOP:
		DLOG("[service process] 停止");
		ReportSvcStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);
		break;
	case SERVICE_CONTROL_INTERROGATE:
		break;
	default:
		break;
	}
	return;
}

VOID ServiceWithGUI::ReportSvcStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint)
{
	static DWORD dwCheckPoint = 1;

	service_status_.dwCurrentState = dwCurrentState;
	service_status_.dwWin32ExitCode = dwWin32ExitCode;
	service_status_.dwWaitHint = dwWaitHint;

	if (dwCurrentState == SERVICE_START_PENDING)
		service_status_.dwControlsAccepted = 0;
	else service_status_.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE;

	if ((dwCurrentState == SERVICE_RUNNING) ||
		(dwCurrentState == SERVICE_STOPPED))
		service_status_.dwCheckPoint = 0;
	else service_status_.dwCheckPoint = dwCheckPoint++;

	::SetServiceStatus(service_status_handle_, &service_status_);
}

