#pragma once
#include "stdafx.h"
#include "winsvc.h"

class StartGUIProc
{
public:
	/** @StartServiceWithGUI				构造服务进程
	*   @bin_path[in]								服务进程路径	*/
	StartGUIProc(const std::wstring& processPath, const std::wstring& arguments = L"");

	//通过服务运行服务进程
	BOOL Run();

private:
	//获取当前用户令牌
	HANDLE GetCurrentUserToken();

private:
	//服务进程的路径
	std::wstring processPath_;

	std::wstring arguments_;
};