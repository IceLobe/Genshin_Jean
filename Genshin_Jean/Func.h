#pragma once

#include <Windows.h>

// �ַ���
namespace Str
{
	// Description: wchar_t * ת char * Ĭ���ַ���ΪUTF-8
	// Access:      public 
	// Parameter:   const wchar_t * lpWideCharStr 
	// Parameter:   unsigned int CodePage Ĭ���ַ���ΪUTF-8
	// Returns:     char * ���ֽ��ַ���
	// Author:      IceLeaf
	std::string w2c(const wchar_t *lpWideCharStr, unsigned int CodePage = CP_UTF8);

	// Description: ȫƽ̨ͨ�ã����ֽ�ת���ֽ�(Ĭ�ϵ�ǰϵͳ�����ַ���)
	// Access:      public 
	// Parameter:   const wchar_t * lpWideCharStr ���ֽ��ַ���
	// Returns:     char * ���ֽ��ַ���
	// Author:      IceLeaf
	char *ws2s(const wchar_t *lpWideCharStr);

	// Description: char * ת wchar_t * Ĭ���ַ���ΪUTF-8
	// Access:      public 
	// Parameter:   std::wstring 
	// Parameter:   unsigned int CodePage Ĭ�ϴ���ҳΪUTF-8
	// Returns:     wchar_t * ���ֽ��ַ���
	// Author:      IceLeaf
	std::wstring c2w(const char *lpMultiByteStr, unsigned int CodePage = CP_UTF8);

	// Description: ȫƽ̨ͨ�ã����ֽ�ת���ֽ�(Ĭ�ϵ�ǰϵͳ�����ַ���)
	// Access:      public 
	// Parameter:   const char * str ���ֽ��ַ���
	// Returns:     wchar_t * ���ֽ��ַ���
	// Author:      IceLeaf
	wchar_t *s2ws(const char *lpMultiByteStr);
}
