#pragma once
#include "start_gui.h"
#include "log.h"
#include "open_process.h"
#include "string_convert.h"
#pragma comment(lib, "Wtsapi32.lib")
#pragma comment(lib, "Userenv.lib")

StartGUIProc::StartGUIProc(const std::wstring& processPath, const std::wstring& arguments)
	: processPath_(processPath), arguments_(arguments)
{
	DLOG(ws2s(processPath));
}

HANDLE StartGUIProc::GetCurrentUserToken()
{
	PWTS_SESSION_INFO pSessionInfo = 0;
	DWORD dwCount = 0;
	::WTSEnumerateSessions(WTS_CURRENT_SERVER_HANDLE, 0, 1, &pSessionInfo, &dwCount);
	int session_id = 0;
	for (DWORD i = 0; i < dwCount; ++i)
	{
		WTS_SESSION_INFO si = pSessionInfo[i];
		if (WTSActive == si.State)
		{
			session_id = si.SessionId;
			break;
		}
	}
	::WTSFreeMemory(pSessionInfo);
	HANDLE current_token = 0;
	BOOL bRet = ::WTSQueryUserToken(session_id, &current_token);

	//打印日志
	int errorcode = GetLastError();
	std::string s = std::to_string(errorcode);
	std::string lasterr = "WTSQueryUserToken";
	lasterr += s;
	DLOG(lasterr);

	if (bRet == false)
	{
		return 0;
	}
	HANDLE primaryToken = 0;
	bRet = ::DuplicateTokenEx(current_token, TOKEN_ASSIGN_PRIMARY | TOKEN_ALL_ACCESS, 0, SecurityImpersonation, TokenPrimary, &primaryToken);
	//打印日志
	errorcode = GetLastError();
	s = std::to_string(errorcode);
	lasterr = "DuplicateTokenEx";
	lasterr += s;
	DLOG(lasterr);

	if (bRet == false)
	{
		return 0;
	}
	return primaryToken;
}

BOOL StartGUIProc::Run()
{
	HANDLE primaryToken = GetCurrentUserToken();
	if (primaryToken == 0)
	{
		return FALSE;
	}
	//STARTUPINFO StartupInfo = {0};
	//PROCESS_INFORMATION processInfo = {};
	//StartupInfo.cb = sizeof(STARTUPINFO);

	std::wstring command = L"\"" + processPath_ + L"\"";
	if (arguments_.length() != 0)
	{
		command += L" " + arguments_;
	}
	//void* lpEnvironment = NULL;
	//BOOL resultEnv = ::CreateEnvironmentBlock(&lpEnvironment, primaryToken, FALSE);
	//if (resultEnv == 0)
	//{                                
	//	long nError = GetLastError();  
	//	//打印日志
	//	std::string s = std::to_string(nError);
	//	std::string lasterr = "CreateEnvironmentBlock";
	//	lasterr += s;
	//	DLOG(lasterr);
	//}
	MyProcessMgr  ProcessMgr;
	ProcessMgr.SetIsWait(TRUE);
	BOOL result = ProcessMgr.CreateProcessAsUser(processPath_.c_str(),  1);
	if (result)
	{
		DLOG("result is true");
	}
	//BOOL result = CreateProcessAsUser(
	//	primaryToken, 
	//	0, 
	//	(LPWSTR)(processPath_.c_str()),
	//	NULL, 
	//	NULL, 
	//	FALSE, 
	//	NORMAL_PRIORITY_CLASS | CREATE_UNICODE_ENVIRONMENT, 
	//	NULL, 
	//	0, 
	//	&StartupInfo, 
	//	&processInfo);

	//打印日志
	long nError = GetLastError();
	std::string s = std::to_string(nError);
	std::string lasterr = "CreateProcessAsUser错误码";
	lasterr += s;
	DLOG(lasterr);
	DLOG(ws2s(processPath_));

	//::DestroyEnvironmentBlock(lpEnvironment);
	//::CloseHandle(primaryToken);
	return result;
}