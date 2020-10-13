// Project1.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#pragma warning(disable:4996)
#include <tchar.h>
#include <Shlobj.h>
#include <iostream>
#include "ReadJson.h"
#include "J:\\Project\\Project1\\include\\ximage.h"
#include "cdHttpClient.h"
#include "thread"
#include "resource.h"
#include "wingdi.h"
#include "getuser_message.h"
#include "log.h"
#include "open_process.h"
#include <time.h>
#pragma comment (lib,"jpeg.lib")
#pragma comment (lib,"png.lib")
#pragma comment (lib,"mng.lib")
#pragma comment (lib,"libpsd.lib")
#pragma comment (lib,"libdcr.lib")
#pragma comment (lib,"jbig.lib")
#pragma comment (lib,"jpeg.lib")
#pragma comment (lib,"jasper.lib")
#pragma comment (lib,"tiff.lib")
#pragma comment (lib,"zlib.lib")
#pragma comment (lib,"cximage.lib")
#pragma comment(lib,"msimg32.lib")
#pragma comment(lib,"Wtsapi32.lib")
/////////////////////////////////////////////////////////////////

#define POPUP_WINDOW_WIDTH  330		//设置显示对话框的宽度
#define POPUP_WINDOW_HEIHT    254  	//设置显示对话框的高度

PopupMessage popup_message;				    //json数据结构体的全局变量
UserMessage user_message;						   //用户操作信息结构体的全局变量

unsigned char WindowCount = 0;				   //计算窗口个数
BOOL bTrackFlag = FALSE;

HINSTANCE Btn_Instance;							   //退出按钮句柄
HINSTANCE btnTextInstance;						   //近期不再显示按钮

HWND hBtnClose;										   //退出按钮
HWND hBtnText;											   //近期不再显示按钮

#define IDB_CLOSE     3301							//退出按钮句柄
#define IDB_CLOSE_HIDE    3302			    //近期不再显示按钮


//检查系统版本是否是Vista或更高的版本  
BOOL  IsOsVersionVistaOrGreater()
{
	OSVERSIONINFOEX    ovex;
	CHAR  szVersionInfo[1024];
	*szVersionInfo = '\x00';
	//设置参数的大小，调用并判断是否成功  
	ovex.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	if (!GetVersionEx((LPOSVERSIONINFO)(&ovex)))
	{
		return false;
	}
	//通过版本号，判断是否是vista及之后版本  
	if (ovex.dwMajorVersion > 5)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//检查并根据系统版本选择打开程序方式  
void ShellExecuteExOpen(LPCWSTR appName, LPCWSTR appPath)
{
	if (IsOsVersionVistaOrGreater())
	{
		OutputDebugString(_T("[project1]系统为高版本进行提权后打开客户端"));
		SHELLEXECUTEINFO sei = { sizeof(SHELLEXECUTEINFO) };
		sei.fMask = SEE_MASK_NOCLOSEPROCESS;
		sei.lpVerb = TEXT("runas");
		sei.lpFile = appName;
		sei.lpDirectory = appPath;
		sei.nShow = SW_SHOWNORMAL;
		if (!ShellExecuteEx(&sei))
		{
			DWORD dwStatus = GetLastError();
			if (dwStatus == ERROR_CANCELLED)
			{
				DLOG("[project1]提升用户权限被拒绝");
			}
			else if (dwStatus == ERROR_FILE_NOT_FOUND)
			{
				DLOG("[project1]所要执行的文件没有找到");
			}
		}
		DLOG("[project1]提权成功");

	}
	else
	{
		ShellExecute(NULL, _T("open"), appName, NULL, appPath, SW_SHOWNORMAL);
		DLOG("ShellExecute");
		DLOG(std::to_string(GetLastError()).c_str());

	}
}

//读取注册表项来进行打开租号玩客户端的操作
BOOL OpenZhwClient()
{
	HKEY hKey;		//注册表获取句柄
	LONG lRet = RegOpenKeyExW(
		HKEY_LOCAL_MACHINE,
		L"SOFTWARE\\WOW6432Node\\daofeng\\zuhaowan",
		REG_OPTION_OPEN_LINK,
		KEY_ALL_ACCESS,
		&hKey);
	if (lRet != ERROR_SUCCESS)
	{
		::CloseHandle(hKey);
		return FALSE;
	}

	//获取客户端路径
	TCHAR tchClientPath[64];
	DWORD dwSize;
	dwSize = sizeof(tchClientPath);
	lRet = RegQueryValueEx(hKey, L"path", NULL, NULL, (LPBYTE)tchClientPath, &dwSize);
	if (lRet != ERROR_SUCCESS)
	{
		::CloseHandle(hKey);
		return FALSE;
	}
	DLOG("[project1]enter shellexecuteexOpen");
	ShellExecuteExOpen(_T("ZhwLogin.exe"), tchClientPath);
	return TRUE;

	//MyProcessMgr  ProcessMgr;
	//ProcessMgr.SetIsWait(TRUE);
	//if (ProcessMgr.CreateProcessAsUser(tchClientPath, 1))
	////创建进程
	//STARTUPINFO stStartUpInfo;
	//::memset(&stStartUpInfo, 0, sizeof(stStartUpInfo));
	//stStartUpInfo.cb = sizeof(stStartUpInfo);
	//PROCESS_INFORMATION stProcessInfo;
	//::memset(&stProcessInfo, 0, sizeof(stProcessInfo));
	//if (::CreateProcess(tchClientPath, NULL, NULL, NULL, false, CREATE_NEW_CONSOLE,
	//	NULL, NULL, &stStartUpInfo, &stProcessInfo))
	//{
	//	::CloseHandle(stProcessInfo.hProcess);
	//	::CloseHandle(stProcessInfo.hThread);
	//	stProcessInfo.hProcess = NULL;
	//	stProcessInfo.hThread = NULL;
	//	stProcessInfo.dwProcessId = 0;
	//	stProcessInfo.dwThreadId = 0;
	//	if (lRet)
	//	{
	//		::CloseHandle(hKey);
	//	}
	//	return TRUE;
	//}
	//else
	//{
	//	::CloseHandle(stProcessInfo.hProcess);
	//	::CloseHandle(stProcessInfo.hThread);
	//	stProcessInfo.hProcess = NULL;
	//	stProcessInfo.hThread = NULL;
	//	stProcessInfo.dwProcessId = 0;
	//	stProcessInfo.dwThreadId = 0;
	//	if (lRet)
	//	{
	//		::CloseHandle(hKey);
	//	}
	//	return FALSE;
	//}

}

//把用户操作和当前操作时间写入注册表
BOOL WriteOperationToReg(const UserMessage &user_message)
{
	HKEY hKey;		//注册表获取句柄
	LONG lRet = RegOpenKeyExW(
		HKEY_LOCAL_MACHINE,
		L"SOFTWARE\\WOW6432Node\\daofeng\\zuhaowan",
		REG_OPTION_OPEN_LINK,
		KEY_ALL_ACCESS,
		&hKey);
	if (lRet != ERROR_SUCCESS)
	{
		::CloseHandle(hKey);
		return FALSE;
	}
	HKEY hTempKey;
	if (ERROR_SUCCESS == ::RegCreateKey(hKey, _T("user_operation"), &hTempKey))
	{
		if (ERROR_SUCCESS == ::RegSetValueEx(hTempKey, _T("click_type"), 0, REG_SZ, (LPBYTE)user_message.action.c_str(), user_message.action.length() + 1))
		{
			DWORD scurTime = time(NULL);
			if (ERROR_SUCCESS == ::RegSetValueEx(hTempKey, _T("click_time"), 0, REG_DWORD, (LPBYTE)&scurTime, sizeof(scurTime)))
			{
				::RegCloseKey(hKey);
				::RegCloseKey(hTempKey);
				return TRUE;
			}
		}
	}
	return FALSE;
}

//字符串拼接上传用户信息
void PostUserMessage()
{
	std::string strPostUrl = "http://www1.zuhaowan.com:98/InterfaceApi/ClientPop/popoutLog?";
	std::string strResponse;
	std::string strUserData;
	strUserData = "ads_id=";
	strUserData += user_message.id;
	strUserData += "&action=";
	strUserData += user_message.action;
	strUserData += "&device=";
	strUserData += user_message.device;
	strUserData += "&system=";
	strUserData += user_message.system;
	strUserData += "&user_id=";
	strUserData += user_message.user_id;
	int err;
	err = PostUserData(strPostUrl.c_str(), strUserData, strResponse, NULL);

}

//设置位图背景颜色的透明颜色
HRGN SetVisibleRgnFromBitmap(HBITMAP hBmp, COLORREF color)
{
	// get image properties
	BITMAP bmp = { 0 };
	GetObject(hBmp, sizeof(BITMAP), &bmp);
	// allocate memory for extended image information
	LPBITMAPINFO bi = (LPBITMAPINFO) new BYTE[sizeof(BITMAPINFO) + 8];
	memset(bi, 0, sizeof(BITMAPINFO) + 8);
	bi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	// set window size
	//DWORD m_dwWidth	= buttonBitmapWidth;		// bitmap width
	//DWORD m_dwHeight = buttonBitmapHeight;		// bitmap height
	// create temporary dc
	HDC dc = CreateIC(L"DISPLAY", NULL, NULL, NULL);
	// get extended information about image (length, compression, length of color table if exist, ...)
	GetDIBits(dc, hBmp, 0, bmp.bmHeight, 0, bi, DIB_RGB_COLORS);
	// allocate memory for image data (colors)
	LPBYTE pBits = new BYTE[bi->bmiHeader.biSizeImage + 4];
	// allocate memory for color table
	if (bi->bmiHeader.biBitCount == 8)
	{
		// actually color table should be appended to this header(BITMAPINFO),
		// so we have to reallocate and copy it
		LPBITMAPINFO old_bi = bi;
		// 255 - because there is one in BITMAPINFOHEADER
		bi = (LPBITMAPINFO)new char[sizeof(BITMAPINFO) + 255 * sizeof(RGBQUAD)];
		memcpy(bi, old_bi, sizeof(BITMAPINFO));
		// release old header
		delete old_bi;
	}
	// get bitmap info header
	BITMAPINFOHEADER& bih = bi->bmiHeader;
	// get color table (for 256 color mode contains 256 entries of RGBQUAD(=DWORD))
	LPDWORD clr_tbl = (LPDWORD)& bi->bmiColors;
	// fill bits buffer
	GetDIBits(dc, hBmp, 0, bih.biHeight, pBits, bi, DIB_RGB_COLORS);
	DeleteDC(dc);

	BITMAP bm;
	GetObject(hBmp, sizeof(BITMAP), &bm);
	// shift bits and byte per pixel (for comparing colors)
	LPBYTE pClr = (LPBYTE)& color;
	// swap red and blue components
	BYTE tmp = pClr[0]; pClr[0] = pClr[2]; pClr[2] = tmp;
	// convert color if curent DC is 16-bit (5:6:5) or 15-bit (5:5:5)
	if (bih.biBitCount == 16)
	{
		// for 16 bit
		color = ((DWORD)(pClr[0] & 0xf8) >> 3) |
			((DWORD)(pClr[1] & 0xfc) << 3) |
			((DWORD)(pClr[2] & 0xf8) << 8);
		// for 15 bit
//		color = ((DWORD)(pClr[0] & 0xf8) >> 3) |
//				((DWORD)(pClr[1] & 0xf8) << 2) |
//				((DWORD)(pClr[2] & 0xf8) << 7);
	}

	const DWORD RGNDATAHEADER_SIZE = sizeof(RGNDATAHEADER);
	const DWORD ADD_RECTS_COUNT = 40;			// number of rects to be appended
													// to region data buffer

	// BitPerPixel
	BYTE	Bpp = bih.biBitCount >> 3;				// bytes per pixel
	// bytes per line in pBits is DWORD aligned and bmp.bmWidthBytes is WORD aligned
	// so, both of them not
	DWORD m_dwAlignedWidthBytes = (bmp.bmWidthBytes & ~0x3) + (!!(bmp.bmWidthBytes & 0x3) << 2);
	// DIB image is flipped that's why we scan it from the last line
	LPBYTE	pColor = pBits + (bih.biHeight - 1) * m_dwAlignedWidthBytes;
	DWORD	dwLineBackLen = m_dwAlignedWidthBytes + bih.biWidth * Bpp;	// offset of previous scan line
													// (after processing of current)
	DWORD	dwRectsCount = bih.biHeight;			// number of rects in allocated buffer
	INT		i, j;									// current position in mask image
	INT		first = 0;								// left position of current scan line
													// where mask was found
	bool	wasfirst = false;						// set when mask has been found in current scan line
	bool	ismask;									// set when current color is mask color


	// allocate memory for region data
	// region data here is set of regions that are rectangles with height 1 pixel (scan line)
	// that's why first allocation is <bm.biHeight> RECTs - number of scan lines in image
	RGNDATAHEADER* pRgnData =
		(RGNDATAHEADER*)new BYTE[RGNDATAHEADER_SIZE + dwRectsCount * sizeof(RECT)];
	// get pointer to RECT table
	LPRECT pRects = (LPRECT)((LPBYTE)pRgnData + RGNDATAHEADER_SIZE);
	// zero region data header memory (header  part only)
	memset(pRgnData, 0, RGNDATAHEADER_SIZE + dwRectsCount * sizeof(RECT));
	// fill it by default
	pRgnData->dwSize = RGNDATAHEADER_SIZE;
	pRgnData->iType = RDH_RECTANGLES;

	for (i = 0; i < bih.biHeight; i++)
	{
		for (j = 0; j < bih.biWidth; j++)
		{
			// get color
			switch (bih.biBitCount)
			{
			case 8:
				ismask = (clr_tbl[*pColor] != color);
				break;
			case 16:
				ismask = (*(LPWORD)pColor != (WORD)color);
				break;
			case 24:
				ismask = ((*(LPDWORD)pColor & 0x00ffffff) != color);
				break;
			case 32:
				ismask = (*(LPDWORD)pColor != color);
			}
			// shift pointer to next color
			pColor += Bpp;
			// place part of scan line as RECT region if transparent color found after mask color or
			// mask color found at the end of mask image
			if (wasfirst)
			{
				if (!ismask)
				{
					// save current RECT
					RECT rc;
					rc.left = first;
					rc.top = i;
					rc.right = j;
					rc.bottom = i + 1;
					pRects[pRgnData->nCount++] = rc;    // Modified line to use RECT instead of CRect
					// if buffer full reallocate it with more room
					if (pRgnData->nCount >= dwRectsCount)
					{
						dwRectsCount += ADD_RECTS_COUNT;
						// allocate new buffer
						LPBYTE pRgnDataNew = new BYTE[RGNDATAHEADER_SIZE + dwRectsCount * sizeof(RECT)];
						// copy current region data to it
						memcpy(pRgnDataNew, pRgnData, RGNDATAHEADER_SIZE + pRgnData->nCount * sizeof(RECT));
						// delte old region data buffer
						delete pRgnData;
						// set pointer to new regiondata buffer to current
						pRgnData = (RGNDATAHEADER*)pRgnDataNew;
						// correct pointer to RECT table
						pRects = (LPRECT)((LPBYTE)pRgnData + RGNDATAHEADER_SIZE);
					}
					wasfirst = false;
				}
			}
			else if (ismask)		// set wasfirst when mask is found
			{
				first = j;
				wasfirst = true;
			}
		}

		if (wasfirst && ismask)
		{
			// save current RECT
			RECT rc;
			rc.left = first;
			rc.top = i;
			rc.right = j;
			rc.bottom = i + 1;
			pRects[pRgnData->nCount++] = rc;    // Modified line to use RECT instead of CRect
			// if buffer full reallocate it with more room
			if (pRgnData->nCount >= dwRectsCount)
			{
				dwRectsCount += ADD_RECTS_COUNT;
				// allocate new buffer
				LPBYTE pRgnDataNew = new BYTE[RGNDATAHEADER_SIZE + dwRectsCount * sizeof(RECT)];
				// copy current region data to it
				memcpy(pRgnDataNew, pRgnData, RGNDATAHEADER_SIZE + pRgnData->nCount * sizeof(RECT));
				// delte old region data buffer
				delete pRgnData;
				// set pointer to new regiondata buffer to current
				pRgnData = (RGNDATAHEADER*)pRgnDataNew;
				// correct pointer to RECT table
				pRects = (LPRECT)((LPBYTE)pRgnData + RGNDATAHEADER_SIZE);
			}
			wasfirst = false;
		}

		pColor -= dwLineBackLen;
	}
	// release image data
	delete pBits;
	delete bi;

	HRGN hVisibleRegion;

	// create region
	hVisibleRegion = ExtCreateRegion(NULL, RGNDATAHEADER_SIZE + pRgnData->nCount * sizeof(RECT), (LPRGNDATA)pRgnData);
	// release region data
	delete pRgnData;
	return hVisibleRegion;
}

LRESULT CALLBACK Btn_Close_Proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	HDC hMemDC;
	HBITMAP hBmp;
	RECT rect;
	TRACKMOUSEEVENT tme;
	hdc = GetDC(hwnd);
	GetClientRect(hwnd, &rect);
	switch (uMsg)
	{
	case WM_CREATE:
	{
		break;
	}

	case WM_PAINT:
	{
		// 关闭按钮窗口显示时从资源中加载位图作为按钮图片
		MoveWindow(hwnd, 313, 5, 12, 12, TRUE);
		InvalidateRect(hwnd, &rect, TRUE);
		hdc = BeginPaint(hwnd, &ps);
		hMemDC = CreateCompatibleDC(hdc);
		hBmp = LoadBitmap((HINSTANCE)Btn_Instance, MAKEINTRESOURCE(IDB_BITMAP2));

		HRGN hRgn = SetVisibleRgnFromBitmap(hBmp, RGB(255, 0, 0));	//设置透明色为红色
		SetWindowRgn(hwnd, hRgn, FALSE);

		SelectObject(hMemDC, hBmp);
		TransparentBlt(hdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top, hMemDC, 0, 0, 12, 12, RGB(0, 0, 0));
		BitBlt(hdc, 0, 0, 12, 12, hMemDC, 0, 0, SRCCOPY);
		DeleteObject(hMemDC);
		DeleteDC(hdc);
		EndPaint(hwnd, &ps);
		//ShowWindow(hwnd, SW_SHOW);
		break;
	}

	case WM_LBUTTONDOWN:   // 响应鼠标点击事件，向主窗口发送退出消息
	{
		PostQuitMessage(0);
		break;
	}

	case WM_MOUSEMOVE:		// 窗口默认不响应 WM_MOUSELEAVE 和 WM_MOUSEHOVER 消息，所以要使用 TrackMouseEvent 函数来激活这两个消息
	{
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_LEAVE | TME_HOVER;
		tme.hwndTrack = hwnd;
		tme.dwHoverTime = 10; // 自定义悬停时间
		TrackMouseEvent(&tme);
		break;
	}
	case WM_MOUSEHOVER:		// 鼠标悬停hover特效
	{

		// 鼠标移上来时切换位图的方法展示hover效果
		MoveWindow(hwnd, 313, 5, 12, 12, TRUE);
		InvalidateRect(hwnd, &rect, TRUE);
		hdc = BeginPaint(hwnd, &ps);
		hMemDC = CreateCompatibleDC(hdc);
		hBmp = LoadBitmap((HINSTANCE)Btn_Instance, MAKEINTRESOURCE(IDB_BITMAP1));
		HRGN hRgn = SetVisibleRgnFromBitmap(hBmp, RGB(255, 0, 0));
		SetWindowRgn(hwnd, hRgn, FALSE);

		SelectObject(hMemDC, hBmp);
		TransparentBlt(hdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top, hMemDC, 0, 0, 12, 12, RGB(0, 0, 0));
		BitBlt(hdc, 0, 0, 12, 12, hMemDC, 0, 0, SRCCOPY);
		DeleteObject(hMemDC);
		DeleteDC(hdc);
		EndPaint(hwnd, &ps);
		ShowWindow(hwnd, SW_SHOW);
		UpdateWindow(hwnd);
		break;
	}

	case WM_MOUSELEAVE:		// 鼠标移走
	{
		MoveWindow(hwnd, 313, 5, 12, 12, TRUE);
		InvalidateRect(hwnd, &rect, TRUE);
		hdc = BeginPaint(hwnd, &ps);
		hMemDC = CreateCompatibleDC(hdc);
		hBmp = LoadBitmap((HINSTANCE)Btn_Instance, MAKEINTRESOURCE(IDB_BITMAP2));
		//HRGN hRgn = SetVisibleRgnFromBitmap(hBmp, RGB(255, 0, 0));
		//SetWindowRgn(hwnd, hRgn, FALSE);

		SelectObject(hMemDC, hBmp);
		TransparentBlt(hdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top, hMemDC, 0, 0, 12, 12, RGB(0, 0, 0));
		BitBlt(hdc, 0, 0, 12, 12, hMemDC, 0, 0, SRCCOPY);
		DeleteObject(hMemDC);
		DeleteDC(hdc);
		EndPaint(hwnd, &ps);
		ShowWindow(hwnd, SW_SHOW);
		UpdateWindow(hwnd);
		break;
	}
	case WM_DESTROY:
	{
		if (!WindowCount) PostQuitMessage(0);
		return(TRUE);
	}
	}

	return(DefWindowProc(hwnd, uMsg, wParam, lParam));
}

LRESULT CALLBACK Btn_Text_Proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	HDC hMemDC;
	HBITMAP hBmp;
	RECT rect;
	TRACKMOUSEEVENT tme;
	hdc = GetDC(hwnd);
	GetClientRect(hwnd, &rect);
	switch (uMsg)
	{
	case WM_CREATE:
	{
		break;
	}

	case WM_PAINT:
	{
		// 关闭按钮窗口显示时从资源中加载位图作为按钮图片
		//SetLayeredWindowAttributes(hBtnClose, RGB(0, 0, 0), 0, LWA_ALPHA | LWA_COLORKEY);
		MoveWindow(hwnd, 234, 7, 61, 13, TRUE);
		InvalidateRect(hwnd, &rect, TRUE);
		hdc = BeginPaint(hwnd, &ps);
		hMemDC = CreateCompatibleDC(hdc);
		hBmp = LoadBitmap((HINSTANCE)Btn_Instance, MAKEINTRESOURCE(IDB_BITMAP3));

		HRGN hRgn = SetVisibleRgnFromBitmap(hBmp, RGB(240, 240, 240));
		SetWindowRgn(hwnd, hRgn, FALSE);

		SelectObject(hMemDC, hBmp);
		TransparentBlt(hdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top, hMemDC, 0, 0, 61, 13, RGB(0, 0, 0));
		BitBlt(hdc, 0, 0, 61, 13, hMemDC, 0, 0, SRCCOPY);
		DeleteObject(hMemDC);
		DeleteDC(hdc);
		EndPaint(hwnd, &ps);
		//ShowWindow(hwnd, SW_SHOW);
		UpdateWindow(hwnd);
		break;
	}

	case WM_LBUTTONDOWN:   // 响应鼠标点击事件，向主窗口发送退出消息
	{
		PostQuitMessage(0);
		break;
	}

	case WM_MOUSEMOVE:		// 窗口默认不响应 WM_MOUSELEAVE 和 WM_MOUSEHOVER 消息，所以要使用 TrackMouseEvent 函数来激活这两个消息
	{
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_LEAVE | TME_HOVER;
		tme.hwndTrack = hwnd;
		tme.dwHoverTime = 10; // 自定义悬停时间
		TrackMouseEvent(&tme);
		break;
	}

	case WM_MOUSEHOVER:		// 鼠标悬停hover特效
	{
		MoveWindow(hwnd, 234, 7, 61, 13, TRUE);
		InvalidateRect(hwnd, &rect, TRUE);
		hdc = BeginPaint(hwnd, &ps);
		hMemDC = CreateCompatibleDC(hdc);
		hBmp = LoadBitmap((HINSTANCE)Btn_Instance, MAKEINTRESOURCE(IDB_BITMAP4));

		HRGN hRgn = SetVisibleRgnFromBitmap(hBmp, RGB(255, 0, 0));
		SetWindowRgn(hwnd, hRgn, FALSE);
		
		SelectObject(hMemDC, hBmp);
		TransparentBlt(hdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top, hMemDC, 0, 0, 61, 13, RGB(0, 0, 0));
		BitBlt(hdc, 0, 0, 61, 13, hMemDC, 0, 0, SRCCOPY);
		DeleteObject(hMemDC);
		DeleteDC(hdc);
		EndPaint(hwnd, &ps);
		ShowWindow(hwnd, SW_SHOW);
		UpdateWindow(hwnd);
		break;

	}

	case WM_MOUSELEAVE:		// 鼠标移走
	{

		MoveWindow(hwnd, 234, 7, 61, 13, TRUE);
		InvalidateRect(hwnd, &rect, TRUE);
		hdc = BeginPaint(hwnd, &ps);

		hMemDC = CreateCompatibleDC(hdc);
		//hBmp = LoadBitmap((HINSTANCE)Btn_Instance, MAKEINTRESOURCE(IDB_BITMAP5));

		hBmp = LoadBitmap((HINSTANCE)Btn_Instance, MAKEINTRESOURCE(IDB_BITMAP3));
		//HRGN hRgn = SetVisibleRgnFromBitmap(hBmp, RGB(255, 0, 0));
		//SetWindowRgn(hwnd, hRgn, FALSE);
		HRGN hRgn1 = SetVisibleRgnFromBitmap(hBmp, RGB(240, 240, 240));
		SetWindowRgn(hwnd, hRgn1, FALSE);

		SelectObject(hMemDC, hBmp);
		TransparentBlt(hdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top, hMemDC, 0, 0, 61, 13, RGB(0, 0, 0));
		BitBlt(hdc, 0, 0, 61, 13, hMemDC, 0, 0, SRCCOPY);
		DeleteObject(hMemDC);
		DeleteDC(hdc);
		EndPaint(hwnd, &ps);
		ShowWindow(hwnd, SW_SHOW);
		UpdateWindow(hwnd);
		break;

	}
	case WM_CTLCOLORSTATIC:
		SetBkMode((HDC)wParam, TRANSPARENT);


	case WM_DESTROY:
	{
		if (!WindowCount) PostQuitMessage(0);
		return(TRUE);
	}
	}

	return(DefWindowProc(hwnd, uMsg, wParam, lParam));
}

LRESULT CALLBACK MainWindProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	HRGN rgn;
	switch (message)
	{
	case WM_CREATE:
	{
		GetClientRect(hwnd, &rect); // 先获取窗口区域
		rgn = CreateRoundRectRgn(rect.left, rect.top, rect.right, rect.bottom, 4, 4);// 在该区域创建圆形区域，宽高圆角4pix
		SetWindowRgn(hwnd, rgn, TRUE);

		Btn_Instance = ((LPCREATESTRUCT)lParam)->hInstance;

		// 创建自绘的关闭按钮（窗口）
		hBtnClose = CreateWindow(L"Button", L"button", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | WS_CLIPSIBLINGS | WS_EX_LAYERED,
			313, 5, 12, 12, hwnd, (HMENU)IDB_CLOSE, Btn_Instance, NULL);
		SetWindowLong(hBtnClose, GWL_WNDPROC, (long)Btn_Close_Proc);		// 窗口子类化
		//SetWindowPos(hBtnClose, HWND_TOP, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);

		// 创建自绘的近期不显示（窗口）
		hBtnText = CreateWindow(L"Button", L"text", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | WS_CLIPSIBLINGS | WS_EX_LAYERED,
			234, 7, 61, 13, hwnd, (HMENU)IDB_CLOSE_HIDE, Btn_Instance, NULL);
		SetWindowLong(hBtnText, GWL_WNDPROC, (long)Btn_Text_Proc);
		//SetWindowPos(hBtnText, HWND_TOP, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);


		//设置定时器3分钟自动关闭窗口
		SetTimer(hwnd, 1, 1000 * 60 * 3, NULL);
		return 0;
	}
	case WM_TIMER:
	{
		KillTimer(hwnd, 1);
		user_message.action = "0";
		DestroyWindow(hwnd);
	}

	case WM_LBUTTONDOWN:
	{
		POINT mousePos;
		GetCursorPos(&mousePos);
		ScreenToClient(hwnd, &mousePos);
		//点击的区域为退出按钮的区域
		if (mousePos.x >= POPUP_WINDOW_WIDTH - 22 && mousePos.x <= POPUP_WINDOW_WIDTH - 10 && mousePos.y >= 0 && mousePos.y <= 22)
		{
			//在这里记录用户点击的是退出
			user_message.action = "0";
			DestroyWindow(hwnd);
		}

		else
		{
			if (mousePos.x >= 234 && mousePos.x <= 295 && mousePos.y >= 7 && mousePos.y <= 20)
			{
				//在这里记录用户点击的是不感兴趣
				user_message.action = "2";
				DestroyWindow(hwnd);
			}
			//这里记录的是进行弹出链接或者打开客户端的操作
			else
			{
				//判断相应事件响应类型
				user_message.action = "1";
				if (1 == atoi(popup_message.popout_data.type.c_str()))
				{
					//鼠标点击事件打开客户端操作
					DLOG("MouseClick");
					OpenZhwClient();
					DestroyWindow(hwnd);
				}
				else
				{
					//鼠标点击事件为打开链接操作
					if (2 == atoi(popup_message.popout_data.type.c_str()))
					{
						//字符串转换string转LPCTSTR
						size_t size = popup_message.popout_data.link.length();
						wchar_t* buffer = new wchar_t[size + 1];
						MultiByteToWideChar(CP_ACP, 0, popup_message.popout_data.link.c_str(), size, buffer, size * sizeof(wchar_t));
						buffer[size] = 0;
						//打开浏览器连接
						ShellExecute(0, NULL, buffer, NULL, NULL, SW_NORMAL);
						delete buffer;
						DestroyWindow(hwnd);
					}
				}
			}
		}
		DestroyWindow(hwnd);
	}

	case WM_SETCURSOR:
	{
		//鼠标移动到对话框上换成小手手
		if (IsWindowVisible(hwnd))
		{
			POINT p;
			//先获取鼠标相对于屏幕的坐标
			GetCursorPos(&p);
			ScreenToClient(hwnd, &p);

			//最后把当前鼠标的坐标转化为相对于rect的坐标
			//ScreenToClient(hwnd, &p);
			if (p.x > 0 && p.x < POPUP_WINDOW_WIDTH && p.y > 0 && p.y < POPUP_WINDOW_HEIHT)
			{
				SetCursor(LoadCursor(NULL, IDC_HAND));
				return TRUE;
			}
			else
			{
				SetCursor(LoadCursor(NULL, IDC_ARROW));
				return TRUE;
			}
		}
		break;
	}
	case WM_DESTROY:
		PostUserMessage();
		WriteOperationToReg(user_message);
		OutputDebugString(_T("[project1]WM_QUIT :PostUserMessage"));
		PostQuitMessage(0);
		return 0;
	}
	return ::DefWindowProc(hwnd, message, wParam, lParam);
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hInstNULL, LPSTR lpszCmdLine, int nCmdShow)
{
	//DLOG("Enter WinMain");
	DbgInfo(L"[project1]%d winmain", __LINE__);
	TCHAR logstring[MAX_PATH] = _T("[project1]");
	CHAR logcharstring[MAX_PATH] = "[project1]";
	WNDCLASSEX wndClass;
	wndClass.cbSize = sizeof(wndClass);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = MainWindProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = ::LoadIcon(nullptr, IDI_APPLICATION);
	wndClass.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)::GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName = nullptr;
	wndClass.lpszClassName = TEXT("PopupWindowClass");
	wndClass.hIconSm = nullptr;

	// 注册窗口类
	::RegisterClassEx(&wndClass);

	//获取桌面工作区大小
	RECT rt;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rt, 0);

	// 创建窗口
	HWND hwnd = ::CreateWindowEx(
		WS_EX_TOOLWINDOW,
		TEXT("PopupWindowClass"),
		TEXT("PopupWindow"),
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		rt.right - POPUP_WINDOW_WIDTH - 16,
		rt.bottom - POPUP_WINDOW_HEIHT - 6,
		POPUP_WINDOW_WIDTH,
		POPUP_WINDOW_HEIHT,
		NULL,
		NULL,
		hInstance,
		NULL);
	if (hwnd == nullptr)
	{
		return -1;
	}

	//设置窗口置顶
	::SetWindowPos(hwnd,
		HWND_TOPMOST,
		rt.right - POPUP_WINDOW_WIDTH - 16,
		rt.bottom - POPUP_WINDOW_HEIHT - 6,
		POPUP_WINDOW_WIDTH,
		POPUP_WINDOW_HEIHT,
		SWP_NOACTIVATE | SWP_HIDEWINDOW);

	HDC hdc = GetDC(hwnd);
	RECT rect = { 0 };
	GetClientRect(hwnd, &rect);

	TCHAR szPath[MAX_PATH] = _T("");				//所有文件保存的路径
	TCHAR szJsonFile[MAX_PATH] = _T("");		//Json文件路径
	TCHAR szPicturePath[MAX_PATH] = _T(""); //图片文件路径
	std::string picture_name;								//图片名

	if (S_OK == SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, 0, szPath))
	{
		lstrcat(szPath, _T("\\service_database"));
		lstrcat(szJsonFile, szPath);
		lstrcat(szJsonFile, _T("\\popup_info.json"));

		OutputDebugString(_T("[project1]prepare read json file"));
		OutputDebugString(szJsonFile);
		//读取json文件
		if (0 == ReadPopupJsonMessage(popup_message, szJsonFile))
		{
			OutputDebugString(_T("[project1]read json file failed"));
			return -1;
		}
		else
		{
			OutputDebugString(_T("[project1]read json file done"));
			//下载图片
			picture_name = DownLoadPircture(popup_message, szPath);
			if (picture_name.empty())
			{
				DbgInfoA("[project1]DownLoadPircture failed, err = %d", GetLastError());
				return -1;
			}
			else
			{
				OutputDebugStringA("[project1]download pic done");
			}
		}
	}
	else
	{
		OutputDebugString(_T("[project1]SHGetFolderPath failed"));
		OutputDebugString(szPath);
		return -1;
	}

	//******读取图片文件******//
	int pos = picture_name.find_last_of(".");
	std::string picture_type = picture_name.substr(pos + 1);

	lstrcatA(logcharstring, picture_name.c_str());
	OutputDebugStringA(logcharstring);
	//判断文件类型
	CxImage img;
	//字符转转换
	_stprintf_s(szPicturePath, MAX_PATH, _T("%S"), picture_name.c_str());
	lstrcat(logstring, szPicturePath);
	OutputDebugString(logstring);
	img.Load(szPicturePath);
	if (0 == picture_type.compare("gif"))
	{
		std::thread([&]() {
			{
				while (true)
				{
					for (int i = 0; i < img.GetNumFrames(); i++)
					{
						CxImage img_1;
						img_1.SetFrame(i);
						img_1.Load(szPicturePath, CXIMAGE_FORMAT_GIF);
						img_1.Draw(hdc, rect);
						Sleep(1000 / 35);
					}
					ShowWindow(hwnd, SW_SHOW);
					UpdateWindow(hwnd);
				}
			}
			}).detach();
	}
	else if (0 == picture_type.compare("png") || 0 == picture_type.compare("jpg") || 0 == picture_type.compare("jpeg"))
	{
		//CxImage img;
		//_stprintf_s(szPicturePath, MAX_PATH, _T("%S"), picture_name.c_str());
		UpdateWindow(hwnd);
		ShowWindow(hwnd, nCmdShow);
		lstrcat(logstring, szPicturePath);
		OutputDebugString(logstring);

		//重绘多次,防止Button绘制镂空情况
		for (int i = 0; i < 10; i++)
		{
			img.Draw(hdc, rect);
			UpdateWindow(hwnd);
			Sleep(0);
		}

	}
	else
	{
		return -1;
	}

	//获取设备信息
	wchar_t* wcMac = GetMacAddress();
	user_message.device = ws2s(wcMac);
	user_message.id = popup_message.popout_data.id;
	user_message.system = GetSysVersion();
	if (!GetUserId(user_message.user_id))
	{
		user_message.user_id = "0";
	}



	//// 刷新窗口客户区
	//::UpdateWindow(hwnd);

	////设置窗口浮现样式
	//AnimateWindow(hwnd, 800, AW_SLIDE | AW_VER_NEGATIVE | AW_ACTIVATE);

	MSG msg;
	while (::GetMessage(&msg, nullptr, 0, 0))
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}
	return msg.wParam;
}

