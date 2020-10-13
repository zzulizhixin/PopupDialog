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
	int iServerStatus = -1;	//初始化服务状态

	////如果判断客户端不存在，删除服务
	//if (FALSE == IsZhwExist())
	//{
	////DeleteUsername();//删除注册表username信息
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
		//	//打印日志
		//	std::string s = std::to_string(nError);
		//	std::string lasterr = "CreateEnvironmentBlock";
		//	lasterr += s;
		//	DLOG(lasterr);
		//}
		//DLOG("[PopupAdvert:PopupJudge]进入创建删除服务进程");
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
		////打印日志
		//long nError = GetLastError();
		//std::string s = std::to_string(nError);
		//std::string lasterr = "CreateProcessAsUser错误码";
		//lasterr += s;
		//DLOG(lasterr);
		//DLOG(ws2s(cWinDir));
		//MyProcessMgr  ProcessMgr;
		//ProcessMgr.SetIsWait(TRUE);
		//BOOL result = ProcessMgr.CreateProcessAsUser(L"H:\\Items\\zhw\\projects\\lizhixin\\Zhwpopup\\Release\\install.exe", 1);
		//std::string dlg = "[PopupAdvert:PopupJudge]服务卸载进程创建错误码：";
		//dlg += std::to_string(GetLastError());
		//DLOG(dlg);
		////::DestroyEnvironmentBlock(lpEnvironment);
		////::CloseHandle(primaryToken);
		////::CloseHandle(pi.hProcess);
		////::CloseHandle(pi.hThread);
	//	if (FALSE == result)
	//	{
	//		std::string dlg = "[PopupAdvert:PopupJudge]服务卸载失败错误码：";
	//		dlg += std::to_string(GetLastError());
	//		DLOG(dlg);
	//	}
	//	else
	//	{
	//		DLOG("[PopupAdvert:PopupJudge]客户端被卸载,服务卸载成功");
	//	}
	//	//DLOG("[PopupAdvert:PopupJudge]客户端被卸载，删除注册表username信息");
	//}

	if (FALSE == IsZhwExist())
	{
		DLOG("[PopupAdvert:PopupJudge]租号玩客户端不存在，删除注册表服务信息：");
		if (DeleteServiceReg())
		{
			DLOG("[PopupAdvert:PopupJudge]删除注册表服务信息成功");
		}
		else
		{
			DLOG("[PopupAdvert:PopupJudge]删除注册表服务信息失败");
		}
	}
	if (FALSE == CheckServerStatus(iServerStatus))	//读取到服务有曾经记录的状态
	{
		DLOG("[PopupAdvert:PopupJudge]读取服务状态失败为空，设置为0");
		iServerStatus = 0;
	}
	DLOG("[PopupAdvert:PopupJudge]读取服务状态成功");

	switch (iServerStatus)//服务当前状态
	{
	case NORMAL_STATE:	//正常进行验证的状态
		result = NormalCheck();
		break;
	case WAIT_12_HOUR:	//等待12小时重新下拉数据的状态
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
	//更新Url数据到本地
	DLOG("[PopupAdvert:PopupJudge]更新Url数据到本地");
	if (FALSE == WriteUrlToFile())
	{
		std::string dlg = "[PopupAdvert:PopupJudge]更新Url数据下载到本地失败，错误码：";
		dlg += std::to_string(GetLastError());
		DLOG(dlg);
		return FALSE;
	}
	DLOG("[PopupAdvert:PopupJudge]更新Url数据到本地成功");

	//获取客户端未打开时长
	DLOG("[PopupAdvert:PopupJudge]获取客户端未打开时长");
	time_t close_time_long;
	if (FALSE == GetCloseTimeLong(close_time_long))
	{
		DLOG("[PopupAdvert:PopupJudge]获取客户端未打开时长失败");
		return FALSE;
	}
	std::string strLimitTime = to_string(close_time_long);
	std::string dlg = "[PopupAdvert:PopupJudge]客户端未启动时长（秒）:";
	dlg += strLimitTime;
	DLOG(dlg);
	DLOG("[PopupAdvert:PopupJudge]获取客户端未打开时长成功");

	//读取本地Json数据到结构体
	DLOG("[PopupAdvert:PopupJudge]读取本地Json数据到结构体");
	if (FALSE == ReadPopupJsonMessage(popup_message_, json_path_.c_str()))
	{
		std::string log = "[PopupAdvert:PopupJudge]读取本地Json数据到结构体失败,错误码：";
		log += std::to_string(GetLastError());
		DLOG(log);
		return FALSE;
	}
	DLOG("[PopupAdvert:PopupJudge]读取本地Json数据到结构体成功");

	//判断客户端未启动时长是否达到阈值来启动对话框进程
	DLOG("[PopupAdvert:PopupJudge]判断客户端未启动时长是否达到阈值");
	if (CheckTimeOut(close_time_long, popup_message_))//时间达到阈值
	{
		DLOG("[PopupAdvert:PopupJudge]时间达到阈值");

		//验证后台是否返回为可弹窗
		DLOG("[PopupAdvert:PopupJudge]验证后台是否返回为可弹窗code数值");
		if (CheckMessagePop(popup_message_))//为可弹窗的时段
		{
			DLOG("[PopupAdvert:PopupJudge]后台为可弹窗code1");
			Sleep(1000 * 60 * 2);	//等待两分钟之后进行弹窗
			DLOG("[PopupAdvert:PopupJudge] 开始GUIDProc");
			StartGUIProc obj(bin_path_);//打开弹窗进程
			obj.Run();
			std::string server_status = "0";	//状态0为正常请求服务状态
			WriteServerStatus(server_status);//把当前服务状态记录到注册表
			return TRUE;
		}
		else//为不可弹窗时段
		{
			DLOG("[PopupAdvert:PopupJudge]后台为不可弹窗code0");
			if (WriteLastAskServerTime())//记录当前时间
			{
				DLOG("[PopupAdvert:PopupJudge]记录当前时间到注册表last_ask_server");
				std::string server_status = "1";	//状态1为等待12小时重新请求服务
				WriteServerStatus(server_status);//把当前服务状态记录到注册表
				return FALSE;
			}
			return FALSE;
		}
	}
	return FALSE;
}

BOOL PopupAdvert::WriteUrlToFile()
{
	DLOG("进入WriteUrlToFile函数");
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
		dlg = "[PopupAdvert:WriteUrlToFile]进入请求url，请求信息：";
		dlg += url;
		DLOG(dlg);
		strJson = get_http.RequestJsonInfo(url);
	}
	else
	{
		DLOG("user_id=NULL");
		std::string dlg;
		dlg = "[PopupAdvert:WriteUrlToFile]进入请求url，请求信息：";
		dlg += url;
		DLOG(dlg);
		strJson = get_http.RequestJsonInfo(url);
	}

	if (!strJson.empty())
	{
		DLOG("[PopupAdvert:PopupJudge]进入服务进程主函数WriteJsonToFile");
		std::string dlg;
		dlg = "[PopupAdvert:WriteUrlToFile]获取请求url，返回信息：";
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
	DLOG("[PopupAdvert:PopupJudge]上次操作为 0:用户关闭弹窗或者 1:用户打开链接");
	DLOG("[PopupAdvert:PopupJudge]验证上次操作时间距离现在是否小于A天（verify_time）");
	std::string strLimitTime = to_string(click_time_long);
	std::string dlg = "[PopupAdvert:PopupJudge]上次操作距离现在的时长（秒）:";
	dlg += strLimitTime;
	DLOG(dlg);
	if (limit_time < click_time_long)//如果上次点击的时间距离现在大于后台下发的A天
	{
		DLOG("[PopupAdvert:PopupJudge]进入大于A天的逻辑");
		if (FALSE == Popup())
		{
			DLOG("[PopupAdvert:PopupJudge]打开弹窗失败");
			return FALSE;
		}
		DLOG("[PopupAdvert:PopupJudge]记录当前服务状态为0正常等待状态");
		if (FALSE == WriteServerStatus("0"))
		{
			std::string dlg = "[PopupAdvert:PopupJudge]当前逻辑状态写入注册表失败,错误码";
			dlg += to_string(GetLastError());
			DLOG(dlg);
			return FALSE;
		}
		DLOG("[PopupAdvert:PopupJudge]当前逻辑状态写入注册表成功");
		return TRUE;
	}
	else//如果上次点击时间距离现在小于后台下发的A天
	{
		DLOG("[PopupAdvert:PopupJudge]进入小于A天的逻辑");
		DLOG("[PopupAdvert:PopupJudge]记录当前服务状态为0正常等待状态");
		if (FALSE == WriteServerStatus("0"))
		{
			std::string dlg = "[PopupAdvert:PopupJudge]当前逻辑状态写入注册表失败,错误码";
			dlg += to_string(GetLastError());
			DLOG(dlg);
			return FALSE;
		}
		DLOG("[PopupAdvert:PopupJudge]当前逻辑状态写入注册表成功");
		return TRUE;
	}
}

BOOL PopupAdvert::NormalCheck()
{
	BOOL result = FALSE;
	DLOG("[PopupAdvert:PopupJudge]验证服务当前状态server_status为0正常判断服务流程的状态");

	time_t click_time_long = time(NULL);	//上次用户进行了点击操作距离现在的时间
	int click_type = -1;//上次用户点击的类型 0关闭 1打开链接 2 不感兴趣

	/*如果进行过弹窗操作*/
	if (CheckUserOperation(click_time_long, click_type))
	{
		DLOG("[PopupAdvert:PopupJudge]进行过弹窗操作，注册表有click_time,click_type数据");
		//读取后本地Json数据到结构体
		if (FALSE == ReadPopupJsonMessage(popup_message_, json_path_.c_str()))
		{
			DLOG("[PopupAdvert:PopupJudge]读取本地Json数据失败");
			return result;
		}
		DLOG("[PopupAdvert:PopupJudge]读取本地Json数据成功");

		time_t limit_time = atol(popup_message_.verify_limit.c_str()) * 60 * 60; //后台下发的重新进行启动时长验证判断的时间长度A天
		std::string strLimitTime = to_string(limit_time);
		std::string dlg = "[PopupAdvert:PopupJudge]获取本地重新进行时长判断的维度verify_time:";
		dlg += strLimitTime;
		DLOG(dlg);

		DLOG("[PopupAdvert:PopupJudge]进入判断上次用户操作");
		//判断上次用户弹窗操作类型
		switch (click_type)
		{
		case CLOSE_WND:/*用户关闭弹窗*/	case OPEN_URL:/*用户打开链接*/
			result = CheckClickTime(limit_time, click_time_long);
			break;
		case NO_INTEREST://用户不感兴趣
			DLOG("[PopupAdvert:PopupJudge]上次操作为2用户不感兴趣");
			DLOG("[PopupAdvert:PopupJudge]验证上次操作时间距离现在是否小于A+14天（verify_time）");
			limit_time += 14 * 24 * 60 * 60; //加上14天
			result = CheckClickTime(limit_time, click_time_long);
			break;
		}
	}
	/*第一次弹窗*/
	else
	{
		DLOG("[PopupAdvert:PopupJudge]进入第一次弹窗逻辑");
		if (FALSE == Popup())
		{
			DLOG("[PopupAdvert:PopupJudge]打开弹窗失败");
			return FALSE;
		}
		DLOG("[PopupAdvert:PopupJudge]记录当前服务状态为0正常等待状态");
		if (FALSE == WriteServerStatus("0"))
		{
			std::string dlg = "[PopupAdvert:PopupJudge]当前逻辑状态写入注册表失败,错误码";
			dlg += to_string(GetLastError());
			DLOG(dlg);
			return FALSE;
		}
		DLOG("[PopupAdvert:PopupJudge]当前逻辑状态写入注册表成功");
	}
	return result;
}

//BOOL PopupAdvert::NextTimeCheck()
//{
//	DLOG("[PopupAdvert:PopupJudge]进入等待12小时重新下拉数据的逻辑");
//	time_t ask_server_long = NULL;	//上次请求服务的时间距离现在的时长
//	if (FALSE == CheckLastAskServerTime(ask_server_long))
//	{
//		DLOG("[PopupAdvert:PopupJudge]获取上次请求服务时间距离现在的时长失败");
//		return FALSE;
//	}
//	else
//	{
//		DLOG("[PopupAdvert:PopupJudge]获取上次请求服务时间距离现在的时长成功时长:");
//		if (ask_server_long > 12 * 60 * 60) //如果时间差大于12小时
//		{
//			DLOG("[PopupAdvert:PopupJudge]获取上次请求服务时间距离现在的时长大于12小时");
//			if (FALSE == Popup())
//			{
//				DLOG("[PopupAdvert:PopupJudge]打开弹窗失败");
//				return FALSE;
//			}
//			DLOG("[PopupAdvert:PopupJudge]打开弹窗成功");
//			return TRUE;
//		}
//		else //如果时间差小于12小时
//		{
//			DLOG("[PopupAdvert:PopupJudge]获取上次请求服务时间距离现在的时长小于12小时，返回等待");
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
	HKEY hKey;		//注册表获取句柄
	LONG lRet = RegOpenKeyExW(
		HKEY_LOCAL_MACHINE,
		_T("SOFTWARE\\WOW6432Node\\daofeng\\zuhaowan\\client"),
		REG_OPTION_OPEN_LINK,
		KEY_ALL_ACCESS,
		&hKey
	);

	if (lRet != ERROR_SUCCESS)
	{
		DLOG("[PopupAdvert:CheckZhwExist]打开注册表SOFTWARE\\WOW6432Node\\daofeng\\zuhaowan\\client失败");
		return result;
	}
	if (ERROR_SUCCESS == RegDeleteValue(hKey, L"username"))
	{
		DLOG("[PopupAdvert:CheckZhwExist]租号玩不存在删除用户username信息");
		result = TRUE;
	}
	DLOG("[PopupAdvert:CheckZhwExist]删除注册表SOFTWARE\\WOW6432Node\\daofeng\\zuhaowan\\client\\username失败");

	CloseHandle(hKey);
	return result;
}

BOOL PopupAdvert::IsZhwExist()
{
	BOOL result = FALSE;
	HKEY hKey;		//注册表获取句柄
	LONG lRet = RegOpenKeyExW(
		HKEY_LOCAL_MACHINE,
		_T("SOFTWARE\\WOW6432Node\\daofeng\\zuhaowan"),
		REG_OPTION_OPEN_LINK,
		KEY_ALL_ACCESS,
		&hKey
	);
	if (lRet != ERROR_SUCCESS)
	{
		DLOG("[PopupAdvert:CheckZhwExist]打开注册表SOFTWARE\\WOW6432Node\\daofeng\\zuhaowan失败");
		return result;
	}

	//获取客户端路径
	TCHAR tchClientPath[64];
	DWORD dwSize;
	dwSize = sizeof(tchClientPath);
	lRet = RegQueryValueEx(hKey, _T("main"), NULL, NULL, (LPBYTE)tchClientPath, &dwSize);
	if (lRet != ERROR_SUCCESS)
	{
		DLOG("[PopupAdvert:CheckZhwExist]打开注册表SOFTWARE\\WOW6432Node\\daofeng\\zuhaowan\\main客户端路径失败");
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
	HKEY hKey;		//注册表获取句柄
	LONG lRet = RegOpenKeyExW(
		HKEY_LOCAL_MACHINE,
		_T("SYSTEM\\CurrentControlSet\\Services\\PopupWindowsService"),
		REG_OPTION_OPEN_LINK,
		KEY_ALL_ACCESS,
		&hKey
	);

	if (lRet != ERROR_SUCCESS)
	{
		DLOG("[PopupAdvert:CheckZhwExist]打开注册表SYSTEM\\CurrentControlSet\\Services\\PopupWindowsService失败");
		return result;
	}
	if (ERROR_SUCCESS == RegDeleteTree(hKey, NULL))
	{
		DLOG("[PopupAdvert:CheckZhwExist]删除服务注册表项");
		result = TRUE;
	}
	else
	{
		DLOG("[PopupAdvert:CheckZhwExist]删除注册表SYSTEM\\CurrentControlSet\\Services\\PopupWindowsService失败");
	}

	CloseHandle(hKey);
	return result;
}

LPCTSTR PopupAdvert::GetJsonFilePath()
{
	TCHAR szPath[MAX_PATH] = _T("");			//Json所在文件夹路径
	static TCHAR szJsonFile[MAX_PATH] = _T("");	//Json文件地址
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
	BOOL result = FALSE;	//返回结果
	BOOL bCircle = TRUE;	//是否继续循环标识
	HANDLE hTimer;	//12小时重新验证定时器

	hTimer = CreateWaitableTimer(NULL, FALSE, NULL);	//创建定时器
	const int nTimerUnitsPerSecond = 10000000;
	LARGE_INTEGER li;
	li.QuadPart = -(1 * nTimerUnitsPerSecond);//SetTimer后一秒开始执行第一次

	if (NULL != hTimer)
	{
		SetWaitableTimer(hTimer, &li, 1000 * /*60 * 60 **/ 12, NULL, NULL, FALSE);//每隔12小时执行一次
		do
		{
			if (WAIT_OBJECT_0 == WaitForSingleObject(hTimer, INFINITE))
			{
				DLOG("[PopupAdvert:PopupJudge]进入等待12小时重新下拉数据的逻辑");
				time_t ask_server_long = NULL;	//上次请求服务的时间距离现在的时长
				if (FALSE == CheckLastAskServerTime(ask_server_long))
				{
					DLOG("[PopupAdvert:PopupJudge]获取上次请求服务时间距离现在的时长失败");
					return result;
				}
				else
				{
					DLOG("[PopupAdvert:PopupJudge]获取上次请求服务时间距离现在的时长成功时长:");
					if (ask_server_long > 12 * 60 * 60) //如果时间差大于12小时
					{
						DLOG("[PopupAdvert:PopupJudge]获取上次请求服务时间距离现在的时长大于12小时");
						if (FALSE == pThis->Popup())
						{
							DLOG("[PopupAdvert:PopupJudge]打开弹窗失败");
						}
						else
						{
							DLOG("[PopupAdvert:PopupJudge]打开弹窗成功");
							result = TRUE;
							bCircle = FALSE;	//跳出循环
							return result;
						}
					}
					else //如果时间差小于12小时
					{
						DLOG("[PopupAdvert:PopupJudge]获取上次请求服务时间距离现在的时长小于12小时，返回等待");
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


