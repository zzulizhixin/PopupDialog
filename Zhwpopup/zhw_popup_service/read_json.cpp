#pragma once
#include "read_json.h"
#include <ShlObj.h>
#include "string_convert.h"
#include "log.h"
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
	std::string path = ws2s(file_name);
	DLOG(path);
	fi.open(file_name, std::ios::in);
	std::string content = std::string((std::istreambuf_iterator<char>(fi)), std::istreambuf_iterator<char>());
	if (content.empty())
	{
		std::string log = "读取文件失败,错误码：";
		log +=	std::to_string(GetLastError());
		DLOG(log);
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

	if (1 == popup_message.code)
	{
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

		arrayItem = cJSON_GetObjectItem(json, "verify_limit");
		popup_message.verify_limit = arrayItem->valuestring;

		arrayItem = cJSON_GetObjectItem(json, "start_up_limit");
		popup_message.start_up_limit = arrayItem->valuestring;
	}

	cJSON_Delete(json); //释放内存
	return TRUE;
}

//BOOL  CheckFolderExist(std::string& strPath)
//{
//	WIN32_FIND_DATA  wfd;
//	bool rValue = false;
//
//	size_t len = strPath.length();
//	if (NULL == len)
//	{
//		return NULL;
//	}
//	int nmlen = MultiByteToWideChar(CP_ACP, 0, strPath.c_str(), len + 1, NULL, 0);
//	wchar_t* buffer = new wchar_t[nmlen];
//	MultiByteToWideChar(CP_ACP, 0, strPath.c_str(), len + 1, buffer, nmlen);
//
//	HANDLE hFind = FindFirstFile(buffer, &wfd);
//	if ((hFind != INVALID_HANDLE_VALUE) && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
//	{
//		rValue = true;
//	}
//	FindClose(hFind);
//	return rValue;
//}
//
//std::string DownLoadPircture(PopupMessage popup_message, TCHAR *szPath)
//{
//	std::wstring path = szPath;
//	//获取url地址进行字符串转换
//	size_t len = popup_message.popout_data.image.length();
//	if (NULL == len)
//	{
//		return NULL;
//	}
//	int nmlen = MultiByteToWideChar(CP_ACP, 0, popup_message.popout_data.image.c_str(), len + 1, NULL, 0);
//	wchar_t* buffer = new wchar_t[nmlen];
//	MultiByteToWideChar(CP_ACP, 0, popup_message.popout_data.image.c_str(), len + 1, buffer, nmlen);
//
//	//截取图片文件名部分保存
//	std::string strPictureName;
//	int pos = popup_message.popout_data.image.find_last_of('/');
//	strPictureName = popup_message.popout_data.image.substr(pos + 1);
//
//	auto lp = s2ws(strPictureName);
//	//lstrcat(szPath, _T("\\"));
//	path += _T("\\");
//
//	path += lp;
//
//	HRESULT hr = URLDownloadToFile(NULL, buffer, path.data(), NULL, NULL);
//	delete buffer;
//	if (S_OK == hr)
//	{
//		return strPictureName;
//	}
//	else
//	{
//		return NULL;
//	}
//
//}
//

