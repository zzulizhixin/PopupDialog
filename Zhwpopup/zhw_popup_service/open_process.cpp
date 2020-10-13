#pragma once
#include"open_process.h"
#include <WtsApi32.h>
#include <Sddl.h>
#include <Userenv.h>
#include "log.h"
#include <string>
//#include ".\..\KisCreateProcess.h"
#pragma warning(disable:4996)
#pragma comment(lib, "Userenv.lib")
BOOL KisCreateProcess::CreateProcess(
	LPTSTR psCmdLine)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);

	ZeroMemory(&pi, sizeof(pi));

	BOOL bRetCode = ::CreateProcess(NULL, psCmdLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
	if (bRetCode == FALSE)
		return FALSE;

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return TRUE;
}

BOOL KisCreateProcess::CreateProcessAsActivedUser(
	LPTSTR psCmd,
	LPTSTR psCurrentDir)
{
	if (psCmd == NULL)
		return FALSE;

	BOOL bRetCode = FALSE;
	HANDLE hToken = NULL;
	int nRetCode;

	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi = { 0 };

	nRetCode = GetActiveUserToken(&hToken);
	if (nRetCode != 0)
		goto Exit0;

	si.lpDesktop = _T("Winsta0\\Default");

	DWORD dwCreationFlag = NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE;
	LPVOID pEnvironment = NULL;

	PFN_CreateEnvironmentBlock pCreateEnvironmentBlock = NULL;
	PFN_DestroyEnvironmentBlock pDestroyEnvironmentBlock = NULL;

	HMODULE hUserEnvLib = NULL;
	hUserEnvLib = LoadLibrary(_T("userenv.dll"));
	if (NULL != hUserEnvLib)
	{
		pCreateEnvironmentBlock = (PFN_CreateEnvironmentBlock)GetProcAddress(hUserEnvLib, "CreateEnvironmentBlock");
		pDestroyEnvironmentBlock = (PFN_DestroyEnvironmentBlock)GetProcAddress(hUserEnvLib, "DestroyEnvironmentBlock");
	}

	if (NULL != pCreateEnvironmentBlock)
	{
		if (pCreateEnvironmentBlock(&pEnvironment, hToken, FALSE))
		{
			dwCreationFlag |= CREATE_UNICODE_ENVIRONMENT;
		}
		else
		{
			pEnvironment = NULL;
		}
	}

	bRetCode = CreateProcessAsUser(hToken, NULL, psCmd, NULL, NULL, FALSE, dwCreationFlag, pEnvironment, psCurrentDir, &si, &pi);

	if (pi.hProcess != NULL)
		CloseHandle(pi.hProcess);

	if (pi.hThread != NULL)
		CloseHandle(pi.hThread);

	if (NULL != pDestroyEnvironmentBlock)
		pDestroyEnvironmentBlock(pEnvironment);

	if (NULL != hUserEnvLib)
		FreeLibrary(hUserEnvLib);

Exit0:
	if (hToken != NULL)
		CloseHandle(hToken);

	return bRetCode;
}

BOOL KisCreateProcess::CreateAdminProcessAsActivedUser(
	LPTSTR psCmd,
	LPTSTR psCurrentDir)
{
	if (psCmd == NULL)
		return FALSE;

	OSVERSIONINFO version = { 0 };
	version.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	BOOL bIsVista = FALSE;

	if (GetVersionEx(&version))
	{
		if (version.dwMajorVersion == 6)
		{
			bIsVista = true;
		}
	}

	BOOL bRetCode = FALSE;
	int nRetCode;
	HANDLE hToken = NULL;

	if (!bIsVista)
	{
		nRetCode = GetActiveUserToken(&hToken);
	}
	else
	{
		nRetCode = GetActiveUserTokenOnVista(&hToken);
	}

	if (nRetCode == 0)
	{
		STARTUPINFO si = { sizeof(si) };
		PROCESS_INFORMATION pi = { 0 };

		if (!IsActiveUserTokenAdmin(hToken))
		{
			GetActiveUserTokenFromSystem(&hToken);
		}

		bRetCode = CreateProcessAsUser(hToken, NULL, psCmd, NULL, NULL, FALSE, 0, NULL, psCurrentDir, &si, &pi);

		if (pi.hProcess != NULL)
			CloseHandle(pi.hProcess);

		if (pi.hThread != NULL)
			CloseHandle(pi.hThread);
	}

	if (hToken != NULL)
		CloseHandle(hToken);

	return bRetCode;
}

BOOL KisCreateProcess::CreateProcessAsActivedUserOnVista(
	LPTSTR psCmd,
	LPTSTR psCurrentDir)
{
	if (psCmd == NULL)
		return FALSE;

	BOOL bRetCode = FALSE;
	HANDLE hToken = NULL;

	if (GetActiveUserTokenOnVista(&hToken) == 0)
	{
		STARTUPINFO si = { sizeof(si) };
		PROCESS_INFORMATION pi = { 0 };

		si.lpDesktop = _T("Winsta0\\Default");

		DWORD dwCreationFlag = NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE;
		LPVOID pEnvironment = NULL;

		PFN_CreateEnvironmentBlock pCreateEnvironmentBlock = NULL;
		PFN_DestroyEnvironmentBlock pDestroyEnvironmentBlock = NULL;

		HMODULE hUserEnvLib = NULL;

		hUserEnvLib = LoadLibrary(_T("userenv.dll"));
		if (NULL != hUserEnvLib)
		{
			pCreateEnvironmentBlock = (PFN_CreateEnvironmentBlock)GetProcAddress(hUserEnvLib, "CreateEnvironmentBlock");
			pDestroyEnvironmentBlock = (PFN_DestroyEnvironmentBlock)GetProcAddress(hUserEnvLib, "DestroyEnvironmentBlock");
		}

		if (NULL != pCreateEnvironmentBlock)
		{
			if (pCreateEnvironmentBlock(&pEnvironment, hToken, FALSE))
			{
				dwCreationFlag |= CREATE_UNICODE_ENVIRONMENT;
			}
			else
			{
				pEnvironment = NULL;
			}
		}

		bRetCode = CreateProcessAsUser(hToken, NULL, psCmd, NULL, NULL, FALSE, dwCreationFlag, pEnvironment, psCurrentDir, &si, &pi);

		if (pi.hProcess != NULL)
			CloseHandle(pi.hProcess);

		if (pi.hThread != NULL)
			CloseHandle(pi.hThread);

		if (NULL != pDestroyEnvironmentBlock)
			pDestroyEnvironmentBlock(pEnvironment);

		if (NULL != hUserEnvLib)
			FreeLibrary(hUserEnvLib);
	}

	if (hToken != NULL)
		CloseHandle(hToken);

	return bRetCode;
}

BOOL KisCreateProcess::CreateProcessAsActivedUserOnVistaTillExit(
	LPTSTR psCmd,
	LPTSTR psCurrentDir)
{
	if (psCmd == NULL)
		return FALSE;

	BOOL bRetCode = FALSE;
	HANDLE hToken = NULL;

	if (GetActiveUserTokenOnVista(&hToken) == 0)
	{
		STARTUPINFO si = { sizeof(si) };
		PROCESS_INFORMATION pi = { 0 };

		si.lpDesktop = _T("Winsta0\\Default");

		DWORD dwCreationFlag = NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE;
		LPVOID pEnvironment = NULL;

		PFN_CreateEnvironmentBlock pCreateEnvironmentBlock = NULL;
		PFN_DestroyEnvironmentBlock pDestroyEnvironmentBlock = NULL;

		HMODULE hUserEnvLib = NULL;
		hUserEnvLib = LoadLibrary(_T("userenv.dll"));
		if (NULL != hUserEnvLib)
		{
			pCreateEnvironmentBlock = (PFN_CreateEnvironmentBlock)GetProcAddress(hUserEnvLib, "CreateEnvironmentBlock");
			pDestroyEnvironmentBlock = (PFN_DestroyEnvironmentBlock)GetProcAddress(hUserEnvLib, "DestroyEnvironmentBlock");
		}

		if (NULL != pCreateEnvironmentBlock)
		{
			if (pCreateEnvironmentBlock(&pEnvironment, hToken, FALSE))
			{
				dwCreationFlag |= CREATE_UNICODE_ENVIRONMENT;
			}
			else
			{
				pEnvironment = NULL;
			}
		}

		bRetCode = CreateProcessAsUser(hToken, NULL, psCmd, NULL, NULL, FALSE, dwCreationFlag, pEnvironment, psCurrentDir, &si, &pi);
		if (bRetCode)
		{
			::WaitForSingleObject(pi.hProcess, INFINITE);
		}

		if (pi.hProcess != NULL)
			CloseHandle(pi.hProcess);

		if (pi.hThread != NULL)
			CloseHandle(pi.hThread);

		if (NULL != pDestroyEnvironmentBlock)
			pDestroyEnvironmentBlock(pEnvironment);

		if (NULL != hUserEnvLib)
			FreeLibrary(hUserEnvLib);
	}

	if (hToken != NULL)
		CloseHandle(hToken);

	return bRetCode;
}

BOOL KisCreateProcess::CreateProcessAsActivedUserBySession(
	LPTSTR pszCmd,
	LPTSTR pszCurrentDir,
	DWORD dwSession)
{
	if (pszCmd == NULL)
		return FALSE;

	BOOL bRetCode = FALSE;
	HANDLE hToken = NULL;

	if (GetActiveUserTokenFromSession(&hToken, dwSession) == 0)
	{
		STARTUPINFO si = { sizeof(si) };
		PROCESS_INFORMATION pi = { 0 };

		si.lpDesktop = _T("Winsta0\\Default");

		DWORD dwCreationFlag = NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE;
		LPVOID pEnvironment = NULL;
		PFN_CreateEnvironmentBlock pCreateEnvironmentBlock = NULL;
		PFN_DestroyEnvironmentBlock pDestroyEnvironmentBlock = NULL;

		HMODULE hUserEnvLib = NULL;
		hUserEnvLib = LoadLibrary(_T("userenv.dll"));
		if (NULL != hUserEnvLib)
		{
			pCreateEnvironmentBlock = (PFN_CreateEnvironmentBlock)GetProcAddress(hUserEnvLib, "CreateEnvironmentBlock");
			pDestroyEnvironmentBlock = (PFN_DestroyEnvironmentBlock)GetProcAddress(hUserEnvLib, "DestroyEnvironmentBlock");
		}

		if (NULL != pCreateEnvironmentBlock)
		{
			if (pCreateEnvironmentBlock(&pEnvironment, hToken, FALSE))
			{
				dwCreationFlag |= CREATE_UNICODE_ENVIRONMENT;
			}
			else
			{
				pEnvironment = NULL;
			}
		}

		bRetCode = CreateProcessAsUser(hToken, NULL, pszCmd, NULL, NULL, FALSE, dwCreationFlag, pEnvironment, pszCurrentDir, &si, &pi);

		if (pi.hProcess != NULL)
			CloseHandle(pi.hProcess);

		if (pi.hThread != NULL)
			CloseHandle(pi.hThread);

		if (NULL != pDestroyEnvironmentBlock)
			pDestroyEnvironmentBlock(pEnvironment);

		if (NULL != hUserEnvLib)
			FreeLibrary(hUserEnvLib);
	}

	if (hToken != NULL)
		CloseHandle(hToken);

	return bRetCode;
}

BOOL KisCreateProcess::CreateProcessAsActivedUserOnVistaBySession(
	LPTSTR pszCmd,
	LPTSTR pszCurrentDir,
	DWORD dwSession)
{
	if (pszCmd == NULL)
		return FALSE;

	BOOL bRetCode = FALSE;
	HANDLE hToken = NULL;

	if (GetActiveUserTokenOnVistaFromSession(&hToken, dwSession) == 0)
	{
		STARTUPINFO si = { sizeof(si) };
		PROCESS_INFORMATION pi = { 0 };

		si.lpDesktop = _T("Winsta0\\Default");

		DWORD dwCreationFlag = NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE;
		LPVOID pEnvironment = NULL;
		PFN_CreateEnvironmentBlock pCreateEnvironmentBlock = NULL;
		PFN_DestroyEnvironmentBlock pDestroyEnvironmentBlock = NULL;

		HMODULE hUserEnvLib = NULL;
		hUserEnvLib = LoadLibrary(_T("userenv.dll"));
		if (NULL != hUserEnvLib)
		{
			pCreateEnvironmentBlock = (PFN_CreateEnvironmentBlock)GetProcAddress(hUserEnvLib, "CreateEnvironmentBlock");
			pDestroyEnvironmentBlock = (PFN_DestroyEnvironmentBlock)GetProcAddress(hUserEnvLib, "DestroyEnvironmentBlock");
		}

		if (NULL != pCreateEnvironmentBlock)
		{
			if (pCreateEnvironmentBlock(&pEnvironment, hToken, FALSE))
			{
				dwCreationFlag |= CREATE_UNICODE_ENVIRONMENT;
			}
			else
			{
				pEnvironment = NULL;
			}
		}

		bRetCode = CreateProcessAsUser(hToken, NULL, pszCmd, NULL, NULL, FALSE, dwCreationFlag, pEnvironment, pszCurrentDir, &si, &pi);

		if (pi.hProcess != NULL)
			CloseHandle(pi.hProcess);

		if (pi.hThread != NULL)
			CloseHandle(pi.hThread);

		if (NULL != pDestroyEnvironmentBlock)
			pDestroyEnvironmentBlock(pEnvironment);

		if (NULL != hUserEnvLib)
			FreeLibrary(hUserEnvLib);
	}

	if (hToken != NULL)
		CloseHandle(hToken);

	return bRetCode;
}

BOOL KisCreateProcess::CreateAdminProcess(
	LPTSTR lpApplicationName,
	LPTSTR lpCurrentDirectory)
{
	OSVERSIONINFO version = { 0 };
	version.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	GetVersionEx(&version);

	if (version.dwMajorVersion == 6)
		return CreateProcessAsActivedUserOnVista(lpApplicationName, lpCurrentDirectory);
	else
		return CreateProcessAsActivedUser(lpApplicationName, lpCurrentDirectory);
}

BOOL  KisCreateProcess::CreateProcessUserToTillExit(
	LPTSTR lpApplicationName,
	LPTSTR lpCommandLine,
	LPTSTR lpCurrentDirectory)
{
	BOOL bRetCode = FALSE;

	TCHAR szCommandLine[2048] = { 0 };
	if (lpApplicationName)
	{
		::_tcsncpy(szCommandLine, lpApplicationName, 2047);
	}

	if (lpCommandLine)
	{
		::_tcsncat(szCommandLine, _T(" "), ::_tcslen(_T(" ")));
		::_tcsncat(szCommandLine, lpCommandLine, 1023);
	}

	bRetCode = CreateProcessAsActivedUserOnVistaTillExit(szCommandLine, lpCurrentDirectory);

	return bRetCode;
}

int KisCreateProcess::GetActiveUserToken(
	HANDLE* phToken)
{
	if (phToken == NULL)
	{
		return -1;
	}

	OSVERSIONINFO version = { 0 };
	version.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	bool bIsWIN2K = false;
	if (GetVersionEx(&version))
	{
		if (version.dwMajorVersion == 5 && version.dwMinorVersion == 0)
		{
			bIsWIN2K = true;
		}
	}

	if (!bIsWIN2K)
	{
		int nRetCode = GetActiveUserTokenFromXP(phToken);
		if (nRetCode != 0)
		{
			return GetActiveUserTokenFrom2k(phToken);
		}

		return nRetCode;
	}
	else
	{
		return GetActiveUserTokenFrom2k(phToken);
	}
}

int KisCreateProcess::GetActiveUserTokenOnVista(
	HANDLE* phToken)
{
	HANDLE hTokenUser = NULL;
	DWORD dwLen;
	int nRetCode;
	TOKEN_LINKED_TOKEN admin;
	BOOL bRetCode;

	nRetCode = GetActiveUserTokenFromXP(&hTokenUser);
	if (nRetCode != 0)
	{
		nRetCode = GetActiveUserTokenFrom2k(&hTokenUser);
	}

	if (nRetCode != 0)
	{
		return -1;
	}

	bRetCode = ::GetTokenInformation(hTokenUser, (TOKEN_INFORMATION_CLASS)TokenLinkedToken, &admin, sizeof(TOKEN_LINKED_TOKEN), &dwLen);
	if (bRetCode == FALSE)
	{
		*phToken = hTokenUser;
	}
	else
	{
		*phToken = admin.LinkedToken;
	}

	return 0;
}

bool KisCreateProcess::IsActiveUserTokenAdmin(
	HANDLE hToken)
{
	bool bResult = false;
	PSID pAdminSid = NULL;
	SID_IDENTIFIER_AUTHORITY ntAuth = SECURITY_NT_AUTHORITY;
	PTOKEN_GROUPS pGroupList = NULL;
	DWORD dwSize;

	if (!::AllocateAndInitializeSid(&ntAuth, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &pAdminSid))
	{
		goto Exit0;
	}

	if (!::GetTokenInformation(hToken, TokenGroups, 0, 0, &dwSize))
	{
		if (ERROR_INSUFFICIENT_BUFFER != ::GetLastError())
		{
			goto Exit0;
		}
	}

	pGroupList = (PTOKEN_GROUPS)::LocalAlloc(LPTR, dwSize);
	if (!::GetTokenInformation(hToken, TokenGroups, pGroupList, dwSize, &dwSize))
	{
		goto Exit0;
	}

	for (DWORD dwLoop = 0; dwLoop < pGroupList->GroupCount; dwLoop++)
	{
		if (::EqualSid(pAdminSid, pGroupList->Groups[dwLoop].Sid))
		{
			bResult = true;
			break;
		}
	}

Exit0:
	if (pGroupList)
	{
		LocalFree(pGroupList);
	}

	if (pAdminSid)
	{
		FreeSid(pAdminSid);
	}

	return bResult;
}

int KisCreateProcess::GetActiveUserTokenFromSystem(
	HANDLE* phToken)
{
	HANDLE hTokenThis = NULL;
	HANDLE hTokenDup = NULL;
	DWORD dwSessionId = 0;

	int nRetCode = GetActiveSession(dwSessionId);
	if (nRetCode != 0)
		return nRetCode;

	OpenProcessToken(GetCurrentProcess(), TOKEN_DUPLICATE | TOKEN_QUERY, &hTokenThis);
	if (hTokenThis == NULL)
		return -1;

	DuplicateTokenEx(hTokenThis, MAXIMUM_ALLOWED, NULL, SecurityIdentification, TokenPrimary, &hTokenDup);
	if (hTokenDup == NULL)
	{
		CloseHandle(hTokenThis);
		return -1;
	}

	CloseHandle(hTokenThis);
	if (SetTokenInformation(hTokenDup, TokenSessionId, &dwSessionId, sizeof(DWORD)))
	{
		*phToken = hTokenDup;
		return 0;
	}
	else
	{
		return -1;
	}

}

int KisCreateProcess::GetActiveUserTokenFromSession(
	HANDLE* phToken,
	DWORD dwSession)
{
	if (phToken == NULL)
		return -1;

	OSVERSIONINFO version = { 0 };
	version.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	bool bIsWIN2K = false;

	if (GetVersionEx(&version))
	{
		if (version.dwMajorVersion == 5 && version.dwMinorVersion == 0)
		{
			bIsWIN2K = true;
		}
	}

	if (!bIsWIN2K)
	{
		int nRetCode;

		HMODULE hWTSApi = LoadLibrary(TEXT("wtsapi32.dll"));

		PFN_WTSQueryUserToken pWTSQueryUserToken = (PFN_WTSQueryUserToken)GetProcAddress(hWTSApi, "WTSQueryUserToken");
		if (pWTSQueryUserToken == NULL)
		{
			nRetCode = (int)GetLastError();
		}
		else
		{
			BOOL bRetCode = pWTSQueryUserToken(dwSession, phToken);
			if (bRetCode == FALSE)
			{
				nRetCode = (int)GetLastError();
			}
			else
			{
				nRetCode = 0;
			}
		}

		if (nRetCode != 0)
			return GetActiveUserTokenFrom2k(phToken);

		return nRetCode;
	}
	else
	{
		return GetActiveUserTokenFrom2k(phToken);
	}
}

int KisCreateProcess::GetActiveUserTokenOnVistaFromSession(
	HANDLE* phToken,
	DWORD dwSession)
{
	HANDLE hTokenThis = NULL;
	HANDLE hTokenDup = NULL;

	OpenProcessToken(GetCurrentProcess(), TOKEN_DUPLICATE | TOKEN_QUERY, &hTokenThis);
	if (hTokenThis == NULL)
		return -1;

	DuplicateTokenEx(hTokenThis, MAXIMUM_ALLOWED, NULL, SecurityIdentification, TokenPrimary, &hTokenDup);
	if (hTokenDup == NULL)
	{
		CloseHandle(hTokenThis);
		return -1;
	}

	CloseHandle(hTokenThis);
	if (SetTokenInformation(hTokenDup, TokenSessionId, &dwSession, sizeof(DWORD)))
	{
		*phToken = hTokenDup;
		return 0;
	}
	else
	{
		CloseHandle(hTokenDup);
		return -1;
	}
}

int KisCreateProcess::GetActiveUserTokenFromXP(
	HANDLE* phToken)
{
	DWORD dwSessionId = 0;
	int nRetCode = GetActiveSessionFromXP(dwSessionId);
	if (nRetCode != 0)
		return nRetCode;

	HMODULE hWTSApi = LoadLibrary(TEXT("wtsapi32.dll"));

	PFN_WTSQueryUserToken pWTSQueryUserToken = (PFN_WTSQueryUserToken)GetProcAddress(hWTSApi, "WTSQueryUserToken");
	if (pWTSQueryUserToken == NULL)
	{
		return GetLastError();
	}
	else
	{
		BOOL bRetCode = pWTSQueryUserToken(dwSessionId, phToken);
		if (bRetCode == FALSE)
			return GetLastError();

		return 0;
	}
}

int KisCreateProcess::GetActiveUserTokenFrom2k(
	HANDLE* phToken)
{
	HANDLE hToolhelp = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hToolhelp == INVALID_HANDLE_VALUE)
		return GetLastError();

	DWORD dwProcessId = 0;
	int nRetCode = ERROR_NOT_FOUND;
	PROCESSENTRY32 ProcessEntry = { 0 };

	ProcessEntry.dwSize = sizeof(PROCESSENTRY32);
	if (Process32First(hToolhelp, &ProcessEntry))
	{
		do
		{

			if (_tcsicmp(ProcessEntry.szExeFile, _T("explorer.exe")) == 0)
			{
				dwProcessId = ProcessEntry.th32ProcessID;
				nRetCode = 0;
				break;
			}
		} while (Process32Next(hToolhelp, &ProcessEntry));
	}
	else
	{
		nRetCode = GetLastError();
	}

	CloseHandle(hToolhelp);

	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwProcessId);
	if (hProcess == NULL)
		return GetLastError();

	BOOL bRetCode = OpenProcessToken(hProcess, TOKEN_ALL_ACCESS, phToken);
	if (bRetCode == FALSE)
		nRetCode = GetLastError();

	CloseHandle(hProcess);

	return nRetCode;
}

int KisCreateProcess::GetActiveSession(
	DWORD& dwSession)
{
	PWTS_SESSION_INFO pSessionInfoArray = NULL;
	DWORD dwCount = 0;

	BOOL bRetCode = WTSEnumerateSessions(WTS_CURRENT_SERVER_HANDLE, 0, 1, &pSessionInfoArray, &dwCount);
	if (bRetCode == FALSE)
	{
		OSVERSIONINFO version = { 0 };
		version.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		if (GetVersionEx(&version))
		{
			if (version.dwMajorVersion == 6)
			{
				return (int)GetLastError();
			}
		}

		dwSession = 0;

		return 0;
	}

	int nRetCode = ERROR_NOT_FOUND;

	for (DWORD dwLoop = 0; dwLoop < dwCount; ++dwLoop)
	{
		if (pSessionInfoArray[dwLoop].State == WTSActive)
		{
			dwSession = pSessionInfoArray[dwLoop].SessionId;
			nRetCode = 0;
			break;
		}
	}

	WTSFreeMemory(pSessionInfoArray);

	return nRetCode;
}

int KisCreateProcess::GetActiveSessionFromXP(
	DWORD& dwSession)
{
	PWTS_SESSION_INFO pSessionInfoArray = NULL;
	DWORD dwCount = 0;

	BOOL bRetCode = WTSEnumerateSessions(WTS_CURRENT_SERVER_HANDLE, 0, 1, &pSessionInfoArray, &dwCount);
	if (bRetCode == FALSE)
		return 0;

	int nRetCode = ERROR_NOT_FOUND;

	for (DWORD dwLoop = 0; dwLoop < dwCount; ++dwLoop)
	{
		if (pSessionInfoArray[dwLoop].State == WTSActive)
		{
			dwSession = pSessionInfoArray[dwLoop].SessionId;
			nRetCode = 0;

			break;
		}
	}

	WTSFreeMemory(pSessionInfoArray);

	return nRetCode;
}


MyProcessMgr::MyProcessMgr(void)
{
	m_bIsWait = FALSE;
	m_dwLastError = 0;
	m_dwProcessExitCode = -12;
	m_hUserToken = NULL;
	m_dwCreationFlags = 0;
}

MyProcessMgr::~MyProcessMgr(void)
{

}

BOOL MyProcessMgr::CreateProcessAsUser(LPCTSTR lpImage, int nIsCmdLine, WORD wShow)
{
	BOOL result = FALSE;
	HANDLE explorer_token = NULL;
	PROCESS_INFORMATION pi = { 0 };
	CString strLog;

	do
	{
		DWORD session_id = WTSGetActiveConsoleSessionId();
		DWORD explorer_pid = GetProcessPID(session_id, _T("explorer.exe"));

		//m_dwLastError = GetLastError();
		////DLOG(std::string("m_dwLastError=") + std::to_string(m_dwLastError));
		//strLog.Format(_T("SESSION_ID: %x"), session_id);

		if (explorer_pid == 0) {
			break;
		}

		if (FAILED(obtain_impersonate_token(explorer_pid, explorer_token))) {

			m_dwLastError = GetLastError();
			//DLOG(std::string("m_dwLastError=") + std::to_string(m_dwLastError));
			break;
		}

		LPVOID pEnv = NULL;
		DWORD dwCreationFlags = m_dwCreationFlags;

		if (CreateEnvironmentBlock(&pEnv, explorer_token, TRUE))
		{
			dwCreationFlags |= CREATE_UNICODE_ENVIRONMENT;
		}
		else {
			pEnv = NULL;
		}

		//m_dwLastError = GetLastError();
		//DLOG(std::string("m_dwLastError=") + std::to_string(m_dwLastError));
		STARTUPINFO si = { 0 };
		si.cb = sizeof(si);
		si.wShowWindow = wShow;

		if (wShow == SW_HIDE)
		{
			//ShowLog("RP_00");
		}
		//char path_buffer[1024] = "C:\\Windows\\SysWOW64\\notepad.exe";

		if (nIsCmdLine == 0)
		{
			result = ::CreateProcessAsUser(
				explorer_token,
				lpImage,
				NULL,
				NULL,
				NULL,
				FALSE,
				dwCreationFlags,
				pEnv,
				NULL,
				&si,
				&pi
			);



			//m_dwLastError = GetLastError();
			//DLOG(std::string("m_dwLastError=") + std::to_string(m_dwLastError));
			if (result)
			{
				m_bExecuteRes = TRUE;
			}


			if (m_bIsWait)
			{
				::WaitForSingleObject(pi.hProcess, INFINITE);
			}

			::GetExitCodeProcess(pi.hProcess, &m_dwProcessExitCode);
		}

		if (nIsCmdLine == 1)
		{
			DLOG("enter CreateProcessUser");
			result = ::CreateProcessAsUser(
				explorer_token,
				NULL,
				(LPTSTR)lpImage,
				NULL,
				NULL,
				FALSE,
				dwCreationFlags,
				pEnv,
				NULL,
				&si,
				&pi
			);
			//std::string dlg;
			//dlg = GetLastError();
			//DLOG(dlg);
			m_dwLastError = GetLastError();
			//DLOG(std::string("m_dwLastError=") + std::to_string(m_dwLastError));
			DLOG(std::string("CreateProcessA errCode = ") + std::to_string(GetLastError()));

			if (m_bIsWait)
			{
				::WaitForSingleObject(pi.hProcess, INFINITE);
			}

			m_dwLastError = GetLastError();

			if (result)
			{
				m_bExecuteRes = TRUE;
			}
			::GetExitCodeProcess(pi.hProcess, &m_dwProcessExitCode);
		}


		int iResultOfCreateProcessAsUser = GetLastError();

		//DLOG(std::string("m_dwLastError=") + std::to_string(iResultOfCreateProcessAsUser));
	} while (false);

	if (explorer_token != NULL) {
		CloseHandle(explorer_token);
		explorer_token = NULL;
	}

	if (pi.hProcess != NULL) {
		CloseHandle(pi.hProcess);
		pi.hProcess = NULL;
	}

	if (pi.hThread != NULL) {
		CloseHandle(pi.hThread);
		pi.hThread = NULL;
	}

	return result;
}

void  MyProcessMgr::SetLowRun(BOOL bIsLow)
{
	if (bIsLow)
		m_dwCreationFlags |= IDLE_PRIORITY_CLASS;
	else
		m_dwCreationFlags ^= IDLE_PRIORITY_CLASS;
}

BOOL MyProcessMgr::CreateProcessAsSystem(LPTSTR lpImage)
{

	BOOL bRes = FALSE;

	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi;

	si.wShowWindow = FALSE;

	bRes = ::CreateProcess(NULL, lpImage, NULL, NULL, FALSE, CREATE_NEW_CONSOLE | m_dwCreationFlags, NULL, NULL, &si, &pi);


	if (bRes)
	{
		if (m_bIsWait)
		{
			::WaitForSingleObject(pi.hProcess, INFINITE);
		}

		::GetExitCodeProcess(pi.hProcess, &m_dwProcessExitCode);
	}
	else
	{
		m_dwLastError = GetLastError();
	}

	return bRes;
}

DWORD MyProcessMgr::GetProcessPID(DWORD dwCurrSession, CString strProcessName)
{
	DWORD result = 0;
	WTS_PROCESS_INFO* pi = NULL;
	DWORD pi_count = 0;
	CString strCurrentProcess;

	strProcessName.MakeLower();

	try
	{
		if (WTSEnumerateProcesses(NULL, 0, 1, &pi, &pi_count) && pi != NULL)
		{
			for (DWORD i = 0; i < pi_count; i++)
			{
				strCurrentProcess.Format(_T("%s"), pi[i].pProcessName);
				strCurrentProcess.MakeLower();

				if (pi[i].SessionId == dwCurrSession
					&& pi[i].pProcessName != NULL
					&& strProcessName == strCurrentProcess)
				{
					result = pi[i].ProcessId;
					break;
				}
			}

			WTSFreeMemory(pi);
			pi = NULL;
		}
	}
	catch (...)
	{
		if (pi != NULL)
			WTSFreeMemory(pi);

		result = 0;
	}

	return result;
}

BOOL MyProcessMgr::PorcessIsRun(CString strProcessName, DWORD& dwPid)
{
	HANDLE hProcessSnap = NULL;
	BOOL   bRet = FALSE;

	PROCESSENTRY32   pe32 = { 0 };


	strProcessName.MakeLower();
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hProcessSnap == INVALID_HANDLE_VALUE)
		return FALSE;

	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (Process32First(hProcessSnap, &pe32))
	{
		do
		{
			CString strExeName;
			strExeName = pe32.szExeFile;

			strExeName.MakeLower();

			if (strExeName == strProcessName)
			{
				dwPid = pe32.th32ProcessID;
				bRet = TRUE;
			}

		} while (Process32Next(hProcessSnap, &pe32));
	}

	CloseHandle(hProcessSnap);

	return bRet;
}

void MyProcessMgr::KillProcess(CString strProcessName)
{
	HANDLE hProcessSnap = NULL;
	BOOL   bRet = FALSE;

	PROCESSENTRY32   pe32 = { 0 };


	strProcessName.MakeLower();
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hProcessSnap == INVALID_HANDLE_VALUE)
		return;

	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (Process32First(hProcessSnap, &pe32))
	{
		do
		{
			CString strExeName;
			strExeName = pe32.szExeFile;

			strExeName.MakeLower();

			if (strExeName == strProcessName)
			{
				HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
				TerminateProcess(hProcess, 0);
				return;
			}

		} while (Process32Next(hProcessSnap, &pe32));
	}

	CloseHandle(hProcessSnap);
}

BOOL MyProcessMgr::IsVistaOrLater()
{
	OSVERSIONINFO osInfo;
	osInfo.dwOSVersionInfoSize = sizeof(osInfo);
	::GetVersionEx(&osInfo);

	return (0 <= ((int)(osInfo.dwMajorVersion - 6)));
}

HRESULT MyProcessMgr::obtain_impersonate_token(DWORD dwProcessId, HANDLE& hTokenObtain)
{
	HRESULT hr = S_OK;
	HANDLE hProcess = NULL;
	HANDLE hToken = NULL;
	HANDLE hDupToken = NULL;
	MY_TOKEN_MANDATORY_LABEL* pTIL = NULL;

	hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcessId);
	if (!hProcess)
	{
		hr = AtlHresultFromLastError();
		goto Exit0;
	}

	BOOL bRet = ::OpenProcessToken(hProcess, TOKEN_ALL_ACCESS, &hToken);
	if (!bRet)
	{
		hr = AtlHresultFromLastError();
		goto Exit0;
	}

	if (IsVistaOrLater())
	{
		DWORD dwBytesNeeded = 0;

		bRet = ::GetTokenInformation(
			hToken,
			(TOKEN_INFORMATION_CLASS)MYTokenIntegrityLevel,
			NULL, 0, &dwBytesNeeded
		);

		pTIL = (MY_TOKEN_MANDATORY_LABEL*)new BYTE[dwBytesNeeded];
		if (!pTIL)
		{
			hr = E_OUTOFMEMORY;
			goto Exit0;
		}

		bRet = ::GetTokenInformation(
			hToken,
			(TOKEN_INFORMATION_CLASS)MYTokenIntegrityLevel,
			pTIL, dwBytesNeeded, &dwBytesNeeded
		);
		if (!bRet || !pTIL)
		{
			hr = AtlHresultFromLastError();
			goto Exit0;
		}

		SID* pSid = static_cast<SID*>(pTIL->Label.Sid);
		if (!pSid)
		{
			hr = E_FAIL;
			goto Exit0;
		}

		if (SECURITY_MANDATORY_HIGH_RID != pSid->SubAuthority[0])
		{
			MY_TOKEN_LINKED_TOKEN linkedToken = { 0 };
			DWORD dwSize = sizeof linkedToken;

			bRet = ::GetTokenInformation(
				hToken,
				(TOKEN_INFORMATION_CLASS)MYTokenLinkedToken,
				&linkedToken,
				dwSize, &dwSize
			);
			if (!bRet)
			{
				hr = AtlHresultFromLastError();
				goto Exit0;
			}

			CloseHandle(hToken);
			hToken = linkedToken.LinkedToken;
			linkedToken.LinkedToken = NULL;
		}
	}

	bRet = ::DuplicateTokenEx(
		hToken,
		TOKEN_ALL_ACCESS,
		0,
		SecurityImpersonation,
		TokenPrimary,
		&hDupToken);
	if (!bRet)
	{
		hr = AtlHresultFromLastError();
		goto Exit0;
	}

	if (IsVistaOrLater())
	{
		{
			PTSTR szIntegritySid = _T("S-1-16-12288"); //high
			PSID  pIntegritySid = NULL;
			MY_TOKEN_MANDATORY_LABEL til = { 0 };

			bRet = ::ConvertStringSidToSid(szIntegritySid, &pIntegritySid);
			if (!bRet)
			{
				hr = AtlHresultFromLastError();
				goto Exit0;
			}

			til.Label.Attributes = SE_GROUP_INTEGRITY;
			til.Label.Sid = pIntegritySid;

			bRet = ::SetTokenInformation(hDupToken, (TOKEN_INFORMATION_CLASS)MYTokenIntegrityLevel, &til, sizeof(MY_TOKEN_MANDATORY_LABEL));
			if (!bRet)
			{
				hr = AtlHresultFromLastError();
				goto Exit0;
			}

			if (pIntegritySid)
			{
				::LocalFree((HLOCAL)pIntegritySid);
				pIntegritySid = NULL;
			}
		}
	}
Exit0:
	if (SUCCEEDED(hr))
	{
		hTokenObtain = hDupToken;
	}
	else
	{
		if (hDupToken)
		{
			CloseHandle(hDupToken);
			hDupToken = NULL;
		}
	}


	if (pTIL)
	{
		delete[] pTIL;
		pTIL = NULL;
	}

	if (hToken)
	{
		CloseHandle(hToken);
		hToken = NULL;
	}

	if (hProcess)
	{
		CloseHandle(hProcess);
		hProcess = NULL;
	}

	return hr;
}

DWORD MyProcessMgr::GetLastError()
{
	return m_dwLastError;
}

void MyProcessMgr::SetIsWait(BOOL bIsWait)
{
	m_bIsWait = bIsWait;
}

DWORD  MyProcessMgr::GetProcessExitCode()
{
	return m_dwProcessExitCode;
}

BOOL MyProcessMgr::StartCurrentUserEnv()
{
	OSVERSIONINFOEX osi = { 0 };
	osi.dwOSVersionInfoSize = sizeof(osi);
	osi.dwMajorVersion = 5;
	INT nRetCode = 0;

	if (osi.dwMajorVersion < 6)
	{
		nRetCode = KisCreateProcess::GetActiveUserToken(&m_hUserToken);
		if (nRetCode != 0)
			return FALSE;
	}
	else
	{
		nRetCode = KisCreateProcess::GetActiveUserTokenOnVista(&m_hUserToken);
		if (nRetCode != 0)
			return FALSE;
	}

	if (NULL == m_hUserToken)
	{
		return FALSE;
	}

	return ::ImpersonateLoggedOnUser(m_hUserToken);
}

void MyProcessMgr::EndCurrentUserEnv()
{
	::RevertToSelf();
}






