#pragma once
#include "stdafx.h"
#include "winsvc.h"
class ServiceWithGUI
{
public:
	ServiceWithGUI();
	~ServiceWithGUI();

	/** @StartServiceWithGUI				打开GUI进程
	*   @bin_path[in]								要打开的GUI进程路径
	*   @return TRUE								GUI进程创建成功
	*   @return FLASE   							GUI进程创建失败	*/
	BOOL StartServiceWithGUI(/*const std::wstring& bin_path*/);
	//static LPCTSTR GetJsonFilePath();

private:

	/*	* @ServiceMain								服务进程主函数
	*   @argc[in]										传递给main()函数的命令行参数个数
	*   @argv[in]								        存放命令行字符串参数的指针数组*/
	VOID static WINAPI ServiceMain(DWORD argc, LPTSTR *argv); 

	/** @ServiceCtrlHandler					服务进程控制
	*   @Opcode[in]									控制操作参数*/
	VOID static WINAPI ServiceCtrlHandler(DWORD Opcode);

	/*	* @ReportSvcStatus						记录服务状态
	*   @dwCurrentState[in]					服务进程现在状态
	*   @dwWin32ExitCode[in]				服务用于报告启动或停止时发生的错误的错误代码。
	*   @dwWaitHint[in]							操作所需的估计时间
	*/
	VOID static ReportSvcStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint);

	//static BOOL Popup();

private:
	//服务句柄
	static SERVICE_STATUS_HANDLE service_status_handle_; 
	//GUI程序地址
	static std::wstring bin_path_;
	//服务状态
	static SERVICE_STATUS service_status_;	
	//static BOOL WriteUrlToFile(std::string strUrl);

};
