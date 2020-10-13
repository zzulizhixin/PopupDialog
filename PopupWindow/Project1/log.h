#pragma once
#include <iostream>

std::string start(const std::string& info, const int line, const std::string&function, const std::string& file);
void WriteLog(const std::string& szLog);

#ifdef _DEBUG
#define DLOG(log)   WriteLog(start(log, __LINE__ , __FUNCTION__ ,__FILE__ ))
#else
#define DLOG(log_rank)
#endif // _DEBUG

