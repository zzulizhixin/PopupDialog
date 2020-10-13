#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>
#include "cJSON.h"

#ifdef _DEBUG
#ifdef _UNICODE
#define DbgInfo(x, ...) DbgInfoW(x, __VA_ARGS__)
#else
#define DbgInfo(x, ...) DbgInfoA(x, __VA_ARGS__)
#endif
#else
#ifdef _UNICODE
#define DbgInfo(x, ...) DbgInfoW(x, __VA_ARGS__)
#else
#define DbgInfo(x, ...) DbgInfoA(x, __VA_ARGS__)
#endif
#endif

//��̨��������·��洢�ṹ��
typedef struct PopupMessage
{
	int code;													//�Ƿ񵯴���ʶ��1����0����
	std::string message;								//�����Ϣ
	struct PopoutData
	{
		std::string type;										//�������Ӧ���ͣ�1���򿪿ͻ��ˣ�2�������ӣ�link��
		std::string id;											//���id
		std::string link;											//�������url
		std::string title;										//��������
		std::string image;										//���ͼƬ��ʾ��url
	}popout_data;																	//������ݽṹ��
	std::string verify_limit;								//�ͻ������½���δ����ʱ����֤�Ļ�������A
	std::string start_up_limit;						//�ͻ���δ����ʱ��
}POPUP_MESSAGE, * LPOPUP_MESSAGE;


/** @ReadPopupJsonMessage						��ȡJson���ݽ��н�������ṹ����
*   @popup_message[out]								���ݴ���Ľṹ����
*   @file_name[in]											��ȡjson���ļ�·��
*   @return TRUE												��ȡ�ļ��ɹ�
*   @return FLASE   											��ȡ�ļ�ʧ��*/
BOOL ReadPopupJsonMessage(PopupMessage &popup_message, LPCTSTR file_name);

//�����ַ���ת��
std::string utf8_to_string(const std::string& str);

/** @DownLoadPircture									���ؽṹ��image��urlͼƬ
*   @popup_message[in]								������ݽṹ��
*   @picture_name[out]									image�ļ���
*   @return string												�ɹ�����image�ļ���
*   @return NULL   											���ɹ�����NULL*/
std::string DownLoadPircture(PopupMessage popup_message, TCHAR *szPath);

/** @CheckFolderExist									����ļ���·���Ƿ����
*   @strPath[in]													·����
*   @return TRUE											    ����
*   @return FLASE   											������*/
BOOL  CheckFolderExist(std::string& strPath);

/** @RequestForData										��ȡURL����
*   @lpszURL[in]												URL��ַ
*   @response[out]											����
*   @return TRUE											    ��ȡ�ɹ�
*   @return FLASE   											��ȡʧ��*/
BOOL RequestForData(const char* lpszURL, std::string& response, HWND hWnd = NULL);

/** @PostUserData											�ύ�û���Ϣ
*   @lpszURL[in]												URL��ַ
*   @response[out]											���ؽ��
*   @user_data[out]											�ϴ����û�����
*   @return TRUE											    �ϴ��ɹ�
*   @return FLASE   											�ϴ�ʧ��*/
BOOL PostUserData(const char* lpszURL, std::string& response, std::string& user_data, HWND hWnd = NULL);

//wchar_tתstring
std::string ws2s(const std::wstring& wstr);

//stringתwchar_t
std::wstring s2ws(const std::string& str);

//�����ڵ��԰汾�����������Ϣ��dbgview
void DbgInfoA(const char* strOutputString, ...);
void DbgInfoW(const wchar_t* strOutputString, ...);
