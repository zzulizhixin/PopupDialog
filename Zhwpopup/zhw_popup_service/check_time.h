#pragma once
#include "stdafx.h"
#include "write_json_file.h"
#include "read_json.h"
#include <time.h>

/**  @GetCloseTimeLong						��ȡ�ͻ��˾�������δ������ʱ��
*		@close_time_long[out]					time_t δ����ʱ��
*		@return TRUE									��ȡ�ɹ�
*		@return FLASE									��ȡʧ��
*/
BOOL GetCloseTimeLong(time_t &close_time_long);

/**  @CheckTimeOut								��֤δ����ʱ���Ƿ�ﵽ��ֵ
*		@close_time_long[in]						δ����ʱ������
*      @pop_message[in]							��̨�·���Json����
*		@return TRUE									�ﵽ��ֵ
*		@return FLASE									δ�ﵽ��ֵ
*/
BOOL CheckTimeOut(const time_t &close_time_long, const PopupMessage &pop_message);

/**  @CheckMessagePop						�ж��Ƿ񵯴�
*      @pop_message[in]							��̨�·���Json����
*		@return TRUE									��̨����Ϊ��
*		@return FLASE									��̨����Ϊ����
*/
BOOL CheckMessagePop(const PopupMessage& pop_message);

/**  @CheckUserOperation					����û�����
*      @click_time_long[out]					�û��ϴβ����������ڵ�ʱ��
*      @click_type[out]									��������
*		@return TRUE									��ȡ�ɹ�
*		@return FLASE									��ȡʧ��
*/
BOOL CheckUserOperation(time_t& click_time_long, int& click_type);


/**  @WriteServerStatus					    ��¼����ǰ״̬��ע���
*      @server_status[in]							����״̬��0�������������ж�״̬�� 1�ȴ�12Сʱ�����������ݣ�2�ȴ���һ�ε���ʱ���Ƿ�ﵽ��̨�·�verify_time����ֵ״̬
*		@return TRUE									��¼�ɹ�
*		@return FLASE									��¼ʧ��
*/
BOOL WriteServerStatus(const std::string& server_status);

/**  @CheckServerStatus					    ������ǰ״̬��ע���
*      @server_status[out]						����״̬��0�������������ж�״̬�� 1�ȴ�12Сʱ�����������ݣ�2�ȴ���һ�ε���ʱ���Ƿ�ﵽ��̨�·�verify_time����ֵ״̬
*		@return TRUE									��ȡ�ɹ�
*		@return FLASE									��ȡʧ��
*/
BOOL CheckServerStatus(int& server_status);

/**  @WriteLastAskServerTime			    ��¼��ǰʱ�䵽ע���last_ask_server
*		@return TRUE									��¼ʱ��ɹ�
*		@return FLASE									��¼ʱ��ʧ��
*/
BOOL WriteLastAskServerTime();

/**  @CheckLastAskServerTime			��ȡ��ǰʱ�����last_ask_server��ʱ�䳤��
*      @last_ask_server_long[out]			�����ϴ������ʱ���
*		@return TRUE									��ȡʱ���ɹ�
*		@return FLASE									��ȡʱ���ʧ��
*/
BOOL CheckLastAskServerTime(time_t& last_ask_server_long);

/**  @GetUserId										��ȡע����û�id
*      @userid[out]										�û�id
*		@return TRUE									��ȡ�ɹ�
*		@return FLASE									��ȡʧ��
*/BOOL GetUserId(std::string& userid);
