#pragma once
#include <Windows.h>
#include <atlstr.h>
#include <TlHelp32.h>
#include <WtsApi32.h>


//////////////////////////////////////////////////////////////////////////

typedef BOOL(STDMETHODCALLTYPE FAR* PFN_CreateEnvironmentBlock)(
	LPVOID* lpEnvironment,
	HANDLE hToken,
	BOOL bInherit);

typedef BOOL(STDMETHODCALLTYPE FAR* PFN_DestroyEnvironmentBlock)(
	LPVOID lpEnvironment);

typedef BOOL(WINAPI* PFN_WTSQueryUserToken)(
	ULONG SessionId,
	PHANDLE phToken);

//////////////////////////////////////////////////////////////////////////

class KisCreateProcess
{
public:
	static BOOL CreateProcess(LPTSTR psCmdLine);

	static BOOL CreateProcessAsActivedUser(LPTSTR psCmd, LPTSTR psCurrentDir);

	static BOOL CreateAdminProcessAsActivedUser(LPTSTR psCmd, LPTSTR psCurrentDir);

	static BOOL CreateProcessAsActivedUserOnVista(LPTSTR psCmd, LPTSTR psCurrentDir);

	BOOL CreateProcessAsActivedUserOnVistaTillExit(LPTSTR psCmd, LPTSTR psCurrentDir);

	BOOL CreateProcessAsActivedUserBySession(LPTSTR pszCmd, LPTSTR pszCurrentDir, DWORD dwSession);

	BOOL CreateProcessAsActivedUserOnVistaBySession(LPTSTR pszCmd, LPTSTR pszCurrentDir,DWORD dwSession);

	static BOOL CreateAdminProcess(LPTSTR lpApplicationName, LPTSTR lpCurrentDirectory);

	BOOL CreateProcessUserToTillExit(LPTSTR lpApplicationName, LPTSTR lpCommandLine, LPTSTR lpCurrentDirectory);

	static int GetActiveUserToken(HANDLE* phToken);

	static int GetActiveUserTokenOnVista(HANDLE* phToken);

	static bool IsActiveUserTokenAdmin(HANDLE hToken);

	static int GetActiveUserTokenFromSystem(HANDLE* phToken);

	int GetActiveUserTokenFromSession(HANDLE* phToken, DWORD dwSession);

	int GetActiveUserTokenOnVistaFromSession(HANDLE* phToken, DWORD dwSession);

	static int GetActiveUserTokenFromXP(HANDLE* phToken);

	static int GetActiveUserTokenFrom2k(HANDLE* phToken);

	static int GetActiveSession(DWORD& dwSession);

	static int GetActiveSessionFromXP(DWORD& dwSession);

};

//////////////////////////////////////////////////////////////////////////

typedef enum _MY_TOKEN_INFORMATION_CLASS {
	MYTokenElevationType = 18, // MaxTokenInfoClass
	MYTokenLinkedToken,
	MYTokenElevation,
	MYTokenHasRestrictions,
	MYTokenAccessInformation,
	MYTokenVirtualizationAllowed,
	MYTokenVirtualizationEnabled,
	MYTokenIntegrityLevel,
	MYTokenUIAccess,
	MYTokenMandatoryPolicy,
	MYTokenLogonSid,
	//MaxTokenInfoClass  // MaxTokenInfoClass should always be the last enum
} MY_TOKEN_INFORMATION_CLASS, * PMY_TOKEN_INFORMATION_CLASS;

typedef struct _MY_TOKEN_MANDATORY_LABEL {
	SID_AND_ATTRIBUTES Label;
} MY_TOKEN_MANDATORY_LABEL, * PMY_TOKEN_MANDATORY_LABEL;

typedef struct _MY_TOKEN_LINKED_TOKEN {
	HANDLE LinkedToken;
} MY_TOKEN_LINKED_TOKEN, * PMY_TOKEN_LINKED_TOKEN;


class MyProcessMgr
{
private:
	BOOL  m_bIsWait;
	BOOL  m_bExecuteRes;
	DWORD m_dwLastError;
	DWORD m_dwProcessExitCode;
	DWORD m_dwCreationFlags;

	HANDLE m_hUserToken;

	BOOL IsVistaOrLater();
	HRESULT obtain_impersonate_token(DWORD dwProcessId, HANDLE& hTokenObtain);
public:
	BOOL CreateProcessAsUser(LPCTSTR lpImage, int nIsCmdLine = 0, WORD wShow = SW_SHOW);
	BOOL CreateProcessAsSystem(LPTSTR lpImage);
	BOOL PorcessIsRun(CString strProcessName, DWORD& dwPid);
	void SetIsWait(BOOL bIsWait);
	void SetLowRun(BOOL bIsLow);

	DWORD GetLastError();
	DWORD  GetProcessExitCode();

	BOOL StartCurrentUserEnv();
	void EndCurrentUserEnv();

	DWORD GetProcessPID(DWORD dwCurrSession, CString strProcessName);
	void KillProcess(CString strProcessName);
	MyProcessMgr(void);
	~MyProcessMgr(void);
};

