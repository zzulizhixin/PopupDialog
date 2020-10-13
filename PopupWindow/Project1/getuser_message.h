#pragma once
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string>
//提交用户数据结构体
typedef struct UserMessage
{
	std::string id;											//广告id
	std::string action;										//用户进行的操作，0：关闭窗口，1：打开链接， 2：不感兴趣
	std::string device;									//设备标识
	std::string system;									//操作系统
	std::string user_id;									//客户端登录的用户id
}USER_MESSAGE, * LPUSER_MESSAGE;

//获取用户MAC地址返回
wchar_t* GetMacAddress();

//获取用户操作系统返回
std::string GetSysVersion();

//获取用户ID
BOOL GetUserId(std::string& user_id);

