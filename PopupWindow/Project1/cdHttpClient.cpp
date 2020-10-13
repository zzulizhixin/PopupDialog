
#include "cdHttpClient.h"
#include "ReadJson.h"
//#include "Common.h"


myCHttpClient::myCHttpClient(HWND hwnd) : m_debug(false)
{
	m_hwnd = hwnd;
}

myCHttpClient::~myCHttpClient(void)
{

}

static int OnDebug(CURL *, curl_infotype itype, char* pData, size_t size, void *)
{
	if(itype == CURLINFO_TEXT)  
	{  
		//printf("[TEXT]%s\n", pData);  
	}  
	else if(itype == CURLINFO_HEADER_IN)  
	{  
		printf("[HEADER_IN]%s\n", pData);  
	}  
	else if(itype == CURLINFO_HEADER_OUT)  
	{  
		printf("[HEADER_OUT]%s\n", pData);  
	}  
	else if(itype == CURLINFO_DATA_IN)  
	{  
		printf("[DATA_IN]%s\n", pData);  
	}  
	else if(itype == CURLINFO_DATA_OUT)  
	{  
		printf("[DATA_OUT]%s\n", pData);  
	}  
	return 0;  
}

static int OnWriteData(void* buffer, size_t size, size_t nmemb, void* lpVoid)
{
	std::string *str =dynamic_cast<string *>((string *)lpVoid);
	if(NULL == str || NULL == buffer)  
	{  
		return -1;  
	}  

	char* pData = (char *)buffer;  
	str->append(pData,size* nmemb);  
	return nmemb;  
}

static int progress_callback(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow)
{
	if (dltotal > 0)
	{
		int dPercet = (int)(dlnow*100.0f / dltotal);
		myCHttpClient *pThis = (myCHttpClient *)clientp;
		if (pThis->m_hwnd)
		{
			PostMessage(pThis->m_hwnd, WM_USER + 9018, (WPARAM)dPercet, 0);
			//printf("[%d]当前下载%d\%\n", __LINE__, dPercet);
			if (dPercet == 100)
			{
				//printf("[%d]下载完成\n\n", __LINE__);
			}
		}
	}
	return CURLE_OK;
}

int myCHttpClient::Post(const string &url, const string &post, string &response)
{
	CURLcode res;
	CURL *curl = curl_easy_init();
	if (NULL == curl)
	{
		return CURLE_FAILED_INIT;
	}

	if (m_debug)
	{
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post.c_str());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 20);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 20);
	res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	return res;
}

int myCHttpClient::Get(const string &url, string &response)
{
	CURLcode res;
	CURL *curl = curl_easy_init();
	if (NULL == curl)
	{
		return CURLE_FAILED_INIT;
	}

	if (m_debug)
	{
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}
	//DbgInfo("%d", __LINE__);
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);

	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
	curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progress_callback);
	curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, this);

	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10*60);//
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10 * 60);
	res = curl_easy_perform(curl);
	if (res != CURLE_OK)
	{
		DbgInfo(L"curl_easy_perform() failed:%s",curl_easy_strerror(res));
	}
	curl_easy_cleanup(curl);  
	return res;
}

int myCHttpClient::Posts(const string &url, const string &post, string &response, const char* pCaPath /* = NULL */)
{
	CURLcode res;  
	CURL *curl = curl_easy_init();  
	if(NULL == curl)  
	{  
		return CURLE_FAILED_INIT;  
	}  
	if(m_debug)  
	{  
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);  
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);  
	}  

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());  
	curl_easy_setopt(curl, CURLOPT_POST, 1);  
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post.c_str());  
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);  
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);  
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&response);  
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);  

	if(NULL == pCaPath)  
	{  
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);  
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);  
	}
	else
	{  
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);  
		curl_easy_setopt(curl, CURLOPT_CAINFO, pCaPath);  
	}  
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10);  
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);  
	res = curl_easy_perform(curl);  
	curl_easy_cleanup(curl);  
	return res;  
}

int myCHttpClient::Gets(const string &url, const string &post, string &response, const char* pCaPath /* = NULL */)
{
	CURLcode res;  
	CURL* curl = curl_easy_init();  
	if(NULL == curl)  
	{  
		return CURLE_FAILED_INIT;  
	}  
	if(m_debug)  
	{  
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);  
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);  
	}  
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());  
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);  
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);  
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&response);  
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);  
	if(NULL == pCaPath)  
	{  
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);  
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);  
	}
	else
	{  
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);  
		curl_easy_setopt(curl, CURLOPT_CAINFO, pCaPath);  
	}  
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);  
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);  
	res = curl_easy_perform(curl);  
	curl_easy_cleanup(curl);  
	return res;  
}

void myCHttpClient::SetDebug(bool debug)
{
	m_debug = debug;
}
