#include "cJSON.h"
#include "ReadJson.h"
#include <ShlObj.h>
#include <tchar.h>
#include <codecvt>
#include "cdHttpClient.h"

#pragma comment(lib, "urlmon.lib")


std::string utf8_to_string(const std::string& str)
{
	int nwLen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
	wchar_t* pwBuf = new wchar_t[nwLen + 1];
	memset(pwBuf, 0, nwLen * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), pwBuf, nwLen);
	int nLen = WideCharToMultiByte(CP_ACP, 0, pwBuf, -1, NULL, NULL, NULL, NULL);
	char* pBuf = new char[nLen + 1];
	memset(pBuf, 0, nLen + 1);
	WideCharToMultiByte(CP_ACP, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);
	std::string retStr = pBuf;
	delete[]pBuf;
	delete[]pwBuf;
	pBuf = NULL;
	pwBuf = NULL;
	return retStr;
}

BOOL ReadPopupJsonMessage(PopupMessage& popup_message, LPCTSTR file_name)
{
	std::ifstream fi;
	fi.open(file_name, std::ios::in);
	std::string content = std::string((std::istreambuf_iterator<char>(fi)), std::istreambuf_iterator<char>());
	char logstring[MAX_PATH] = {"[project1]"};
	if (content.empty())
	{
		return FALSE;
	}
	fi.close();

	//Json文件格式
	//{
	//	"code":	1,
	//		"message":	"广告信息",
	//		"popout_data":	{
	//			"type":	"1",
	//				"id":	"4",
	//				"link":	"http://develop.zuhaowan.com/ahzhw2018/Client/addPopAd.html",
	//				"title":	"通用弹窗",
	//				"image":	"http://pic.zuhaowan.com/images/gg_img/2019-11-14/5dccc15a33069.jpg"
	//	},
	//	"verify_limit":	"2"
	//}

	//解析文件
	cJSON* json;
	json = cJSON_Parse((const char*)content.c_str());

	cJSON* arrayItem = cJSON_GetObjectItem(json, "code");
	popup_message.code = arrayItem->valueint;
	arrayItem = cJSON_GetObjectItem(json, "message");
	popup_message.message = utf8_to_string(arrayItem->valuestring);

	arrayItem = cJSON_GetObjectItem(json, "popout_data");
	cJSON* pData = cJSON_GetObjectItem(arrayItem, "type");
	popup_message.popout_data.type = pData->valuestring;
	pData = cJSON_GetObjectItem(arrayItem, "id");
	popup_message.popout_data.id = pData->valuestring;
	pData = cJSON_GetObjectItem(arrayItem, "link");
	popup_message.popout_data.link = pData->valuestring;
	pData = cJSON_GetObjectItem(arrayItem, "title");
	popup_message.popout_data.title = utf8_to_string(pData->valuestring);
	pData = cJSON_GetObjectItem(arrayItem, "image");
	popup_message.popout_data.image = pData->valuestring;
	lstrcatA(logstring, popup_message.popout_data.image.c_str());
	OutputDebugStringA(logstring);

	arrayItem = cJSON_GetObjectItem(json, "verify_limit");
	popup_message.verify_limit = arrayItem->valuestring;

	arrayItem = cJSON_GetObjectItem(json, "start_up_limit");
	popup_message.start_up_limit = arrayItem->valuestring;

	cJSON_Delete(json); //释放内存
	return TRUE;
}

BOOL  CheckFolderExist(std::string& strPath)
{
	WIN32_FIND_DATA  wfd;
	bool rValue = false;

	size_t len = strPath.length();
	if (NULL == len)
	{
		return NULL;
	}
	int nmlen = MultiByteToWideChar(CP_ACP, 0, strPath.c_str(), len + 1, NULL, 0);
	wchar_t* buffer = new wchar_t[nmlen];
	MultiByteToWideChar(CP_ACP, 0, strPath.c_str(), len + 1, buffer, nmlen);

	HANDLE hFind = FindFirstFile(buffer, &wfd);
	if ((hFind != INVALID_HANDLE_VALUE) && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		rValue = true;
	}
	FindClose(hFind);
	return rValue;
}

std::wstring s2ws(const std::string& str)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converterX;

	return converterX.from_bytes(str);
}

std::string ws2s(const std::wstring& wstr)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converterX;

	return converterX.to_bytes(wstr);
}

std::string DownLoadPircture(PopupMessage popup_message, TCHAR *szPath)
{
	std::wstring path = szPath;
	std::wstring logstring = L"[project1]";
	//获取url地址进行字符串转换
	OutputDebugStringA("[project1]DownLoadPircture function");
	size_t len = popup_message.popout_data.image.length();
	if (NULL == len)
	{
		OutputDebugStringA("[project1]image.length  == 0");
		return NULL;
	}

	OutputDebugStringA("[project1]image.length  != 0");

	int nmlen = MultiByteToWideChar(CP_ACP, 0, popup_message.popout_data.image.c_str(), len + 1, NULL, 0);
	wchar_t* buffer = new wchar_t[nmlen];
	MultiByteToWideChar(CP_ACP, 0, popup_message.popout_data.image.c_str(), len + 1, buffer, nmlen);

	logstring += buffer;
	OutputDebugStringW(logstring.c_str());

	//截取图片文件名部分保存
	std::string strPictureName;
	int pos = popup_message.popout_data.image.find_last_of('/');
	strPictureName = popup_message.popout_data.image.substr(pos + 1);

	auto lp = s2ws(strPictureName);
	path += _T("\\");
	path += lp;

	logstring += path;
	OutputDebugStringW(logstring.c_str());
	std::string response;
	BOOL hr;
	hr = RequestForData(popup_message.popout_data.image.c_str(), response);

	//HRESULT hr = URLDownloadToFile(NULL, buffer, path.data(), NULL, NULL);
	//int err = GetLastError();
	//wchar_t ch[20];
	//_stprintf_s(ch, L"%d", err);
	//OutputDebugStringW(ch);

	delete buffer;
	//if (S_OK == hr)
	//{
	//	OutputDebugStringA("[project1]url download to file done");
	//	return ws2s(path);
	//}
	//else
	//{
	//	DbgInfoA("[project1]url download to file failed, err = %d", GetLastError());
	//	return NULL;
	//}
	if ( hr)
	{
		std::string spath = ws2s(path);
		FILE* fp = NULL;
		fopen_s(&fp, spath.c_str(), "ab+");
		size_t nWrite = fwrite(response.c_str(), response.size(), 1, fp);
		fclose(fp);

		DbgInfoA("[project1]url download to file done, size = %d", response.size());
		return ws2s(path);
	}
	else
	{
		DbgInfoA("[project1]url download to file failed, err = %d", GetLastError());
		return NULL;
	}

}

BOOL	RequestForData(const char* lpszURL, std::string& response, HWND hWnd)
{
	BOOL		bRet = FALSE;
	DWORD		error = 0;
	myCHttpClient* pClient = new myCHttpClient(hWnd);
	std::string url;
	url = lpszURL;
	try
	{
		if ("" == url)
		{
			throw 1;
		}
		pClient->SetDebug(false);
		error = pClient->Get(url, response);
		if (CURLE_OK == error)
		{
			bRet = TRUE;
		}
	}

	catch (int e)
	{
		DbgInfoA("[project1]...[%d]RequestForUpdate except, exception code = %d", __LINE__, e);
	}
	catch (...)
	{
		DbgInfoA("[project1]...[%d]Unknown exception", __LINE__);
	}

	delete pClient;
	return bRet;
}

BOOL PostUserData(const char* lpszURL,  std::string& user_data, std::string& response, HWND hWnd)
{
	BOOL		bRet = FALSE;
	DWORD		error = 0;
	myCHttpClient* pClient = new myCHttpClient(hWnd);
	std::string url;
	url = lpszURL;
	std::string UserData;
	UserData = user_data;
	try
	{
		if (NULL == lpszURL)
		{
			throw 1;
		}
		pClient->SetDebug(false);
		error = pClient->Post(url, UserData, response);
		if (CURLE_OK == error)
		{
			bRet = TRUE;
		}
	}

	catch (int e)
	{
		DbgInfoA("[project1]...[%d]RequestForUpdate except, exception code = %d", __LINE__, e);
	}
	catch (...)
	{
		DbgInfoA("[project1]...[%d]Unknown exception", __LINE__);
	}

	delete pClient;
	return bRet;
}

void DbgInfoA(const char* strOutputString, ...)
{
	va_list vlArgs = NULL;
	if (NULL == strOutputString)
	{
		return;
	}
	va_start(vlArgs, strOutputString);
	size_t nLen = _vscprintf(strOutputString, vlArgs) + 1;
	char* strBuffer = new char[nLen + 1];
	_vsnprintf_s(strBuffer, nLen, nLen, strOutputString, vlArgs);
	va_end(vlArgs);
	lstrcatA(strBuffer, "\n");
	OutputDebugStringA(strBuffer);
	delete[] strBuffer;
}
void DbgInfoW(const wchar_t* strOutputString, ...)
{
	va_list vlArgs = NULL;
	if (NULL == strOutputString)
	{
		return;
	}
	va_start(vlArgs, strOutputString);
	size_t nLen = _vscwprintf(strOutputString, vlArgs) + 1;
	wchar_t* strBuffer = new wchar_t[nLen];
	_vsnwprintf_s(strBuffer, nLen, nLen, strOutputString, vlArgs);
	va_end(vlArgs);
	lstrcatW(strBuffer, L"\n");
	OutputDebugStringW(strBuffer);
	delete[] strBuffer;
}
