#pragma once
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string>
//�ύ�û����ݽṹ��
typedef struct UserMessage
{
	std::string id;											//���id
	std::string action;										//�û����еĲ�����0���رմ��ڣ�1�������ӣ� 2��������Ȥ
	std::string device;									//�豸��ʶ
	std::string system;									//����ϵͳ
	std::string user_id;									//�ͻ��˵�¼���û�id
}USER_MESSAGE, * LPUSER_MESSAGE;

//��ȡ�û�MAC��ַ����
wchar_t* GetMacAddress();

//��ȡ�û�����ϵͳ����
std::string GetSysVersion();

//��ȡ�û�ID
BOOL GetUserId(std::string& user_id);

