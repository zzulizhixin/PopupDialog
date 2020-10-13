#pragma once
#include "stdafx.h"
#include "write_json_file.h"
#include "read_json.h"
#include <time.h>

/**  @GetCloseTimeLong						获取客户端距离现在未启动的时长
*		@close_time_long[out]					time_t 未启动时长
*		@return TRUE									获取成功
*		@return FLASE									获取失败
*/
BOOL GetCloseTimeLong(time_t &close_time_long);

/**  @CheckTimeOut								验证未启动时长是否达到阈值
*		@close_time_long[in]						未启动时长秒数
*      @pop_message[in]							后台下发的Json数据
*		@return TRUE									达到阈值
*		@return FLASE									未达到阈值
*/
BOOL CheckTimeOut(const time_t &close_time_long, const PopupMessage &pop_message);

/**  @CheckMessagePop						判断是否弹窗
*      @pop_message[in]							后台下发的Json数据
*		@return TRUE									后台数据为弹
*		@return FLASE									后台数据为不弹
*/
BOOL CheckMessagePop(const PopupMessage& pop_message);

/**  @CheckUserOperation					检查用户操作
*      @click_time_long[out]					用户上次操作距离现在的时长
*      @click_type[out]									操作类型
*		@return TRUE									读取成功
*		@return FLASE									读取失败
*/
BOOL CheckUserOperation(time_t& click_time_long, int& click_type);


/**  @WriteServerStatus					    记录服务当前状态到注册表
*      @server_status[in]							服务状态：0服务正常流程判断状态， 1等待12小时重新请求数据，2等待下一次弹出时间是否达到后台下发verify_time的阈值状态
*		@return TRUE									记录成功
*		@return FLASE									记录失败
*/
BOOL WriteServerStatus(const std::string& server_status);

/**  @CheckServerStatus					    检查服务当前状态到注册表
*      @server_status[out]						服务状态：0服务正常流程判断状态， 1等待12小时重新请求数据，2等待下一次弹出时间是否达到后台下发verify_time的阈值状态
*		@return TRUE									读取成功
*		@return FLASE									读取失败
*/
BOOL CheckServerStatus(int& server_status);

/**  @WriteLastAskServerTime			    记录当前时间到注册表last_ask_server
*		@return TRUE									记录时间成功
*		@return FLASE									记录时间失败
*/
BOOL WriteLastAskServerTime();

/**  @CheckLastAskServerTime			获取当前时间距离last_ask_server的时间长度
*      @last_ask_server_long[out]			距离上次请求的时间差
*		@return TRUE									获取时间差成功
*		@return FLASE									获取时间差失败
*/
BOOL CheckLastAskServerTime(time_t& last_ask_server_long);

/**  @GetUserId										获取注册表用户id
*      @userid[out]										用户id
*		@return TRUE									获取成功
*		@return FLASE									获取失败
*/BOOL GetUserId(std::string& userid);
