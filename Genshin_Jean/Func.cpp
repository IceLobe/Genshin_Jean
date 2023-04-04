#include "framework.h"
#include <string>
#include <locale>
#include "Func.h"

std::string Str::w2c(const wchar_t* lpWideCharStr, unsigned int CodePage /*= CP_UTF8*/) {
	int cchWideChar = (int)wcslen(lpWideCharStr);
	int cbMultiByte = WideCharToMultiByte(CodePage, 0, lpWideCharStr, cchWideChar, 0, 0, 0, NULL) + 1;
	char* lpMultiByteStr = new char[cbMultiByte];
	RtlZeroMemory(lpMultiByteStr, cbMultiByte * sizeof(char));
	WideCharToMultiByte(CodePage, 0, lpWideCharStr, cchWideChar, lpMultiByteStr, cbMultiByte, 0, 0);
	std::string str = lpMultiByteStr;
	delete[] lpMultiByteStr;
	return str;
}

char* Str::ws2s(const wchar_t* lpWideCharStr) {
	char* strLocale = setlocale(LC_ALL, NULL);
	size_t cbMultiByte;
	wcstombs_s(&cbMultiByte, NULL, NULL, lpWideCharStr, wcslen(lpWideCharStr));
	std::unique_ptr<char[]> lpMultiByteStr = std::unique_ptr<char[]>();
	lpMultiByteStr.reset(new char[cbMultiByte + 1]);
	wcstombs_s(&cbMultiByte, lpMultiByteStr.get(), cbMultiByte, lpWideCharStr, _TRUNCATE);
	setlocale(LC_ALL, strLocale);
	return lpMultiByteStr.release();
}

std::wstring Str::c2w(const char* lpMultiByteStr, unsigned int CodePage /*= CP_UTF8*/) {
	int cbMultiByte = (int)strlen(lpMultiByteStr);
	int cchWideChar = MultiByteToWideChar(CodePage, NULL, lpMultiByteStr, cbMultiByte, NULL, NULL) + 1;
	wchar_t* pWideCharStr = new wchar_t[cchWideChar];
	RtlZeroMemory(pWideCharStr, cchWideChar * sizeof(wchar_t));
	MultiByteToWideChar(CodePage, NULL, lpMultiByteStr, cbMultiByte, pWideCharStr, cchWideChar);
	std::wstring wstr(pWideCharStr);
	delete[] pWideCharStr;
	return wstr;
}

wchar_t* Str::s2ws(const char* lpMultiByteStr) {
	char* strLocale = setlocale(LC_ALL, NULL);
	size_t cchWideChar;
	mbstowcs_s(&cchWideChar, NULL, NULL, lpMultiByteStr, strlen(lpMultiByteStr));
	std::unique_ptr<wchar_t[]> pWideCharStr = std::unique_ptr<wchar_t[]>();
	pWideCharStr.reset(new wchar_t[cchWideChar + 1]);
	mbstowcs_s(&cchWideChar, pWideCharStr.get(), cchWideChar, lpMultiByteStr, _TRUNCATE);
	setlocale(LC_ALL, strLocale);
	return pWideCharStr.release();
}
