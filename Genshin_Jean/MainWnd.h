#pragma once

#include "framework.h"
#include <map>
#include <thread>
#include <functional>
#include "midi.h"

class MainWnd : public WindowImplBase {
public:
	MainWnd(TCHAR* lpszClassName);
	~MainWnd();

	virtual void InitWindow();

	virtual void OnFinalMessage(HWND hWnd);

	virtual void Notify(TNotifyUI& msg);

	virtual UILIB_RESOURCETYPE GetResourceType() const;

	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

protected:

	virtual CDuiString GetSkinFolder();

	virtual CDuiString GetSkinFile();

	virtual LPCTSTR GetWindowClassName(void) const;

	void usleep(uint64_t microseconds);

	void ThreadFunc(std::vector<Mapping_Key> vecMapkey);

private:
	CComboUI* m_pCmbMidFile = 0;

	CButtonUI* m_pBtnPlay = 0;
	CButtonUI* m_pBtnStop = 0;
	CButtonUI* m_pBtnPrevious = 0;
	CButtonUI* m_pBtnNext = 0;

	unsigned short division = 0;
	unsigned int nQuarterNote = 0;

	bool bQuit = false;

	TCHAR* m_lpszClassName = 0;

	std::vector<std::vector<Mapping_Key>> g_vecMapKey;

	std::thread* m_pThread = 0;

	HWND m_hWndGame = 0;
	size_t m_uThreadCount = 0;

	std::map<int, char> g_mapDefault{
	{72,'Q'},{73,'Q'},{74,'W'},{75,'W'}, {76,'E'}, {77,'R'},{78,'R'}, {79,'T'},{80,'T'},{81,'Y'},{82,'Y'},{83,'U'}, // high
	{60,'A'},{61,'A'},{62,'S'},{63,'S'}, {64,'D'}, {65,'F'},{66,'F'}, {67,'G'},{68,'G'},{69,'H'},{70,'H'},{71,'J'}, // Ä¬ÈÏÖÐµ÷
	{48,'Z'},{49,'Z'},{50,'X'},{51,'X'}, {52,'C'}, {53,'V'},{54,'V'}, {55,'B'},{56,'B'},{57,'N'},{58,'N'},{59,'M'}  // low
	};
};

