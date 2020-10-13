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

//后台广告数据下发存储结构体
typedef struct PopupMessage
{
	int code;													//是否弹窗标识：1弹，0不弹
	std::string message;								//广告信息
	struct PopoutData
	{
		std::string type;										//广告点击相应类型，1：打开客户端，2：打开连接（link）
		std::string id;											//广告id
		std::string link;											//广告连接url
		std::string title;										//弹窗标题
		std::string image;										//广告图片显示的url
	}popout_data;																	//广告数据结构体
	std::string verify_limit;								//客户端重新进行未启动时长验证的基础天数A
	std::string start_up_limit;						//客户端未启动时长
}POPUP_MESSAGE, * LPOPUP_MESSAGE;


/** @ReadPopupJsonMessage						读取Json数据进行解析存入结构体中
*   @popup_message[out]								数据存入的结构体名
*   @file_name[in]											读取json的文件路径
*   @return TRUE												读取文件成功
*   @return FLASE   											读取文件失败*/
BOOL ReadPopupJsonMessage(PopupMessage &popup_message, LPCTSTR file_name);

//中文字符串转换
std::string utf8_to_string(const std::string& str);

/** @DownLoadPircture									下载结构体image的url图片
*   @popup_message[in]								广告数据结构体
*   @picture_name[out]									image文件名
*   @return string												成功返回image文件名
*   @return NULL   											不成功返回NULL*/
std::string DownLoadPircture(PopupMessage popup_message, TCHAR *szPath);

/** @CheckFolderExist									检查文件夹路径是否存在
*   @strPath[in]													路径名
*   @return TRUE											    存在
*   @return FLASE   											不存在*/
BOOL  CheckFolderExist(std::string& strPath);

/** @RequestForData										拉取URL数据
*   @lpszURL[in]												URL地址
*   @response[out]											数据
*   @return TRUE											    拉取成功
*   @return FLASE   											拉取失败*/
BOOL RequestForData(const char* lpszURL, std::string& response, HWND hWnd = NULL);

/** @PostUserData											提交用户信息
*   @lpszURL[in]												URL地址
*   @response[out]											返回结果
*   @user_data[out]											上传的用户数据
*   @return TRUE											    上传成功
*   @return FLASE   											上传失败*/
BOOL PostUserData(const char* lpszURL, std::string& response, std::string& user_data, HWND hWnd = NULL);

//wchar_t转string
std::string ws2s(const std::wstring& wstr);

//string转wchar_t
std::wstring s2ws(const std::string& str);

//用于在调试版本中输出调试信息到dbgview
void DbgInfoA(const char* strOutputString, ...);
void DbgInfoW(const wchar_t* strOutputString, ...);
