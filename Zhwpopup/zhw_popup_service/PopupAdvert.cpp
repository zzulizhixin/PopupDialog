#pragma once
#include "read_json.h"
#include "log.h"
#include "check_time.h"
#include "start_gui.h"
#include <ShlObj.h>
#include "GetHttp.h"
#include "PopupAdvert.h"
#include "string_convert.h"
#include "open_process.h"


//std::wstring PopupAdvert::json_path_ = PopupAdvert::GetJsonFilePath();
//std::wstring PopupAdvert::bin_path_ = PopupAdvert::GetAppPath() + L"\\pop.exe";
//std::wstring PopupAdvert::str_url_ = L"http://www1.zuhaowan.com:98/InterfaceApi/ClientPop/getPopout";
HANDLE GetCurrentUserToken()
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

	//��ӡ��־
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
	//��ӡ��־
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

PopupAdvert::PopupAdvert() :popup_message_()
{
	json_path_ = GetJsonFilePath();
	bin_path_ = GetAppPath() + L"\\pop.exe";
	str_url_ = "http://www.zuhaowan.com/InterfaceApi/ClientPop/getPopout";
}

PopupAdvert::~PopupAdvert()
{
}

BOOL PopupAdvert::PopupJudge()
{
	BOOL result = FALSE;
	int iServerStatus = -1;	//��ʼ������״̬

	////����жϿͻ��˲����ڣ�ɾ������
	//if (FALSE == IsZhwExist())
	//{
	////DeleteUsername();//ɾ��ע���username��Ϣ
		//STARTUPINFO si = { sizeof(si) };
		//PROCESS_INFORMATION pi;
		//LPTSTR cWinDir = new TCHAR[MAX_PATH];
		//GetCurrentDirectory(MAX_PATH, cWinDir);
		//LPCTSTR strInstallName = L"\\install.exe";
		//wcscat_s(cWinDir, 256, strInstallName);
		//HANDLE primaryToken = GetCurrentUserToken();
		//if (primaryToken == 0)
		//{
		//	return FALSE;
		//}
		//void* lpEnvironment = NULL;
		//BOOL resultEnv = ::CreateEnvironmentBlock(&lpEnvironment, primaryToken, FALSE);
		//if (resultEnv == 0)
		//{
		//	long nError = GetLastError();
		//	//��ӡ��־
		//	std::string s = std::to_string(nError);
		//	std::string lasterr = "CreateEnvironmentBlock";
		//	lasterr += s;
		//	DLOG(lasterr);
		//}
		//DLOG("[PopupAdvert:PopupJudge]���봴��ɾ���������");
		//BOOL result = CreateProcessAsUser(
		//	primaryToken, 
		//	0, 
		//	_T("J:\\Project\\Zhwpopup\\Release\\install.exe"),
		//	NULL, 
		//	NULL, 
		//	FALSE, 
		//	NORMAL_PRIORITY_CLASS | CREATE_UNICODE_ENVIRONMENT, 
		//	NULL, 
		//	0, 
		//	&si,
		//	&pi);
		//Sleep(5000);
		////��ӡ��־
		//long nError = GetLastError();
		//std::string s = std::to_string(nError);
		//std::string lasterr = "CreateProcessAsUser������";
		//lasterr += s;
		//DLOG(lasterr);
		//DLOG(ws2s(cWinDir));
		//MyProcessMgr  ProcessMgr;
		//ProcessMgr.SetIsWait(TRUE);
		//BOOL result = ProcessMgr.CreateProcessAsUser(L"H:\\Items\\zhw\\projects\\lizhixin\\Zhwpopup\\Release\\install.exe", 1);
		//std::string dlg = "[PopupAdvert:PopupJudge]����ж�ؽ��̴��������룺";
		//dlg += std::to_string(GetLastError());
		//DLOG(dlg);
		////::DestroyEnvironmentBlock(lpEnvironment);
		////::CloseHandle(primaryToken);
		////::CloseHandle(pi.hProcess);
		////::CloseHandle(pi.hThread);
	//	if (FALSE == result)
	//	{
	//		std::string dlg = "[PopupAdvert:PopupJudge]����ж��ʧ�ܴ����룺";
	//		dlg += std::to_string(GetLastError());
	//		DLOG(dlg);
	//	}
	//	else
	//	{
	//		DLOG("[PopupAdvert:PopupJudge]�ͻ��˱�ж��,����ж�سɹ�");
	//	}
	//	//DLOG("[PopupAdvert:PopupJudge]�ͻ��˱�ж�أ�ɾ��ע���username��Ϣ");
	//}

	if (FALSE == IsZhwExist())
	{
		DLOG("[PopupAdvert:PopupJudge]�����ͻ��˲����ڣ�ɾ��ע��������Ϣ��");
		if (DeleteServiceReg())
		{
			DLOG("[PopupAdvert:PopupJudge]ɾ��ע��������Ϣ�ɹ�");
		}
		else
		{
			DLOG("[PopupAdvert:PopupJudge]ɾ��ע��������Ϣʧ��");
		}
	}
	if (FALSE == CheckServerStatus(iServerStatus))	//��ȡ��������������¼��״̬
	{
		DLOG("[PopupAdvert:PopupJudge]��ȡ����״̬ʧ��Ϊ�գ�����Ϊ0");
		iServerStatus = 0;
	}
	DLOG("[PopupAdvert:PopupJudge]��ȡ����״̬�ɹ�");

	switch (iServerStatus)//����ǰ״̬
	{
	case NORMAL_STATE:	//����������֤��״̬
		result = NormalCheck();
		break;
	case WAIT_12_HOUR:	//�ȴ�12Сʱ�����������ݵ�״̬
		//result = NextTimeCheck();
		DWORD dwThreadID;
		HANDLE hThread = NULL;

		hThread = CreateThread(NULL, NULL, NextTimeCheck, this, NULL, &dwThreadID);
		if (hThread != NULL)
		{
			result = TRUE;
			CloseHandle(hThread);
		}
	}
	return result;
}

BOOL PopupAdvert::Popup()
{
	//����Url���ݵ�����
	DLOG("[PopupAdvert:PopupJudge]����Url���ݵ�����");
	if (FALSE == WriteUrlToFile())
	{
		std::string dlg = "[PopupAdvert:PopupJudge]����Url�������ص�����ʧ�ܣ������룺";
		dlg += std::to_string(GetLastError());
		DLOG(dlg);
		return FALSE;
	}
	DLOG("[PopupAdvert:PopupJudge]����Url���ݵ����سɹ�");

	//��ȡ�ͻ���δ��ʱ��
	DLOG("[PopupAdvert:PopupJudge]��ȡ�ͻ���δ��ʱ��");
	time_t close_time_long;
	if (FALSE == GetCloseTimeLong(close_time_long))
	{
		DLOG("[PopupAdvert:PopupJudge]��ȡ�ͻ���δ��ʱ��ʧ��");
		return FALSE;
	}
	std::string strLimitTime = to_string(close_time_long);
	std::string dlg = "[PopupAdvert:PopupJudge]�ͻ���δ����ʱ�����룩:";
	dlg += strLimitTime;
	DLOG(dlg);
	DLOG("[PopupAdvert:PopupJudge]��ȡ�ͻ���δ��ʱ���ɹ�");

	//��ȡ����Json���ݵ��ṹ��
	DLOG("[PopupAdvert:PopupJudge]��ȡ����Json���ݵ��ṹ��");
	if (FALSE == ReadPopupJsonMessage(popup_message_, json_path_.c_str()))
	{
		std::string log = "[PopupAdvert:PopupJudge]��ȡ����Json���ݵ��ṹ��ʧ��,�����룺";
		log += std::to_string(GetLastError());
		DLOG(log);
		return FALSE;
	}
	DLOG("[PopupAdvert:PopupJudge]��ȡ����Json���ݵ��ṹ��ɹ�");

	//�жϿͻ���δ����ʱ���Ƿ�ﵽ��ֵ�������Ի������
	DLOG("[PopupAdvert:PopupJudge]�жϿͻ���δ����ʱ���Ƿ�ﵽ��ֵ");
	if (CheckTimeOut(close_time_long, popup_message_))//ʱ��ﵽ��ֵ
	{
		DLOG("[PopupAdvert:PopupJudge]ʱ��ﵽ��ֵ");

		//��֤��̨�Ƿ񷵻�Ϊ�ɵ���
		DLOG("[PopupAdvert:PopupJudge]��֤��̨�Ƿ񷵻�Ϊ�ɵ���code��ֵ");
		if (CheckMessagePop(popup_message_))//Ϊ�ɵ�����ʱ��
		{
			DLOG("[PopupAdvert:PopupJudge]��̨Ϊ�ɵ���code1");
			Sleep(1000 * 60 * 2);	//�ȴ�������֮����е���
			DLOG("[PopupAdvert:PopupJudge] ��ʼGUIDProc");
			StartGUIProc obj(bin_path_);//�򿪵�������
			obj.Run();
			std::string server_status = "0";	//״̬0Ϊ�����������״̬
			WriteServerStatus(server_status);//�ѵ�ǰ����״̬��¼��ע���
			return TRUE;
		}
		else//Ϊ���ɵ���ʱ��
		{
			DLOG("[PopupAdvert:PopupJudge]��̨Ϊ���ɵ���code0");
			if (WriteLastAskServerTime())//��¼��ǰʱ��
			{
				DLOG("[PopupAdvert:PopupJudge]��¼��ǰʱ�䵽ע���last_ask_server");
				std::string server_status = "1";	//״̬1Ϊ�ȴ�12Сʱ�����������
				WriteServerStatus(server_status);//�ѵ�ǰ����״̬��¼��ע���
				return FALSE;
			}
			return FALSE;
		}
	}
	return FALSE;
}

BOOL PopupAdvert::WriteUrlToFile()
{
	DLOG("����WriteUrlToFile����");
	CWininetHttp get_http;
	std::string user_id = "";
	std::string strJson = "";
	std::string post_message = "user_id=";
	std::string url;
	url = str_url_;
	url += "?";
	if (GetUserId(user_id))
	{
		DLOG(user_id);
		url += post_message;
		url += user_id;
		std::string dlg;
		dlg = "[PopupAdvert:WriteUrlToFile]��������url��������Ϣ��";
		dlg += url;
		DLOG(dlg);
		strJson = get_http.RequestJsonInfo(url);
	}
	else
	{
		DLOG("user_id=NULL");
		std::string dlg;
		dlg = "[PopupAdvert:WriteUrlToFile]��������url��������Ϣ��";
		dlg += url;
		DLOG(dlg);
		strJson = get_http.RequestJsonInfo(url);
	}

	if (!strJson.empty())
	{
		DLOG("[PopupAdvert:PopupJudge]����������������WriteJsonToFile");
		std::string dlg;
		dlg = "[PopupAdvert:WriteUrlToFile]��ȡ����url��������Ϣ��";
		dlg += strJson;
		DLOG(dlg);

		if (WriteJsonToFile(strJson))
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL PopupAdvert::CheckClickTime(time_t limit_time, time_t click_time_long)
{
	DLOG("[PopupAdvert:PopupJudge]�ϴβ���Ϊ 0:�û��رյ������� 1:�û�������");
	DLOG("[PopupAdvert:PopupJudge]��֤�ϴβ���ʱ����������Ƿ�С��A�죨verify_time��");
	std::string strLimitTime = to_string(click_time_long);
	std::string dlg = "[PopupAdvert:PopupJudge]�ϴβ����������ڵ�ʱ�����룩:";
	dlg += strLimitTime;
	DLOG(dlg);
	if (limit_time < click_time_long)//����ϴε����ʱ��������ڴ��ں�̨�·���A��
	{
		DLOG("[PopupAdvert:PopupJudge]�������A����߼�");
		if (FALSE == Popup())
		{
			DLOG("[PopupAdvert:PopupJudge]�򿪵���ʧ��");
			return FALSE;
		}
		DLOG("[PopupAdvert:PopupJudge]��¼��ǰ����״̬Ϊ0�����ȴ�״̬");
		if (FALSE == WriteServerStatus("0"))
		{
			std::string dlg = "[PopupAdvert:PopupJudge]��ǰ�߼�״̬д��ע���ʧ��,������";
			dlg += to_string(GetLastError());
			DLOG(dlg);
			return FALSE;
		}
		DLOG("[PopupAdvert:PopupJudge]��ǰ�߼�״̬д��ע���ɹ�");
		return TRUE;
	}
	else//����ϴε��ʱ���������С�ں�̨�·���A��
	{
		DLOG("[PopupAdvert:PopupJudge]����С��A����߼�");
		DLOG("[PopupAdvert:PopupJudge]��¼��ǰ����״̬Ϊ0�����ȴ�״̬");
		if (FALSE == WriteServerStatus("0"))
		{
			std::string dlg = "[PopupAdvert:PopupJudge]��ǰ�߼�״̬д��ע���ʧ��,������";
			dlg += to_string(GetLastError());
			DLOG(dlg);
			return FALSE;
		}
		DLOG("[PopupAdvert:PopupJudge]��ǰ�߼�״̬д��ע���ɹ�");
		return TRUE;
	}
}

BOOL PopupAdvert::NormalCheck()
{
	BOOL result = FALSE;
	DLOG("[PopupAdvert:PopupJudge]��֤����ǰ״̬server_statusΪ0�����жϷ������̵�״̬");

	time_t click_time_long = time(NULL);	//�ϴ��û������˵�������������ڵ�ʱ��
	int click_type = -1;//�ϴ��û���������� 0�ر� 1������ 2 ������Ȥ

	/*������й���������*/
	if (CheckUserOperation(click_time_long, click_type))
	{
		DLOG("[PopupAdvert:PopupJudge]���й�����������ע�����click_time,click_type����");
		//��ȡ�󱾵�Json���ݵ��ṹ��
		if (FALSE == ReadPopupJsonMessage(popup_message_, json_path_.c_str()))
		{
			DLOG("[PopupAdvert:PopupJudge]��ȡ����Json����ʧ��");
			return result;
		}
		DLOG("[PopupAdvert:PopupJudge]��ȡ����Json���ݳɹ�");

		time_t limit_time = atol(popup_message_.verify_limit.c_str()) * 60 * 60; //��̨�·������½�������ʱ����֤�жϵ�ʱ�䳤��A��
		std::string strLimitTime = to_string(limit_time);
		std::string dlg = "[PopupAdvert:PopupJudge]��ȡ�������½���ʱ���жϵ�ά��verify_time:";
		dlg += strLimitTime;
		DLOG(dlg);

		DLOG("[PopupAdvert:PopupJudge]�����ж��ϴ��û�����");
		//�ж��ϴ��û�������������
		switch (click_type)
		{
		case CLOSE_WND:/*�û��رյ���*/	case OPEN_URL:/*�û�������*/
			result = CheckClickTime(limit_time, click_time_long);
			break;
		case NO_INTEREST://�û�������Ȥ
			DLOG("[PopupAdvert:PopupJudge]�ϴβ���Ϊ2�û�������Ȥ");
			DLOG("[PopupAdvert:PopupJudge]��֤�ϴβ���ʱ����������Ƿ�С��A+14�죨verify_time��");
			limit_time += 14 * 24 * 60 * 60; //����14��
			result = CheckClickTime(limit_time, click_time_long);
			break;
		}
	}
	/*��һ�ε���*/
	else
	{
		DLOG("[PopupAdvert:PopupJudge]�����һ�ε����߼�");
		if (FALSE == Popup())
		{
			DLOG("[PopupAdvert:PopupJudge]�򿪵���ʧ��");
			return FALSE;
		}
		DLOG("[PopupAdvert:PopupJudge]��¼��ǰ����״̬Ϊ0�����ȴ�״̬");
		if (FALSE == WriteServerStatus("0"))
		{
			std::string dlg = "[PopupAdvert:PopupJudge]��ǰ�߼�״̬д��ע���ʧ��,������";
			dlg += to_string(GetLastError());
			DLOG(dlg);
			return FALSE;
		}
		DLOG("[PopupAdvert:PopupJudge]��ǰ�߼�״̬д��ע���ɹ�");
	}
	return result;
}

//BOOL PopupAdvert::NextTimeCheck()
//{
//	DLOG("[PopupAdvert:PopupJudge]����ȴ�12Сʱ�����������ݵ��߼�");
//	time_t ask_server_long = NULL;	//�ϴ���������ʱ��������ڵ�ʱ��
//	if (FALSE == CheckLastAskServerTime(ask_server_long))
//	{
//		DLOG("[PopupAdvert:PopupJudge]��ȡ�ϴ��������ʱ��������ڵ�ʱ��ʧ��");
//		return FALSE;
//	}
//	else
//	{
//		DLOG("[PopupAdvert:PopupJudge]��ȡ�ϴ��������ʱ��������ڵ�ʱ���ɹ�ʱ��:");
//		if (ask_server_long > 12 * 60 * 60) //���ʱ������12Сʱ
//		{
//			DLOG("[PopupAdvert:PopupJudge]��ȡ�ϴ��������ʱ��������ڵ�ʱ������12Сʱ");
//			if (FALSE == Popup())
//			{
//				DLOG("[PopupAdvert:PopupJudge]�򿪵���ʧ��");
//				return FALSE;
//			}
//			DLOG("[PopupAdvert:PopupJudge]�򿪵����ɹ�");
//			return TRUE;
//		}
//		else //���ʱ���С��12Сʱ
//		{
//			DLOG("[PopupAdvert:PopupJudge]��ȡ�ϴ��������ʱ��������ڵ�ʱ��С��12Сʱ�����صȴ�");
//			return FALSE;
//		}
//	}
//}

BOOL PopupAdvert::IsFileExist(const std::wstring& csFile)
{
	DWORD dwAttrib = GetFileAttributes(csFile.c_str());
	return INVALID_FILE_ATTRIBUTES != dwAttrib && 0 == (dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
}

BOOL PopupAdvert::DeleteUsername()
{
	BOOL result = FALSE;
	HKEY hKey;		//ע����ȡ���
	LONG lRet = RegOpenKeyExW(
		HKEY_LOCAL_MACHINE,
		_T("SOFTWARE\\WOW6432Node\\daofeng\\zuhaowan\\client"),
		REG_OPTION_OPEN_LINK,
		KEY_ALL_ACCESS,
		&hKey
	);

	if (lRet != ERROR_SUCCESS)
	{
		DLOG("[PopupAdvert:CheckZhwExist]��ע���SOFTWARE\\WOW6432Node\\daofeng\\zuhaowan\\clientʧ��");
		return result;
	}
	if (ERROR_SUCCESS == RegDeleteValue(hKey, L"username"))
	{
		DLOG("[PopupAdvert:CheckZhwExist]����治����ɾ���û�username��Ϣ");
		result = TRUE;
	}
	DLOG("[PopupAdvert:CheckZhwExist]ɾ��ע���SOFTWARE\\WOW6432Node\\daofeng\\zuhaowan\\client\\usernameʧ��");

	CloseHandle(hKey);
	return result;
}

BOOL PopupAdvert::IsZhwExist()
{
	BOOL result = FALSE;
	HKEY hKey;		//ע����ȡ���
	LONG lRet = RegOpenKeyExW(
		HKEY_LOCAL_MACHINE,
		_T("SOFTWARE\\WOW6432Node\\daofeng\\zuhaowan"),
		REG_OPTION_OPEN_LINK,
		KEY_ALL_ACCESS,
		&hKey
	);
	if (lRet != ERROR_SUCCESS)
	{
		DLOG("[PopupAdvert:CheckZhwExist]��ע���SOFTWARE\\WOW6432Node\\daofeng\\zuhaowanʧ��");
		return result;
	}

	//��ȡ�ͻ���·��
	TCHAR tchClientPath[64];
	DWORD dwSize;
	dwSize = sizeof(tchClientPath);
	lRet = RegQueryValueEx(hKey, _T("main"), NULL, NULL, (LPBYTE)tchClientPath, &dwSize);
	if (lRet != ERROR_SUCCESS)
	{
		DLOG("[PopupAdvert:CheckZhwExist]��ע���SOFTWARE\\WOW6432Node\\daofeng\\zuhaowan\\main�ͻ���·��ʧ��");
		return result;
	}
	if (IsFileExist(tchClientPath))
	{
		result = TRUE;
	}
	CloseHandle(hKey);
	return result;
}

BOOL PopupAdvert::DeleteServiceReg()
{
	BOOL result = FALSE;
	HKEY hKey;		//ע����ȡ���
	LONG lRet = RegOpenKeyExW(
		HKEY_LOCAL_MACHINE,
		_T("SYSTEM\\CurrentControlSet\\Services\\PopupWindowsService"),
		REG_OPTION_OPEN_LINK,
		KEY_ALL_ACCESS,
		&hKey
	);

	if (lRet != ERROR_SUCCESS)
	{
		DLOG("[PopupAdvert:CheckZhwExist]��ע���SYSTEM\\CurrentControlSet\\Services\\PopupWindowsServiceʧ��");
		return result;
	}
	if (ERROR_SUCCESS == RegDeleteTree(hKey, NULL))
	{
		DLOG("[PopupAdvert:CheckZhwExist]ɾ������ע�����");
		result = TRUE;
	}
	else
	{
		DLOG("[PopupAdvert:CheckZhwExist]ɾ��ע���SYSTEM\\CurrentControlSet\\Services\\PopupWindowsServiceʧ��");
	}

	CloseHandle(hKey);
	return result;
}

LPCTSTR PopupAdvert::GetJsonFilePath()
{
	TCHAR szPath[MAX_PATH] = _T("");			//Json�����ļ���·��
	static TCHAR szJsonFile[MAX_PATH] = _T("");	//Json�ļ���ַ
	if (S_OK == SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, 0, szPath))
	{
		lstrcat(szPath, _T("\\service_database"));
		lstrcat(szJsonFile, szPath);
		lstrcat(szJsonFile, _T("\\popup_info.json"));
		return szJsonFile;
	}
	return NULL;
}

std::wstring  PopupAdvert::GetFilePath(const std::wstring& wsFullName)
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

std::wstring PopupAdvert::GetAppPath()
{
	wchar_t lpszFileName[MAX_PATH] = { 0 };
	::GetModuleFileName(NULL, lpszFileName, MAX_PATH);
	std::wstring strFullName = lpszFileName;
	return GetFilePath(strFullName);
}

DWORD WINAPI PopupAdvert::NextTimeCheck(PVOID pvParam)
{
	PopupAdvert* pThis = (PopupAdvert*)pvParam;
	BOOL result = FALSE;	//���ؽ��
	BOOL bCircle = TRUE;	//�Ƿ����ѭ����ʶ
	HANDLE hTimer;	//12Сʱ������֤��ʱ��

	hTimer = CreateWaitableTimer(NULL, FALSE, NULL);	//������ʱ��
	const int nTimerUnitsPerSecond = 10000000;
	LARGE_INTEGER li;
	li.QuadPart = -(1 * nTimerUnitsPerSecond);//SetTimer��һ�뿪ʼִ�е�һ��

	if (NULL != hTimer)
	{
		SetWaitableTimer(hTimer, &li, 1000 * /*60 * 60 **/ 12, NULL, NULL, FALSE);//ÿ��12Сʱִ��һ��
		do
		{
			if (WAIT_OBJECT_0 == WaitForSingleObject(hTimer, INFINITE))
			{
				DLOG("[PopupAdvert:PopupJudge]����ȴ�12Сʱ�����������ݵ��߼�");
				time_t ask_server_long = NULL;	//�ϴ���������ʱ��������ڵ�ʱ��
				if (FALSE == CheckLastAskServerTime(ask_server_long))
				{
					DLOG("[PopupAdvert:PopupJudge]��ȡ�ϴ��������ʱ��������ڵ�ʱ��ʧ��");
					return result;
				}
				else
				{
					DLOG("[PopupAdvert:PopupJudge]��ȡ�ϴ��������ʱ��������ڵ�ʱ���ɹ�ʱ��:");
					if (ask_server_long > 12 * 60 * 60) //���ʱ������12Сʱ
					{
						DLOG("[PopupAdvert:PopupJudge]��ȡ�ϴ��������ʱ��������ڵ�ʱ������12Сʱ");
						if (FALSE == pThis->Popup())
						{
							DLOG("[PopupAdvert:PopupJudge]�򿪵���ʧ��");
						}
						else
						{
							DLOG("[PopupAdvert:PopupJudge]�򿪵����ɹ�");
							result = TRUE;
							bCircle = FALSE;	//����ѭ��
							return result;
						}
					}
					else //���ʱ���С��12Сʱ
					{
						DLOG("[PopupAdvert:PopupJudge]��ȡ�ϴ��������ʱ��������ڵ�ʱ��С��12Сʱ�����صȴ�");
					}
				}
			}
		} while (bCircle);
		if (hTimer != NULL)
		{
			CloseHandle(hTimer);
		}
	}
	else 
	{
		return result;
	}
}


