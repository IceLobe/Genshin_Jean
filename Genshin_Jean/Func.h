#pragma once

#include <Windows.h>

// 字符串
namespace Str
{
	// Description: wchar_t * 转 char * 默认字符集为UTF-8
	// Access:      public 
	// Parameter:   const wchar_t * lpWideCharStr 
	// Parameter:   unsigned int CodePage 默认字符集为UTF-8
	// Returns:     char * 多字节字符串
	// Author:      IceLeaf
	std::string w2c(const wchar_t *lpWideCharStr, unsigned int CodePage = CP_UTF8);

	// Description: 全平台通用，宽字节转多字节(默认当前系统区域字符集)
	// Access:      public 
	// Parameter:   const wchar_t * lpWideCharStr 宽字节字符串
	// Returns:     char * 多字节字符串
	// Author:      IceLeaf
	char *ws2s(const wchar_t *lpWideCharStr);

	// Description: char * 转 wchar_t * 默认字符集为UTF-8
	// Access:      public 
	// Parameter:   std::wstring 
	// Parameter:   unsigned int CodePage 默认代码页为UTF-8
	// Returns:     wchar_t * 宽字节字符串
	// Author:      IceLeaf
	std::wstring c2w(const char *lpMultiByteStr, unsigned int CodePage = CP_UTF8);

	// Description: 全平台通用，多字节转宽字节(默认当前系统区域字符集)
	// Access:      public 
	// Parameter:   const char * str 多字节字符串
	// Returns:     wchar_t * 宽字节字符串
	// Author:      IceLeaf
	wchar_t *s2ws(const char *lpMultiByteStr);
}
