#pragma once
#include "stdafx.h"

//字符串转换TCHAR转const char*
char* ConvertLPWSTRToLPSTR(LPWSTR lpwszStrIn);

/** @WriteJsonToFile							读取的字符串保存成Json文件到系统目录C:\ProgramData下
*   @strjson[in]							    string类型的json字符串
*   @return TRUE							文件保存成功
*   @return FLASE   						文件保存失败
*/
BOOL WriteJsonToFile(std:: string strjson);
