#pragma once

#include <Windows.h>
#include <string>
#include <curl/curl.h>


using std::string;

class myCHttpClient
{
public:
	myCHttpClient(HWND hwnd);
	~myCHttpClient(void);
	int Post(const string &url, const string &post, string &response);
	int Get(const string &url, string &response);
	int Posts(const string &url, const string &post, string &response, const char* pCaPath = NULL);
	int Gets(const string &url, const string &post, string &response, const char* pCaPath = NULL);
	void SetDebug(bool debug);
protected:	
private:
	bool m_debug;
public:
	HWND m_hwnd;
};