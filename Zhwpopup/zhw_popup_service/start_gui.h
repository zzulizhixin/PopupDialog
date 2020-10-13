#pragma once
#include "stdafx.h"
#include "winsvc.h"

class StartGUIProc
{
public:
	/** @StartServiceWithGUI				����������
	*   @bin_path[in]								�������·��	*/
	StartGUIProc(const std::wstring& processPath, const std::wstring& arguments = L"");

	//ͨ���������з������
	BOOL Run();

private:
	//��ȡ��ǰ�û�����
	HANDLE GetCurrentUserToken();

private:
	//������̵�·��
	std::wstring processPath_;

	std::wstring arguments_;
};