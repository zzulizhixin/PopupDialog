#pragma once
#include "stdafx.h"

//�ַ���ת��TCHARתconst char*
char* ConvertLPWSTRToLPSTR(LPWSTR lpwszStrIn);

/** @WriteJsonToFile							��ȡ���ַ��������Json�ļ���ϵͳĿ¼C:\ProgramData��
*   @strjson[in]							    string���͵�json�ַ���
*   @return TRUE							�ļ�����ɹ�
*   @return FLASE   						�ļ�����ʧ��
*/
BOOL WriteJsonToFile(std:: string strjson);
