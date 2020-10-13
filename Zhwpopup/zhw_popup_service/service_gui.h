#pragma once
#include "stdafx.h"
#include "winsvc.h"
class ServiceWithGUI
{
public:
	ServiceWithGUI();
	~ServiceWithGUI();

	/** @StartServiceWithGUI				��GUI����
	*   @bin_path[in]								Ҫ�򿪵�GUI����·��
	*   @return TRUE								GUI���̴����ɹ�
	*   @return FLASE   							GUI���̴���ʧ��	*/
	BOOL StartServiceWithGUI(/*const std::wstring& bin_path*/);
	//static LPCTSTR GetJsonFilePath();

private:

	/*	* @ServiceMain								�������������
	*   @argc[in]										���ݸ�main()�����������в�������
	*   @argv[in]								        ����������ַ���������ָ������*/
	VOID static WINAPI ServiceMain(DWORD argc, LPTSTR *argv); 

	/** @ServiceCtrlHandler					������̿���
	*   @Opcode[in]									���Ʋ�������*/
	VOID static WINAPI ServiceCtrlHandler(DWORD Opcode);

	/*	* @ReportSvcStatus						��¼����״̬
	*   @dwCurrentState[in]					�����������״̬
	*   @dwWin32ExitCode[in]				�������ڱ���������ֹͣʱ�����Ĵ���Ĵ�����롣
	*   @dwWaitHint[in]							��������Ĺ���ʱ��
	*/
	VOID static ReportSvcStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint);

	//static BOOL Popup();

private:
	//������
	static SERVICE_STATUS_HANDLE service_status_handle_; 
	//GUI�����ַ
	static std::wstring bin_path_;
	//����״̬
	static SERVICE_STATUS service_status_;	
	//static BOOL WriteUrlToFile(std::string strUrl);

};
