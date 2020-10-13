#pragma once
#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include "ServiceInstaller.h"
#include "log.h"

#pragma once

class ServiceControl
{
public:
	ServiceControl();
	~ServiceControl();

	/** @InstallService						��������
	*   @pszServiceName[in]			��������
	*   @pszDisplayName[in]			�������б�����ʾ����
	*	 @szPath[in]							�������exe�ĵ�ַ
	*   @return TRUE							�������ɹ�
	*   @return FLASE   						������ʧ��
	*/
	BOOL ServiceControl::InstallService(PWSTR pszServiceName, PWSTR pszDisplayName, const std::wstring& bin_path);

	/** @StartService						���÷���
	*   @pszServiceName[in]			��������
	*   @pszDisplayName[in]			�������б�����ʾ����
	*   @return TRUE							�������óɹ�
	*   @return FLASE   						��������ʧ��
	*/
	BOOL StartService(PWSTR pszServiceName);

	/** @StopService						��ͣ����
	*   @pszServiceName[in]			��������
	*   @stop_dependence				�����Ƿ���������Ŀ
	*   @return TRUE							����ֹͣ�ɹ�
	*   @return FLASE   						����ֹͣʧ��
	*/
	BOOL StopService(PWSTR pszServiceName, BOOL stop_dependence);
	
	/** @DeleteService						ɾ������
	*   @pszServiceName[in]			��������
	*   @return TRUE							����ɾ���ɹ�
	*   @return FLASE   						����ɾ��ʧ��
	*/
	BOOL DeleteService(PWSTR pszServiceName);

	/** @UpdateSvcDesc					��ӷ�������
	*   @pszServiceName[in]			��������
	*   @service_discription[in]		��ӵ������ַ���
	*   @return TRUE							����������ӳɹ�
	*   @return FLASE   						����������ӳɹ�
	*/
	BOOL UpdateSvcDesc(PWSTR pszServiceName, const std::wstring& service_discription);

private:
	/** @StopServiceDependence	ֹͣ���������
	*   @service_handle[in]				������
	*   @return TRUE							ֹͣ����������ɹ�
	*   @return FLASE   						ֹͣ���������ʧ��
	*/
	BOOL StopServiceDependence(SC_HANDLE service_handle);

	/** @CheckStopFinish				����Ƿ�ֹͣ��
	*   @service_handle[in]				������
	*   @return TRUE							ֹͣ����������ɹ�
	*   @return FLASE   						ֹͣ���������ʧ��
	*/
	BOOL CheckStopFinish(SC_HANDLE service_handle);

private:
	//������
	SC_HANDLE manager_handle_;
};

