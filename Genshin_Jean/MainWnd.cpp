#include "MainWnd.h"
#include "resource.h"
#include <string>
#include <DbgHelp.h>
#include <Shlwapi.h>
#include <chrono>
#include "Func.h"

#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Dbghelp.lib")

MainWnd::MainWnd(TCHAR* lpszClassName) : m_lpszClassName(lpszClassName) {

}

MainWnd::~MainWnd() {
}

void MainWnd::InitWindow() {
	m_pCmbMidFile = static_cast<CComboUI*>(m_PaintManager.FindControl(_T("cmbMidFile")));

	m_pBtnPlay = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnPlay")));
	m_pBtnStop = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnStop")));
	m_pBtnPrevious = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnPrevious")));
	m_pBtnNext = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnNext")));

	char lpFilename[_MAX_FNAME];
	::GetModuleFileNameA(NULL, lpFilename, _MAX_FNAME);
	std::string filename(lpFilename);
	size_t uPos = filename.rfind('\\');
	if (uPos != std::string::npos) {
		filename = filename.substr(0, uPos + 1) + "Midi\\";
	}
	if (!PathFileExistsA(filename.c_str())) {
		::MakeSureDirectoryPathExists(filename.c_str());
	}
	std::string pathName = filename + "*.mid";
	WIN32_FIND_DATAA lpFFData;
	HANDLE hFindFile = FindFirstFileA(pathName.c_str(), &lpFFData);
	if (INVALID_HANDLE_VALUE != hFindFile) {
		do {
			CListLabelElementUI* pListLabel = new CListLabelElementUI;
			pListLabel->SetText(Str::c2w(lpFFData.cFileName, CP_ACP).c_str());
			pListLabel->SetUserData(Str::c2w((filename + lpFFData.cFileName).c_str(), CP_ACP).c_str());
			m_pCmbMidFile->Add(pListLabel);
		} while (FindNextFileA(hFindFile, &lpFFData));
		m_pCmbMidFile->SelectItem(0);
	}
	m_hWndGame = ::FindWindow(TEXT("UnityWndClass"), TEXT("原神"));
	if (m_hWndGame == 0)
		return;
}

void MainWnd::OnFinalMessage(HWND hWnd) {
	__super::OnFinalMessage(hWnd);
	delete this;
}

void MainWnd::Notify(TNotifyUI& msg) {
	if (msg.sType == DUI_MSGTYPE_CLICK) {
		if (msg.pSender == m_pBtnPlay) {
			bQuit = false;
			//int nStyle = ::GetWindowLong(hWnd, GWL_STYLE);
			//nStyle &= ~(WS_MINIMIZEBOX);
			//SetWindowLong(hWnd, GWL_STYLE, nStyle);//废掉最小化按钮
			::ShowWindow(m_hWndGame, SW_SHOWMAXIMIZED);
			::SetForegroundWindow(m_hWndGame);
			m_uThreadCount = g_vecMapKey.size();
			if (m_uThreadCount == 0)
				return;
			m_pThread = new std::thread[m_uThreadCount];
			::memset(m_pThread, 0, m_uThreadCount * sizeof(std::thread));
			Sleep(1000);
			for (size_t i = 0; i < m_uThreadCount; i++) {
				m_pThread[i] = std::thread(std::bind(&MainWnd::ThreadFunc, this, std::placeholders::_1), g_vecMapKey.at(i));
				m_pThread[i].detach();
				//m_hThread[i] = CreateThread(NULL, 0, ThreadFunc, &g_vecMapKey.at(i), 0, NULL);
			}
			m_uThreadCount = 0;
		}
		else if (msg.pSender == m_pBtnStop) {
			bQuit = true;
			::ShowWindow(m_hWndGame, SW_SHOWMAXIMIZED);
			::SetForegroundWindow(m_hWndGame);
		}
		else if (msg.pSender == m_pBtnPrevious) {
			bQuit = true;
			Sleep(100);
			int nCurSel = m_pCmbMidFile->GetCurSel();
			if ((--nCurSel) >= 0) {
				m_pCmbMidFile->SelectItem(nCurSel);
				m_PaintManager.SendNotify(m_pBtnPlay, DUI_MSGTYPE_CLICK);
			}
		}
		else if (msg.pSender == m_pBtnNext) {
			bQuit = true;
			Sleep(100);
			int nCurSel = m_pCmbMidFile->GetCurSel();
			if ((++nCurSel) < m_pCmbMidFile->GetCount()) {
				m_pCmbMidFile->SelectItem(nCurSel);
				m_PaintManager.SendNotify(m_pBtnPlay, DUI_MSGTYPE_CLICK);
			}
		}
	}
	else if (msg.sType == DUI_MSGTYPE_ITEMSELECT) {
		g_vecMapKey.clear();
		std::wstring wsFilepath = m_pCmbMidFile->GetItemAt(m_pCmbMidFile->GetCurSel())->GetUserData().GetData();
		if (!midi::Get().ReadMidiFile(Str::w2c(wsFilepath.c_str(), CP_ACP).c_str()))
			//if (!midi::Get().ReadMidiFile("稻香.mid"))
			//if (!midi::Get().ReadMidiFile("Unravel.mid"))
			return; // 读取文件错误
		division = midi::Get().GetHeaderData().division;
		nQuarterNote = midi::Get().GetQuarterNote();
		g_vecMapKey = midi::Get().GetMapKeyTable();
	}
	__super::Notify(msg);
}

DuiLib::UILIB_RESOURCETYPE MainWnd::GetResourceType() const {
	return UILIB_RESOURCE;
}

LRESULT MainWnd::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	if (uMsg == WM_CLOSE) {
		bQuit = true;
		::ShowWindow(m_hWndGame, SW_SHOWMAXIMIZED);
		::SetForegroundWindow(m_hWndGame);
		::PostQuitMessage(0);
		return 0;
	}
	return __super::HandleCustomMessage(uMsg, wParam, lParam, bHandled);
}

DuiLib::CDuiString MainWnd::GetSkinFolder() {
	return _T("");
}

DuiLib::CDuiString MainWnd::GetSkinFile() {
	TCHAR szBuf[MAX_PATH] = { 0 };
	_stprintf_s(szBuf, MAX_PATH - 1, _T("%d"), IDR_SKINXML);
	return szBuf;
}

LPCTSTR MainWnd::GetWindowClassName(void) const {
	return m_lpszClassName;
}

void MainWnd::usleep(uint64_t microseconds) {
	std::chrono::steady_clock::time_point time_begin = std::chrono::steady_clock::now();
	while (!bQuit) {
		std::chrono::steady_clock::time_point time_end = std::chrono::steady_clock::now();
		uint64_t used = std::chrono::duration_cast<std::chrono::microseconds>(time_end - time_begin).count();
		if (used > microseconds)
			return;
	}
	return;
}

void MainWnd::ThreadFunc(std::vector<Mapping_Key> vecMapkey) {
	long tick = 0;
	float countsleep = 0.f;
	for (auto var : vecMapkey) {
		if (bQuit)
			break;
		int midi_code = 0;
		//if (var.midi_note_code < 48)
		//	midi_code = (var.midi_note_code % 12 + 48);
		//else if (83 < var.midi_note_code)
		//	midi_code = var.midi_note_code % 12 + 72;
		//else
		midi_code = var.midi_note_code;
		auto it = g_mapDefault.find(midi_code);
		if (it != g_mapDefault.end()) {
			if (var.isDwon)
				usleep((uint64_t)((float)var.time_tick / (float)division * (float)nQuarterNote));
			else
				usleep(var.time_tick);
			keybd_event(it->second, 0, var.isDwon ? KEYEVENTF_EXTENDEDKEY : KEYEVENTF_KEYUP, 0);
			//printf_s(var.isDwon ? "按下%c\t" : "谈起%c\t", it->second);
			if (!var.isDwon) {
				float fSleep = (float)var.time_tick / (float)division * (float)nQuarterNote;
				//printf_s("延时:%lf\n", fSleep);
				countsleep += fSleep;
				usleep((uint64_t)fSleep);
			}
		}
	}
}
