#include "log.h"
#include <Windows.h>
#include <string>
#include <mutex>

std::mutex mut;
std::string start(const std::string& info, const int line, const std::string&function , const std::string& file)
{
	std::string strline = std::to_string(line);
	std::string ret = "File:"+file + " Line:" + strline + " Funciton:" + function + " [msg]:" + info;
	return  ret;
}

void WriteLog(const std::string& szLog)
{
	try
	{
		SYSTEMTIME st;
		GetLocalTime(&st);
		FILE *fp;
		char buf[MAX_PATH] = { 0 };

		sprintf_s(buf, "%04d.%02d.%02d_log.txt", st.wYear, st.wMonth, st.wDay);
		fp = _fsopen( buf, "at", _SH_DENYNO);
		mut.lock();
		fprintf_s(fp, "time:%02d:%02d:%02d:%03d %s\n", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, szLog.c_str());
		mut.unlock();
		fclose(fp);
	}
	catch (const std::exception&)
	{
		OutputDebugStringA("WriteLog Error");
	}
}
