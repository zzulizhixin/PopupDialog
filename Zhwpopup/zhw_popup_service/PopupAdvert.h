#pragma once
#include "stdafx.h"
#include "read_json.h"

class PopupAdvert
{
	enum SERVER_STATE
	{
		NORMAL_STATE,
		WAIT_12_HOUR
	};

	enum CLICK_TYPE
	{
		CLOSE_WND,
		OPEN_URL,
		NO_INTEREST
	};
public:
	PopupAdvert();
	~PopupAdvert();

public:

	/** @PopupJudge							�ж��Ƿ���е����߼�����
	*   @return TRUE								�����ɹ�
	*   @return FLASE   							����ʧ��
	*/
	BOOL PopupJudge();

private:
	/** @GetJsonFilePath						��ȡJson�ļ������ַ
*   @return LPCTSTR						Json�ļ����Ե�ַ
*   @return NULL   							��ȡʧ��
*/
	LPCTSTR GetJsonFilePath();
	/** @Popup										��������������ж���Ҫ������ִ�д˺������е���
*   @return TRUE								�����ɹ�
*   @return FLASE   							����ʧ��
*/
	BOOL Popup();

	/** @GetFilePath								��ȡ��ǰexe�����ļ��е�ַ
	*    @wsFullName[in]						exe���Ե�ַ
	*   @return wstring							·����ַ
	*/
	std::wstring GetFilePath(const std::wstring& wsFullName);

	/** @GetAppPath						��ȡ��ǰexe���Ե�ַ
	*   @return wstring							��ַ
	*/
	std::wstring GetAppPath();

	/** @WriteUrlToFile						����URL���ݵ�����Json�ļ�
	*   @return TRUE								����ɹ�
	*   @return FLASE   							����ʧ��
	*/
	BOOL WriteUrlToFile();


	/** @CheckClickTime						ʱ����
	*   @return TRUE								����ɹ�
	*   @return FLASE   							����ʧ��
	*/
	BOOL CheckClickTime(time_t limit_time, time_t click_time_long);

	/** @NormalCheck							�����߼����
	*   @return TRUE								�����ɹ�
	*   @return FLASE   							����ʧ��
	*/
	BOOL NormalCheck();

	/** @NextTimeCheck						�´ε���ʱ�����߳�
	*   @return TRUE								�����ɹ�
	*   @return FLASE   							����ʧ��
	*/
	static DWORD WINAPI NextTimeCheck(PVOID pvParam);

	/** @IsFileExist								·���ļ��Ƿ����
	*    @csFile[in]									·��
	*   @return TRUE								����
	*   @return FLASE   							������
	*/
	BOOL IsFileExist(const std::wstring& csFile);

	/** @DeletUsername						ж��ɾ��ע���username
	*   @return TRUE								ж�سɹ�
	*   @return FLASE   							ж��ʧ��
	*/
	BOOL DeleteUsername();

	/** @IsZhwExist						   �����ͻ����Ƿ�����ڵ�����
	*   @return TRUE								����
	*   @return FLASE   							������
	*/
	BOOL IsZhwExist();
	/** @DeletUsername						ж�ط���ע�����Ϣ
*   @return TRUE								ж�سɹ�
*   @return FLASE   							ж��ʧ��
*/
	BOOL DeleteServiceReg();

private:
	std::wstring bin_path_;	//pop.exe��ַ
	std::wstring json_path_;//Json�ļ���ַ
	std::string str_url_;		//url��ַ
	PopupMessage popup_message_;	//��ȡJson�ļ�����Ľṹ��


};