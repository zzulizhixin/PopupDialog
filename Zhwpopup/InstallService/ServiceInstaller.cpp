#pragma region "Includes"
#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include "ServiceInstaller.h"
#include "log.h"
#pragma endregion

ServiceControl::ServiceControl()
	: manager_handle_(NULL)
{
	manager_handle_ = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
}

ServiceControl::~ServiceControl()
{
	if(manager_handle_ != NULL)
		::CloseServiceHandle(manager_handle_);
}


BOOL ServiceControl::InstallService(PWSTR pszServiceName, PWSTR pszDisplayName, const std::wstring& bin_path)
{
	// 打开本地默认服务控制管理器数据库
	if (manager_handle_ == NULL)
	{
		DLOG("OpenSCManager失败");
		return FALSE;
	}

	SC_HANDLE schService = NULL;

	//通过调用CreateService将服务安装到SCM中
	schService = CreateService(
		manager_handle_,																						     // 服务控制管理程序维护的登记数据库的句柄
		pszServiceName,																						     // 服务名
		pszDisplayName,																						     // 显示的服务名
		SERVICE_ALL_ACCESS,																				     // 服务返回类型
		SERVICE_WIN32_OWN_PROCESS |SERVICE_INTERACTIVE_PROCESS,        // 服务类型
		SERVICE_AUTO_START,																					 		 	// 启动服务
		SERVICE_ERROR_NORMAL,												    					 // 服务启动失败的严重程度
		bin_path.c_str(),																											// 服务程序二进制文件的路径
		NULL,																											
		NULL,																										    
		NULL,																											
		NULL,																											
		NULL																											
		);
	if (schService == NULL)
	{
		DLOG("CreateService失败");
		return FALSE;
	}
	else
	{
		DLOG("CreateService成功");
		if (schService)
		{
			CloseServiceHandle(schService);
			schService = NULL;
		}
		return TRUE;
	}
}

BOOL ServiceControl::StartService(PWSTR pszServiceName)
{
	if (manager_handle_ == NULL)
	{
		DLOG("OpenSCManager失败");
		return FALSE;
	}

	SC_HANDLE service_handle = ::OpenService(manager_handle_, pszServiceName, SERVICE_ALL_ACCESS); 
	if (service_handle == NULL) 
	{ 
		DLOG("OpenService失败句柄为空");
		return FALSE; 
	}
	if (!::StartService(service_handle, 0, NULL))
	{
		DLOG("StartService失败");
		return FALSE; 
	}
	DLOG("StartService成功");

	SERVICE_STATUS_PROCESS service_status = {0}; 
	DWORD byte_need = 0;
	if (!::QueryServiceStatusEx(service_handle, SC_STATUS_PROCESS_INFO, (LPBYTE)&service_status, sizeof(SERVICE_STATUS_PROCESS), &byte_need)) 
	{
		DLOG("QueryServiceStatusEx失败");
		return FALSE; 
	}
	//服务有可能进入了SERVICE_START_PENDING状态，需要处理
	DWORD dwOldCheckPoint = service_status.dwCheckPoint;
	DWORD start_tick = ::GetTickCount();
	while (service_status.dwCurrentState == SERVICE_START_PENDING) 
	{ 
		::Sleep(1000);
		if (!::QueryServiceStatusEx(service_handle, SC_STATUS_PROCESS_INFO, (LPBYTE)&service_status, sizeof(SERVICE_STATUS_PROCESS), &byte_need))
		{
			DLOG("QueryServiceStatusEx失败");
			break; 
		}
		if (service_status.dwCheckPoint > dwOldCheckPoint)
		{
			start_tick = ::GetTickCount();
			dwOldCheckPoint = service_status.dwCheckPoint;
		}
		else
		{
			if(::GetTickCount() - start_tick > service_status.dwWaitHint)
			{
				break;
			}
		}
	} 
	::CloseServiceHandle(service_handle); 
	if (service_status.dwCurrentState == SERVICE_RUNNING) 
	{
		DLOG("服务启动成功");
		return TRUE;
	}
	else 
	{ 
		DLOG("服务启动失败");
		return FALSE;
	} 
}

BOOL ServiceControl::StopService(PWSTR pszServiceName, BOOL stop_dependence)
{
	if (manager_handle_ == NULL)
	{
		return FALSE;
	}
	SC_HANDLE service_handle = ::OpenService(manager_handle_, pszServiceName, SERVICE_ALL_ACCESS); 
	if (CheckStopFinish(service_handle))
	{
		return TRUE;
	}

	//下边是先结束依赖，再结束自己
	if (stop_dependence) 
	{
		StopServiceDependence(service_handle);
	}
	SERVICE_STATUS own_service_status = {0};
	if (!::ControlService(service_handle, SERVICE_CONTROL_STOP, &own_service_status))
	{
		return FALSE;
	}
	return CheckStopFinish(service_handle);
}

BOOL ServiceControl::StopServiceDependence( SC_HANDLE service_handle )
{

	LPENUM_SERVICE_STATUS lpDependencies = NULL;
	ENUM_SERVICE_STATUS     ess;

	DWORD byte_need = 0;
	DWORD service_count = 0;
	// 如果EnumDependentServices 直接返回非0，说明没有依赖服务
	if (::EnumDependentServices(service_handle, SERVICE_ACTIVE, lpDependencies, 0, &byte_need, &service_count)) 
	{
		return TRUE;
	}
	if (GetLastError() != ERROR_MORE_DATA)
	{
		return FALSE;
	}
	lpDependencies = (LPENUM_SERVICE_STATUS)::HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, byte_need);
	if (!lpDependencies)
	{
		return FALSE;
	}
	__try {
		// 获得依赖服务
		if (!::EnumDependentServices( service_handle, SERVICE_ACTIVE, lpDependencies, byte_need, &byte_need, &service_count))
		{
			return FALSE;
		}
		for (DWORD i = 0; i < service_count; i++) 
		{
			ess = *(lpDependencies + i);
			SC_HANDLE dep_service_handle = NULL;
			dep_service_handle = ::OpenService(manager_handle_, ess.lpServiceName, SERVICE_STOP | SERVICE_QUERY_STATUS);
			if (dep_service_handle == NULL)
			{
				return FALSE;
			}
			__try {
				SERVICE_STATUS dep_service_status = {0};
				if (!::ControlService(dep_service_handle, SERVICE_CONTROL_STOP, &dep_service_status))
				{
					::HeapFree(::GetProcessHeap(), 0, lpDependencies);
					return FALSE;
				}
				SERVICE_STATUS_PROCESS service_status_pro = {0};
				DWORD byte_need = 0;
				if (!::QueryServiceStatusEx(service_handle, SC_STATUS_PROCESS_INFO, (LPBYTE)&service_status_pro, sizeof(SERVICE_STATUS_PROCESS), &byte_need))
				{
					::HeapFree(::GetProcessHeap(), 0, lpDependencies);
					return FALSE;
				}
				DWORD dwOldCheckPoint = service_status_pro.dwCheckPoint;
				DWORD start_tick = ::GetTickCount();
				while (service_status_pro.dwCurrentState == SERVICE_STOP_PENDING)
				{
					::Sleep(1000);
					if (!::QueryServiceStatusEx(service_handle, SC_STATUS_PROCESS_INFO, (LPBYTE)&service_status_pro, sizeof(SERVICE_STATUS_PROCESS), &byte_need))
					{
						::HeapFree(::GetProcessHeap(), 0, lpDependencies);
						return FALSE;
					}
					if (service_status_pro.dwCheckPoint > dwOldCheckPoint)
					{
						//service start into next step
						start_tick = ::GetTickCount();
						dwOldCheckPoint = service_status_pro.dwCheckPoint;
					}
					else
					{
						if(::GetTickCount() - start_tick > service_status_pro.dwWaitHint)
						{
							::HeapFree(::GetProcessHeap(), 0, lpDependencies);
							return FALSE;
						}
					}
				}
			} 
			__finally 
			{
				// 关闭服务
				::CloseServiceHandle(dep_service_handle);
			}
		}
	} 
	__finally 
	{
		// 释放内存
		::HeapFree(::GetProcessHeap(), 0, lpDependencies);
	}

	return TRUE;
}

BOOL ServiceControl::CheckStopFinish( SC_HANDLE service_handle )
{
	SERVICE_STATUS_PROCESS service_status_pro = {0};
	DWORD byte_need = 0;
	if (!::QueryServiceStatusEx(service_handle, SC_STATUS_PROCESS_INFO, (LPBYTE)&service_status_pro, sizeof(SERVICE_STATUS_PROCESS), &byte_need))
	{
		return FALSE;
	}
	if (service_status_pro.dwCurrentState == SERVICE_STOPPED) 
	{
		return TRUE;
	}
	DWORD dwOldCheckPoint = service_status_pro.dwCheckPoint;
	DWORD start_tick = ::GetTickCount();
	//如果已经进入到SERVICE_STOP_PENDING状态，则等待结束
	while (service_status_pro.dwCurrentState == SERVICE_STOP_PENDING) 
	{
		::Sleep(1000);
		if (!::QueryServiceStatusEx(service_handle, SC_STATUS_PROCESS_INFO, (LPBYTE)&service_status_pro, sizeof(SERVICE_STATUS_PROCESS), &byte_need))
		{
			break; 
		}
		if (service_status_pro.dwCheckPoint > dwOldCheckPoint)
		{
			//service start into next step
			start_tick = ::GetTickCount();
			dwOldCheckPoint = service_status_pro.dwCheckPoint;
		}
		else
		{
			if(::GetTickCount() - start_tick > service_status_pro.dwWaitHint)
			{
				break;
			}
		}
	}
	if (service_status_pro.dwCurrentState == SERVICE_STOPPED)
	{
		return TRUE;
	}
	else 
	{
		return FALSE;
	}
}

BOOL ServiceControl::DeleteService(PWSTR pszServiceName)
{
	if (manager_handle_ == NULL)
	{
		return FALSE;
	}
	SC_HANDLE service_handle = ::OpenService(manager_handle_, pszServiceName, DELETE);
	if (service_handle == NULL)
	{ 
		return FALSE;
	}
	if (!::DeleteService(service_handle) ) 
	{
		::CloseServiceHandle(service_handle); 
		return FALSE;
	}
	else 
	{
		::CloseServiceHandle(service_handle); 
		return TRUE;
	}
}

BOOL ServiceControl::UpdateSvcDesc( PWSTR pszServiceName, const std::wstring& service_discription )
{
	if(manager_handle_ == NULL)
	{
		return FALSE;
	}
	SC_HANDLE schService = ::OpenService( 
		manager_handle_,            
		pszServiceName,              
		SERVICE_CHANGE_CONFIG); 

	//更改服务描述
	SERVICE_DESCRIPTION sd;
	wchar_t* buff = new(std::nothrow)wchar_t[service_discription.length() + 1];
	::memset(buff, 0, (service_discription.length() + 1) * sizeof(wchar_t));
	::wcsncpy_s(buff, service_discription.length()+1, service_discription.c_str(), service_discription.length());
	sd.lpDescription = buff;

	BOOL bRet = FALSE;
	if(!::ChangeServiceConfig2(
		schService,                
		SERVICE_CONFIG_DESCRIPTION, 
		&sd))                      
	{
		DLOG("ChangeServiceConfig失败");
	}
	else
	{
		DLOG("ChangeServiceConfig成功");
		bRet = TRUE;
	}
	::delete [] buff;
	CloseServiceHandle(schService); 
	return bRet;
}

