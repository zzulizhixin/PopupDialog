#pragma once
#include "stdafx.h"
#include "cJSON.h"

//后台广告数据下发存储结构体
typedef struct PopupMessage
{
	int code ;													//是否弹窗标识：1弹，0不弹
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
}POPUP_MESSAGE, *LPOPUP_MESSAGE;


//提交用户数据结构体
typedef struct UserMessage
{
	std::string id;											//广告id
	std::string action;										//用户进行的操作，0：关闭窗口，1：打开链接， 2：不感兴趣
	std::string device;									//设备标识
	std::string system;									//操作系统
	std::string user_id;									//客户端登录的用户id
}USER_MESSAGE, *LPUSER_MESSAGE;


/** @ReadPopupJsonMessage						读取Json数据进行解析存入结构体中
*   @popup_message[out]								数据存入的结构体名
*   @file_name[in]											读取json的文件路径
*   @return TRUE												读取文件成功
*   @return FLASE   											读取文件失败
*/
BOOL ReadPopupJsonMessage(PopupMessage &popup_message, LPCTSTR file_name);

///** @DownLoadPircture									下载结构体image的url图片
//*   @popup_message[in]									广告数据结构体
//*   @picture_name[out]									image文件名
//*   @return string												成功返回image文件名
//*   @return NULL   											不成功返回NULL*/
//std::string DownLoadPircture(PopupMessage popup_message, TCHAR *szPath);
//
///** @CheckFolderExist									检查路径是否存在
//*   @strPath[in]													路径
//*   @return TRUE												存在
//*   @return FLASE   											不存在*/
//BOOL CheckFolderExist(std::string& strPath);

//中文字符串转换
std::string utf8_to_string(const std::string& str);
