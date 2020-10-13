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

	/** @PopupJudge							判断是否进行弹窗逻辑函数
	*   @return TRUE								弹出成功
	*   @return FLASE   							弹出失败
	*/
	BOOL PopupJudge();

private:
	/** @GetJsonFilePath						获取Json文件保存地址
*   @return LPCTSTR						Json文件绝对地址
*   @return NULL   							获取失败
*/
	LPCTSTR GetJsonFilePath();
	/** @Popup										弹窗函数，如果判断需要弹窗，执行此函数进行弹窗
*   @return TRUE								弹出成功
*   @return FLASE   							弹出失败
*/
	BOOL Popup();

	/** @GetFilePath								获取当前exe所在文件夹地址
	*    @wsFullName[in]						exe绝对地址
	*   @return wstring							路径地址
	*/
	std::wstring GetFilePath(const std::wstring& wsFullName);

	/** @GetAppPath						获取当前exe绝对地址
	*   @return wstring							地址
	*/
	std::wstring GetAppPath();

	/** @WriteUrlToFile						保存URL数据到本地Json文件
	*   @return TRUE								保存成功
	*   @return FLASE   							保存失败
	*/
	BOOL WriteUrlToFile();


	/** @CheckClickTime						时间检查
	*   @return TRUE								保存成功
	*   @return FLASE   							保存失败
	*/
	BOOL CheckClickTime(time_t limit_time, time_t click_time_long);

	/** @NormalCheck							正常逻辑检查
	*   @return TRUE								弹出成功
	*   @return FLASE   							弹出失败
	*/
	BOOL NormalCheck();

	/** @NextTimeCheck						下次弹窗时间检查线程
	*   @return TRUE								弹出成功
	*   @return FLASE   							弹出失败
	*/
	static DWORD WINAPI NextTimeCheck(PVOID pvParam);

	/** @IsFileExist								路径文件是否存在
	*    @csFile[in]									路径
	*   @return TRUE								存在
	*   @return FLASE   							不存在
	*/
	BOOL IsFileExist(const std::wstring& csFile);

	/** @DeletUsername						卸载删除注册表username
	*   @return TRUE								卸载成功
	*   @return FLASE   							卸载失败
	*/
	BOOL DeleteUsername();

	/** @IsZhwExist						   租号玩客户端是否存在于电脑中
	*   @return TRUE								存在
	*   @return FLASE   							不存在
	*/
	BOOL IsZhwExist();
	/** @DeletUsername						卸载服务注册表信息
*   @return TRUE								卸载成功
*   @return FLASE   							卸载失败
*/
	BOOL DeleteServiceReg();

private:
	std::wstring bin_path_;	//pop.exe地址
	std::wstring json_path_;//Json文件地址
	std::string str_url_;		//url地址
	PopupMessage popup_message_;	//读取Json文件保存的结构体


};