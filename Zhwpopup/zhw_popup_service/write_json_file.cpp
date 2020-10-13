#pragma once
#include "stdafx.h"
#include <shlobj.h>
#include "log.h"
#include "cJSON.h"
#include "write_json_file.h"

char* ConvertLPWSTRToLPSTR(LPWSTR lpwszStrIn)
{
	LPSTR pszOut = NULL;
	if (lpwszStrIn != NULL)
	{
		int nInputStrLen = wcslen(lpwszStrIn);
		int nOutputStrLen = WideCharToMultiByte(CP_ACP, 0, lpwszStrIn, nInputStrLen, NULL, 0, 0, 0) + 2;
		pszOut = new char[nOutputStrLen];
		if (pszOut)
		{
			memset(pszOut, 0x00, nOutputStrLen);
			WideCharToMultiByte(CP_ACP, 0, lpwszStrIn, nInputStrLen, pszOut, nOutputStrLen, 0, 0);
		}
	}
	return pszOut;
}

BOOL WriteJsonToFile(std::string strjson)
{
	cJSON* json = cJSON_Parse(strjson.c_str());
	if (json == NULL)
	{
		DLOG("WriteJsonToFile: json NULL");
		return FALSE;
	}
	//将传入的JSON结构转化为字符串 
	char* buf = NULL;
	buf = cJSON_Print(json);

	TCHAR szPath[MAX_PATH];	//C:\ProgramData路径
	if (SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, 0, szPath) != S_OK) //获取C:\ProgramData路径
	{
		DLOG("WriteJson: SHGetFolderPath FALSE");
		return FALSE;
	}
	//在C:\ProgramData路径下创建\\service_database文件夹
	lstrcat(szPath, _T("\\service_database"));
	CreateDirectory(szPath, NULL);
	int err = GetLastError();
	if (ERROR_PATH_NOT_FOUND == err)
	{
		DLOG("Create dir failed");
		return FALSE;
	}
	DLOG("create dir success");
	lstrcat(szPath, _T("\\popup_info.json"));

	FILE* fp;
	if (fopen_s(&fp, ConvertLPWSTRToLPSTR(szPath), "w") != 0)
	{
		DLOG("WriteJson: fopen_s FALSE");
		return FALSE;
	}
	DLOG("WriteJson: fwrite ");
	fwrite(buf, strlen(buf), 1, fp);
	fclose(fp);
	free(buf);
	cJSON_Delete(json);
	return TRUE;
}



