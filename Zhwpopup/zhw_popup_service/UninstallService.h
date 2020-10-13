#pragma once
#include<Windows.h>
#include <tchar.h>
#include <string.h>

BOOL IsZhwExist();
BOOL IsFileExist(const std::wstring& csFile);
BOOL DeleteServiceRegedit();