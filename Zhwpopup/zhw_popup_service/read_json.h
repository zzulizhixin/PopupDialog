#pragma once
#include "stdafx.h"
#include "cJSON.h"

//��̨��������·��洢�ṹ��
typedef struct PopupMessage
{
	int code ;													//�Ƿ񵯴���ʶ��1����0����
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
}POPUP_MESSAGE, *LPOPUP_MESSAGE;


//�ύ�û����ݽṹ��
typedef struct UserMessage
{
	std::string id;											//���id
	std::string action;										//�û����еĲ�����0���رմ��ڣ�1�������ӣ� 2��������Ȥ
	std::string device;									//�豸��ʶ
	std::string system;									//����ϵͳ
	std::string user_id;									//�ͻ��˵�¼���û�id
}USER_MESSAGE, *LPUSER_MESSAGE;


/** @ReadPopupJsonMessage						��ȡJson���ݽ��н�������ṹ����
*   @popup_message[out]								���ݴ���Ľṹ����
*   @file_name[in]											��ȡjson���ļ�·��
*   @return TRUE												��ȡ�ļ��ɹ�
*   @return FLASE   											��ȡ�ļ�ʧ��
*/
BOOL ReadPopupJsonMessage(PopupMessage &popup_message, LPCTSTR file_name);

///** @DownLoadPircture									���ؽṹ��image��urlͼƬ
//*   @popup_message[in]									������ݽṹ��
//*   @picture_name[out]									image�ļ���
//*   @return string												�ɹ�����image�ļ���
//*   @return NULL   											���ɹ�����NULL*/
//std::string DownLoadPircture(PopupMessage popup_message, TCHAR *szPath);
//
///** @CheckFolderExist									���·���Ƿ����
//*   @strPath[in]													·��
//*   @return TRUE												����
//*   @return FLASE   											������*/
//BOOL CheckFolderExist(std::string& strPath);

//�����ַ���ת��
std::string utf8_to_string(const std::string& str);
