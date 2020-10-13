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

	/** @InstallService						创建服务
	*   @pszServiceName[in]			服务名称
	*   @pszDisplayName[in]			服务在列表中显示名称
	*	 @szPath[in]							服务进程exe的地址
	*   @return TRUE							服务建立成功
	*   @return FLASE   						服务建立失败
	*/
	BOOL ServiceControl::InstallService(PWSTR pszServiceName, PWSTR pszDisplayName, const std::wstring& bin_path);

	/** @StartService						启用服务
	*   @pszServiceName[in]			服务名称
	*   @pszDisplayName[in]			服务在列表中显示名称
	*   @return TRUE							服务启用成功
	*   @return FLASE   						服务启用失败
	*/
	BOOL StartService(PWSTR pszServiceName);

	/** @StopService						暂停服务
	*   @pszServiceName[in]			服务名称
	*   @stop_dependence				服务是否有依赖项目
	*   @return TRUE							服务停止成功
	*   @return FLASE   						服务停止失败
	*/
	BOOL StopService(PWSTR pszServiceName, BOOL stop_dependence);
	
	/** @DeleteService						删除服务
	*   @pszServiceName[in]			服务名称
	*   @return TRUE							服务删除成功
	*   @return FLASE   						服务删除失败
	*/
	BOOL DeleteService(PWSTR pszServiceName);

	/** @UpdateSvcDesc					添加服务描述
	*   @pszServiceName[in]			服务名称
	*   @service_discription[in]		添加的描述字符串
	*   @return TRUE							服务描述添加成功
	*   @return FLASE   						服务描述添加成功
	*/
	BOOL UpdateSvcDesc(PWSTR pszServiceName, const std::wstring& service_discription);

private:
	/** @StopServiceDependence	停止服务的依赖
	*   @service_handle[in]				服务句柄
	*   @return TRUE							停止服务的依赖成功
	*   @return FLASE   						停止服务的依赖失败
	*/
	BOOL StopServiceDependence(SC_HANDLE service_handle);

	/** @CheckStopFinish				检查是否停止了
	*   @service_handle[in]				服务句柄
	*   @return TRUE							停止服务的依赖成功
	*   @return FLASE   						停止服务的依赖失败
	*/
	BOOL CheckStopFinish(SC_HANDLE service_handle);

private:
	//服务句柄
	SC_HANDLE manager_handle_;
};

